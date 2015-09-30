/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "cswordmoduleinfo.h"

#ifndef BT_NO_LUCENE
#include <CLucene.h>
#endif
#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QScopedArrayPointer>
#include <QScopedPointer>
#include <QTextDocument>
#include "../../util/cresmgr.h"
#include "../../util/directory.h"
#include "../../util/htmlescape.h"
#include "../config/btconfig.h"
#include "../keys/cswordkey.h"
#include "../managers/clanguagemgr.h"
#include "../managers/cswordbackend.h"
#include "../rendering/centrydisplay.h"
#include "../cswordmodulesearch.h"
#include "cswordlexiconmoduleinfo.h"

// Sword includes:
#include <listkey.h>
#include <swbuf.h>
#include <swconfig.h>
#include <swkey.h>
#include <rtfhtml.h>
#include <versekey.h>


//Increment this, if the index format changes
//Then indices on the user's systems will be rebuilt
const unsigned int INDEX_VERSION = 7;

//Maximum index entry size, 1MiB for now
//Lucene default is too small
const unsigned long BT_MAX_LUCENE_FIELD_LENGTH = 1024 * 1024;

CSwordModuleInfo::CSwordModuleInfo(sword::SWModule * module,
                                   CSwordBackend & backend,
                                   ModuleType type)
    : m_module(module),
      m_backend(backend),
      m_type(type),
      m_cancelIndexing(false),
      m_cachedName(QString::fromUtf8(module->getName())),
      m_cachedHasVersion(!QString((*m_backend.getConfig())[module->getName()]["Version"]).isEmpty())
{
    Q_ASSERT(module);
    // Initialize m_cachedCategory:
    {
        /// \todo Maybe we can use raw string comparsion instead of QString?
        QString const cat(m_module->getConfigEntry("Category"));

        /// \warning cat has to be checked before m_type !!!
        if (cat == "Cults / Unorthodox / Questionable Material") {
            m_cachedCategory = Cult;
        } else if (cat == "Daily Devotional"
                   || m_module->getConfig().has("Feature","DailyDevotion"))
        {
            m_cachedCategory = DailyDevotional;
        } else if (cat == "Glossaries"
                   || m_module->getConfig().has("Feature", "Glossary"))
        {
            m_cachedCategory = Glossary;
        } else if (cat == "Images" || cat == "Maps") {
            m_cachedCategory = Images;
        } else {
            switch (m_type) {
                case Bible:       m_cachedCategory = Bibles; break;
                case Commentary:  m_cachedCategory = Commentaries; break;
                case Lexicon:     m_cachedCategory = Lexicons; break;
                case GenericBook: m_cachedCategory = Books; break;
                case Unknown: // Fall thru
                default:          m_cachedCategory = UnknownCategory; break;
            }
        }
    }

    // Initialize m_cachedLanguage:
    {
        CLanguageMgr const & lm = *CLanguageMgr::instance();
        if (m_cachedCategory == Glossary) {
            /*
              Special handling for glossaries, we use the "from language" as
              language for the module.
            */
            m_cachedLanguage = lm.languageForAbbrev(config(GlossaryFrom));
        } else {
            m_cachedLanguage = lm.languageForAbbrev(m_module->getLanguage());
        }
    }

    m_hidden = btConfig().value<QStringList>("state/hiddenModules",
                                             QStringList()).contains(m_cachedName);

    if (m_cachedHasVersion
        && (minimumSwordVersion() > sword::SWVersion::currentVersion))
    {
        qWarning("The module \"%s\" requires a newer Sword library. Please "
                 "update to \"Sword %s\".",
                 m_cachedName.toUtf8().constData(),
                 minimumSwordVersion().getText());

        /// \todo if this is the case, can we use the module at all?
    }
}

CSwordModuleInfo::CSwordModuleInfo(const CSwordModuleInfo & copy)
    : QObject(NULL)
    , m_module(copy.m_module)
    , m_backend(copy.m_backend)
    , m_type(copy.m_type)
    , m_hidden(copy.m_hidden)
    , m_cancelIndexing(copy.m_cancelIndexing)
    , m_cachedName(copy.m_cachedName)
    , m_cachedCategory(copy.m_cachedCategory)
    , m_cachedLanguage(copy.m_cachedLanguage)
    , m_cachedHasVersion(copy.m_cachedHasVersion)
{
    // Intentionally empty
}

bool CSwordModuleInfo::unlock(const QString & unlockKey) {
    if (!isEncrypted())
        return false;

    bool unlocked = unlockKeyIsValid();

    btConfig().setModuleEncryptionKey(m_cachedName, unlockKey);

    /// \todo remove this comment once it is no longer needed
    /* There is currently a deficiency in sword 1.6.1 in that
       backend->setCipherKey() does not work correctly for modules from which
       data was already fetched. Therefore we have to reload the modules in
       bibletime.cpp */
    m_backend.setCipherKey(m_module->getName(), unlockKey.toUtf8().constData());

    /// \todo write to Sword config as well

    if (unlockKeyIsValid() != unlocked)
        emit unlockedChanged(!unlocked);
    return true;
}

bool CSwordModuleInfo::isLocked() const {
    // still works, but the cipherkey is stored in BtConfig.
    // Works because it is set in sword on program startup.

    return isEncrypted() && !unlockKeyIsValid();
}

bool CSwordModuleInfo::isEncrypted() const {
    /**
    * If we have the CipherKey entry the module
    * is encrypted but not necessarily locked
    */

    /* This code is still right, though we do no longer write to the module
       config files any more. */
    typedef sword::SectionMap::const_iterator SMCI;
    SMCI it = m_backend.getConfig()->Sections.find(m_cachedName.toUtf8().constData());
    if (it == m_backend.getConfig()->Sections.end())
        return false;

    const sword::ConfigEntMap & config = it->second;
    return config.find("CipherKey") != config.end();
}

bool CSwordModuleInfo::unlockKeyIsValid() const {
    m_module->setPosition(sword::TOP);

    /* This needs to use ::fromLatin1 because if the text is still locked, a lot
       of garbage will show up. It will also work with properly decrypted
       Unicode text, because all non-ASCII Unicode chars consist of bytes >127
       and therefore contain no control (nonprintable) characters, which are all
       <127. */
    const QString test(isUnicode()
                       ? QString::fromUtf8(m_module->getRawEntryBuf().c_str())
                       : QString::fromLatin1(m_module->getRawEntryBuf().c_str()));

    if (test.isEmpty())
        return false;

    for (int i = 0; i < test.length() && i < 100; i++)
        if (!test[i].isPrint() && !test[i].isNull())
            return false;
    return true;
}

QString CSwordModuleInfo::getGlobalBaseIndexLocation() {
    return util::directory::getUserIndexDir().absolutePath();
}

QString CSwordModuleInfo::getModuleBaseIndexLocation() const {
    return getGlobalBaseIndexLocation() + QString("/") + m_cachedName.toLocal8Bit();
}

QString CSwordModuleInfo::getModuleStandardIndexLocation() const {
    // This for now returns the location of the main index
    return getModuleBaseIndexLocation() + QString("/standard");
}

bool CSwordModuleInfo::hasIndex() const {
#ifndef BT_NO_LUCENE
    { // Is this a directory?
        QFileInfo fi(getModuleStandardIndexLocation());
        if (!fi.isDir())
            return false;
    }

    // Are the index version and module version OK?
    QSettings module_config(getModuleBaseIndexLocation()
                            + QString("/bibletime-index.conf"),
                            QSettings::IniFormat);

    if (m_cachedHasVersion
        && module_config.value("module-version").toString()
           != config(CSwordModuleInfo::ModuleVersion))
    {
        return false;
    }

    if (module_config.value("index-version").toUInt() != INDEX_VERSION) {
        qDebug("%s: INDEX_VERSION is not compatible with this version of "
               "BibleTime.",
               m_cachedName.toUtf8().constData());
        return false;
    }

    // Is the index there?
    return lucene::index::IndexReader::indexExists(getModuleStandardIndexLocation()
                                                   .toLatin1().constData());
#else
    return false;
#endif
}

void CSwordModuleInfo::buildIndex() {
    m_cancelIndexing = false;

#ifndef BT_NO_LUCENE
    try {
        // Without this we don't get strongs, lemmas, etc.
        m_backend.setFilterOptions(btConfig().getFilterOptions());
        /* Make sure we reset all important filter options which influcence the
           plain filters. Turn on these options, they are needed for the
           EntryAttributes population */
        m_backend.setOption(CSwordModuleInfo::strongNumbers, true);
        m_backend.setOption(CSwordModuleInfo::morphTags, true);
        m_backend.setOption(CSwordModuleInfo::footnotes, true);
        m_backend.setOption(CSwordModuleInfo::headings, true);
        /* We don't want the following in the text, the do not carry searchable
           information. */
        m_backend.setOption(CSwordModuleInfo::morphSegmentation, false);
        m_backend.setOption(CSwordModuleInfo::scriptureReferences, false);
        m_backend.setOption(CSwordModuleInfo::redLetterWords, false);

        // Do not use any stop words:
        static const TCHAR * stop_words[1u]  = { NULL };
        lucene::analysis::standard::StandardAnalyzer an(static_cast<const TCHAR **>(stop_words));
        const QString index(getModuleStandardIndexLocation());

        QDir dir("/");
        dir.mkpath(getGlobalBaseIndexLocation());
        dir.mkpath(getModuleBaseIndexLocation());
        dir.mkpath(getModuleStandardIndexLocation());

        if (lucene::index::IndexReader::indexExists(index.toLatin1().constData()))
            if (lucene::index::IndexReader::isLocked(index.toLatin1().constData()))
                lucene::index::IndexReader::unlock(index.toLatin1().constData());

        // Always create a new index:
        typedef lucene::index::IndexWriter IW;
        QScopedPointer<IW> writer(new IW(index.toLatin1().constData(), &an, true));
        writer->setMaxFieldLength(BT_MAX_LUCENE_FIELD_LENGTH);
        writer->setUseCompoundFile(true); // Merge segments into a single file
#ifndef CLUCENE2
        writer->setMinMergeDocs(1000);
#endif

        m_module->setPosition(sword::TOP);
        unsigned long verseLowIndex = m_module->getIndex();
        m_module->setPosition(sword::BOTTOM);
        unsigned long verseHighIndex = m_module->getIndex();

        // verseLowIndex is not 0 in all cases (i.e. NT-only modules)
        unsigned long verseIndex = verseLowIndex + 1;
        unsigned long verseSpan = verseHighIndex - verseLowIndex;

        // Index() is not implemented properly for lexicons, so work around it:
        if (m_type == CSwordModuleInfo::Lexicon) {
            verseIndex = 0;
            verseLowIndex = 0;
            verseSpan = static_cast<CSwordLexiconModuleInfo *>(this)->entries().size();
        }

        emit indexingProgress(0);

        sword::SWKey * const key = m_module->getKey();
        sword::VerseKey * const vk = dynamic_cast<sword::VerseKey *>(key);

        if (vk) {
            /* We have to be sure to insert the english key into the index,
               otherwise we'd be in trouble if the language changes. */
            vk->setLocale("en_US");
            /* If we have a verse based module, we want to include the pre-
               chapter etc. headings in the search. */
            vk->setIntros(true);
        }

        QByteArray textBuffer; // Holds UTF-8 data and is faster than QString.

        // we start with the first module entry, key is automatically updated
        // because key is a pointer to the modules key
        m_module->setSkipConsecutiveLinks(true);

        QScopedArrayPointer<wchar_t> sPwcharBuffer(
                new wchar_t[BT_MAX_LUCENE_FIELD_LENGTH  + 1]);
        wchar_t * const wcharBuffer = sPwcharBuffer.data();
        Q_ASSERT(wcharBuffer);

        m_module->setPosition(sword::TOP);
        while (!(m_module->popError()) && !m_cancelIndexing) {

            /* Also index Chapter 0 and Verse 0, because they might have
               information in the entry attributes. We used to just put their
               content into the textBuffer and continue to the next verse, but
               with entry attributes this doesn't work any more. Hits in the
               search dialog will show up as 1:1 (instead of 0). */

            QScopedPointer<lucene::document::Document> doc(
                    new lucene::document::Document());

            //index the key
            lucene_utf8towcs(wcharBuffer, key->getText(), BT_MAX_LUCENE_FIELD_LENGTH);

            doc->add(*(new lucene::document::Field(static_cast<const TCHAR *>(_T("key")),
                                                   static_cast<const TCHAR *>(wcharBuffer),
                                                   lucene::document::Field::STORE_YES
                                                   | lucene::document::Field::INDEX_NO)));

            /* At this point we have to make sure we disabled the strongs and
               the other options, so the plain filters won't include the numbers
               somehow. */
            textBuffer.append(m_module->stripText());
            lucene_utf8towcs(wcharBuffer,
                             static_cast<const char *>(textBuffer),
                             BT_MAX_LUCENE_FIELD_LENGTH);
            doc->add(*(new lucene::document::Field(static_cast<const TCHAR *>(_T("content")),
                                                   static_cast<const TCHAR *>(wcharBuffer),
                                                   lucene::document::Field::STORE_NO
                                                   | lucene::document::Field::INDEX_TOKENIZED)));
            textBuffer.clear();

            typedef sword::AttributeList::iterator ALI;
            typedef sword::AttributeValue::iterator AVI;

            for (ALI it = m_module->getEntryAttributes()["Footnote"].begin();
                 it != m_module->getEntryAttributes()["Footnote"].end();
                 ++it)
            {
                lucene_utf8towcs(wcharBuffer, it->second["body"], BT_MAX_LUCENE_FIELD_LENGTH);
                doc->add(*(new lucene::document::Field(static_cast<const TCHAR *>(_T("footnote")),
                                                       static_cast<const TCHAR *>(wcharBuffer),
                                                       lucene::document::Field::STORE_NO
                                                       | lucene::document::Field::INDEX_TOKENIZED)));
            }

            // Headings
            for (AVI it = m_module->getEntryAttributes()["Heading"]["Preverse"].begin();
                 it != m_module->getEntryAttributes()["Heading"]["Preverse"].end();
                 ++it)
            {
                lucene_utf8towcs(wcharBuffer, it->second, BT_MAX_LUCENE_FIELD_LENGTH);
                doc->add(*(new lucene::document::Field(static_cast<const TCHAR *>(_T("heading")),
                                                       static_cast<const TCHAR *>(wcharBuffer),
                                                       lucene::document::Field::STORE_NO
                                                       | lucene::document::Field::INDEX_TOKENIZED)));
            }

            // Strongs/Morphs
            for (ALI it = m_module->getEntryAttributes()["Word"].begin();
                 it != m_module->getEntryAttributes()["Word"].end();
                 ++it)
            {
                if (it->second["LemmaClass"] == "strong") {
                    lucene_utf8towcs(wcharBuffer, it->second["Lemma"], BT_MAX_LUCENE_FIELD_LENGTH);
                    doc->add(*(new lucene::document::Field(static_cast<const TCHAR *>(_T("strong")),
                                                           static_cast<const TCHAR *>(wcharBuffer),
                                                           lucene::document::Field::STORE_NO
                                                           | lucene::document::Field::INDEX_TOKENIZED)));
                }
                if (it->second.find("Morph") != it->second.end()) {
                    lucene_utf8towcs(wcharBuffer, it->second["Morph"], BT_MAX_LUCENE_FIELD_LENGTH);
                    doc->add(*(new lucene::document::Field(static_cast<const TCHAR *>(_T("morph")),
                                                           static_cast<const TCHAR *>(wcharBuffer),
                                                           lucene::document::Field::STORE_NO
                                                           | lucene::document::Field::INDEX_TOKENIZED)));
                }
            }

            writer->addDocument(doc.data());
            //Index() is not implemented properly for lexicons, so we use a
            //workaround.
            if (m_type == CSwordModuleInfo::Lexicon) {
                verseIndex++;
            } else {
                verseIndex = m_module->getIndex();
            }

            if (verseIndex % 200 == 0) {
                if (verseSpan == 0) { // Prevent division by zero
                    emit indexingProgress(0);
                } else {
                    emit indexingProgress(
                            static_cast<int>(
                                    (100 * (verseIndex - verseLowIndex))
                                    / verseSpan));
                }
            }

            m_module->increment();
        } // while (!(m_module->Error()) && !m_cancelIndexing)

        if (!m_cancelIndexing)
            writer->optimize();
        writer->close();
        writer.reset();

        if (m_cancelIndexing) {
            deleteIndex();
            m_cancelIndexing = false;
        } else {
            QSettings module_config(getModuleBaseIndexLocation()
                                    + QString("/bibletime-index.conf"),
                                    QSettings::IniFormat);
            if (m_cachedHasVersion)
                module_config.setValue("module-version",
                                       config(CSwordModuleInfo::ModuleVersion));
            module_config.setValue("index-version", INDEX_VERSION);
            emit hasIndexChanged(true);
        }
    // } catch (CLuceneError & e) {
    } catch (...) {
        deleteIndex();
        m_cancelIndexing = false;
        throw;
    }
#else
    return false;
#endif
}

void CSwordModuleInfo::deleteIndex() {
    deleteIndexForModule(m_cachedName);
    emit hasIndexChanged(false);
}

void CSwordModuleInfo::deleteIndexForModule(const QString & name) {
    util::directory::removeRecursive(getGlobalBaseIndexLocation() + "/" + name);
}

size_t CSwordModuleInfo::indexSize() const {
    namespace DU = util::directory;
    return DU::getDirSizeRecursive(getModuleBaseIndexLocation());
}

size_t CSwordModuleInfo::searchIndexed(const QString & searchedText,
                                       const sword::ListKey & scope,
                                       sword::ListKey & results) const
{
    QScopedArrayPointer<char> sPutfBuffer(
            new char[BT_MAX_LUCENE_FIELD_LENGTH  + 1]);
    QScopedArrayPointer<wchar_t> sPwcharBuffer(
            new wchar_t[BT_MAX_LUCENE_FIELD_LENGTH  + 1]);
    char * const utfBuffer = sPutfBuffer.data();
    Q_ASSERT(utfBuffer);
    wchar_t * const wcharBuffer = sPwcharBuffer.data();
    Q_ASSERT(wcharBuffer);

    // work around Swords thread insafety for Bibles and Commentaries
    {
        QScopedPointer<CSwordKey> key(CSwordKey::createInstance(this));
        const sword::SWKey * const s = dynamic_cast<sword::SWKey *>(key.data());
        if (s)
            m_module->setKey(*s);
    }
    QList<sword::VerseKey *> list;

    results.clear();

#ifndef BT_NO_LUCENE
    // do not use any stop words
    static const TCHAR * stop_words[1u]  = { NULL };
    lucene::analysis::standard::StandardAnalyzer analyzer(stop_words);
    lucene::search::IndexSearcher searcher(getModuleStandardIndexLocation().toLatin1().constData());
    lucene_utf8towcs(wcharBuffer, searchedText.toUtf8().constData(), BT_MAX_LUCENE_FIELD_LENGTH);
    QScopedPointer<lucene::search::Query> q(lucene::queryParser::QueryParser::parse(static_cast<const TCHAR *>(wcharBuffer),
                                                                                    static_cast<const TCHAR *>(_T("content")),
                                                                                    &analyzer));

    QScopedPointer<lucene::search::Hits> h(searcher.search(q.data(),
                                                           #ifdef CLUCENE2
                                                           lucene::search::Sort::INDEXORDER()));
                                                           #else
                                                           lucene::search::Sort::INDEXORDER));
                                                           #endif

    const bool useScope = (scope.getCount() > 0);

    lucene::document::Document * doc = 0;
    QScopedPointer<sword::SWKey> swKey(m_module->createKey());

    sword::VerseKey * const vk = dynamic_cast<sword::VerseKey *>(swKey.data());
    if (vk)
        vk->setIntros(true);

#ifdef CLUCENE2
    for (size_t i = 0; i < h->length(); ++i) {
#else
    for (int i = 0; i < h->length(); ++i) {
#endif
        doc = &h->doc(i);
        lucene_wcstoutf8(utfBuffer,
                         static_cast<const wchar_t *>(doc->get(static_cast<const TCHAR *>(_T("key")))),
                         BT_MAX_LUCENE_FIELD_LENGTH);


        swKey->setText(utfBuffer);

        // Limit results based on scope:
        if (useScope) {
            for (int j = 0; j < scope.getCount(); j++) {
                Q_ASSERT(dynamic_cast<const sword::VerseKey *>(scope.getElement(j)));
                const sword::VerseKey * const vkey = static_cast<const sword::VerseKey *>(scope.getElement(j));
                if (vkey->getLowerBound().compare(*swKey) <= 0
                    && vkey->getUpperBound().compare(*swKey) >= 0)
                {
                    results.add(*swKey);
                }
            }
        } else {
            results.add(*swKey); // No scope, give me all buffers
        }
    }
#endif

    qDeleteAll(list);
    list.clear();

    return static_cast<size_t>(results.getCount());
}

sword::SWVersion CSwordModuleInfo::minimumSwordVersion() const {
    return sword::SWVersion(config(CSwordModuleInfo::MinimumSwordVersion)
                            .toUtf8().constData());
}

QString CSwordModuleInfo::config(const CSwordModuleInfo::ConfigEntry entry) const {
    switch (entry) {

        case AboutInformation:
            return getFormattedConfigEntry("About");

        case CipherKey: {
            if (btConfig().getModuleEncryptionKey(m_cachedName).isNull()) {
                return QString(m_module->getConfigEntry("CipherKey")); // Fallback
            } else {
                return btConfig().getModuleEncryptionKey(m_cachedName);
            }
        }

        case AbsoluteDataPath: {
            QString path(getSimpleConfigEntry("AbsoluteDataPath"));
            if (!path.endsWith('/')) /// \todo is this needed?
                path.append('/');

            return path;
        }

        case DataPath: {
            QString path(getSimpleConfigEntry("DataPath"));

            // Make sure we remove the dataFile part if it's a Lexicon
            if (m_type == CSwordModuleInfo::GenericBook
                || m_type == CSwordModuleInfo::Lexicon)
            {
                int pos = path.lastIndexOf("/"); // Last slash in the string
                if (pos != -1)
                    path = path.left(pos + 1); // Include the slash
            }
            return path;
        }

        case Description:
            return getFormattedConfigEntry("Description");

        case ModuleVersion: {
            const QString version(getSimpleConfigEntry("Version"));
            return version.isEmpty() ? "1.0" : version;
        }

        case MinimumSwordVersion: {
            const QString minimumVersion(getSimpleConfigEntry("MinimumVersion"));
            return minimumVersion.isEmpty() ? "0.0" : minimumVersion;
        }

        case TextDir: {
            const QString dir(getSimpleConfigEntry("Direction"));
            return dir.isEmpty() ? QString("LtoR") : dir;
        }

        case DisplayLevel: {
            const QString level(getSimpleConfigEntry("DisplayLevel"));
            return level.isEmpty() ? QString("1") : level;
        }

        case GlossaryFrom: {
            if (m_cachedCategory != Glossary)
                return QString::null;

            const QString lang(getSimpleConfigEntry("GlossaryFrom"));
            return lang.isEmpty() ? QString::null : lang;
        }

        case GlossaryTo: {
            if (m_cachedCategory != Glossary) {
                return QString::null;
            };

            const QString lang(getSimpleConfigEntry("GlossaryTo"));
            return lang.isEmpty() ? QString::null : lang;
        }

        case Markup: {
            const QString markup(getSimpleConfigEntry("SourceType"));
            return markup.isEmpty() ? QString("Unknown") : markup;
        }

        case DistributionLicense:
            return getFormattedConfigEntry("DistributionLicense");

        case DistributionSource:
            return getFormattedConfigEntry("DistributionSource");

        case DistributionNotes:
            return getFormattedConfigEntry("DistributionNotes");

        case TextSource:
            return getFormattedConfigEntry("TextSource");

        case CopyrightNotes:
            return getFormattedConfigEntry("CopyrightNotes");

        case CopyrightHolder:
            return getFormattedConfigEntry("CopyrightHolder");

        case CopyrightDate:
            return getFormattedConfigEntry("CopyrightDate");

        case CopyrightContactName:
            return getFormattedConfigEntry("CopyrightContactName");

        case CopyrightContactAddress:
            return getFormattedConfigEntry("CopyrightContactAddress");

        case CopyrightContactEmail:
            return getFormattedConfigEntry("CopyrightContactEmail");

        default:
            return QString::null;
    }
}

bool CSwordModuleInfo::has(const CSwordModuleInfo::Feature feature) const {
    switch (feature) {
        case GreekDef: return m_module->getConfig().has("Feature", "GreekDef");
        case HebrewDef: return m_module->getConfig().has("Feature", "HebrewDef");
        case GreekParse: return m_module->getConfig().has("Feature", "GreekParse");
        case HebrewParse: return m_module->getConfig().has("Feature", "HebrewParse");
    }
    return false;
}

bool CSwordModuleInfo::has(const CSwordModuleInfo::FilterTypes option) const {
    /// \todo This is a BAD workaround to see if the filter is GBF, OSIS or ThML!
    const QString name = m_backend.configOptionName(option);
    return m_module->getConfig().has("GlobalOptionFilter",
                                     QString("OSIS").append(name).toUtf8().constData())
        || m_module->getConfig().has("GlobalOptionFilter",
                                     QString("GBF").append(name).toUtf8().constData())
        || m_module->getConfig().has("GlobalOptionFilter",
                                     QString("ThML").append(name).toUtf8().constData())
        || m_module->getConfig().has("GlobalOptionFilter",
                                     QString("UTF8").append(name).toUtf8().constData())
        || m_module->getConfig().has("GlobalOptionFilter",
                                     name.toUtf8().constData());
}

CSwordModuleInfo::TextDirection CSwordModuleInfo::textDirection() const {
    return (config(TextDir) == "RtoL")
           ? CSwordModuleInfo::RightToLeft
           : CSwordModuleInfo::LeftToRight;
}

void CSwordModuleInfo::write(CSwordKey * key, const QString & newText) {
    m_module->setKey(key->key().toUtf8().constData());

    /* Don't store a pointer to the const char* value somewhere because QCString
      doesn't keep the value of it. */
    m_module->setEntry(isUnicode()
                       ? newText.toUtf8().constData()
                       : newText.toLocal8Bit().constData());
}

void CSwordModuleInfo::deleteEntry(CSwordKey * const key) {
    Q_ASSERT(key);
    m_module->setKey(isUnicode()
                     ? key->key().toUtf8().constData()
                     : key->key().toLocal8Bit().constData());
    m_module->deleteEntry();
}

Rendering::CEntryDisplay * CSwordModuleInfo::getDisplay() const {
    return dynamic_cast<Rendering::CEntryDisplay *>(m_module->getDisplay());
}

QString CSwordModuleInfo::aboutText() const {
    using util::htmlEscape;

    static const QString row("<tr><td><b>%1</b></td><td>%2</td></tr>");

    QString text;
    text += "<table>";

    text += row
            .arg(tr("Version"))
            .arg(m_cachedHasVersion
                 ? htmlEscape(config(CSwordModuleInfo::ModuleVersion))
                 : tr("unknown"));

    {
        const QString sourceType(m_module->getConfigEntry("SourceType"));
        text += row
                .arg(tr("Markup"))
                .arg(!sourceType.isEmpty()
                     ? htmlEscape(sourceType)
                     : tr("unknown"));
    }

    text += row
            .arg(tr("Location"))
            .arg(htmlEscape(config(CSwordModuleInfo::AbsoluteDataPath)));

    text += row
            .arg(tr("Language"))
            .arg(htmlEscape(m_cachedLanguage->translatedName()));

    if (m_module->getConfigEntry("Category"))
        text += row
                .arg(tr("Category"))
                .arg(htmlEscape(m_module->getConfigEntry("Category")));

    if (m_module->getConfigEntry("LCSH"))
        text += row
                .arg(tr("LCSH"))
                .arg(htmlEscape(m_module->getConfigEntry("LCSH")));

    text += row
            .arg(tr("Writable"))
            .arg(isWritable() ? tr("yes") : tr("no"));

    if (isEncrypted())
        text += row
                .arg(tr("Unlock key"))
                .arg(htmlEscape(config(CSwordModuleInfo::CipherKey)));

    QString options;

    unsigned int opts;

    for (opts = CSwordModuleInfo::filterTypesMIN;
         opts <= CSwordModuleInfo::filterTypesMAX;
         ++opts)
    {
        if (has(static_cast < CSwordModuleInfo::FilterTypes > (opts))) {
            if (!options.isEmpty())
                options += QString::fromLatin1(", ");
            typedef CSwordModuleInfo::FilterTypes FT;
            options += CSwordBackend::translatedOptionName(static_cast<FT>(opts));
        }
    }

    if (!options.isEmpty())
        text += row
                .arg(tr("Features"))
                .arg(htmlEscape(options));

    text += "</table><hr>";

    // Clearly say the module contains cult/questionable materials
    if (m_cachedCategory == Cult)
        text += QString("<br/><b>%1</b><br/><br/>")
                .arg(tr("Take care, this work contains cult / questionable "
                        "material!"));

    text += QString("<b>%1:</b><br/>%2</font>")
            .arg(tr("About"))
            .arg(config(AboutInformation)); // May contain HTML, don't escape

    QMap<CSwordModuleInfo::ConfigEntry, QString> entryMap;
    entryMap[DistributionLicense] = tr("Distribution license");
    entryMap[DistributionSource] = tr("Distribution source");
    entryMap[DistributionNotes] = tr("Distribution notes");
    entryMap[TextSource] = tr("Text source");
    entryMap[CopyrightNotes] = tr("Copyright notes");
    entryMap[CopyrightHolder] = tr("Copyright holder");
    entryMap[CopyrightDate] = tr("Copyright date");
    entryMap[CopyrightContactName] = tr("Copyright contact name");
    entryMap[CopyrightContactAddress] = tr("Copyright contact address");
    entryMap[CopyrightContactEmail] = tr("Copyright contact email");

    text += ("<hr><table>");

    static CSwordModuleInfo::ConfigEntry const entries[] = {
        DistributionLicense,
        DistributionSource,
        DistributionNotes,
        TextSource,
        CopyrightNotes,
        CopyrightHolder,
        CopyrightDate,
        CopyrightContactName,
        CopyrightContactAddress,
        CopyrightContactEmail
    };
    for (CSwordModuleInfo::ConfigEntry const * it = &entries[0u];
         it != &entries[sizeof(entries) / sizeof(entries[0u])];
         ++it)
        if (!config(*it).isEmpty())
            text += row
                    .arg(htmlEscape(entryMap[*it]))
                    .arg(htmlEscape(config(*it)));

    text += "</table></font>";

    return text;
}

QIcon const & CSwordModuleInfo::moduleIcon(const CSwordModuleInfo & module) {
    CSwordModuleInfo::Category const cat(module.m_cachedCategory);
    switch (cat) {
        case CSwordModuleInfo::Bibles:
            return module.isLocked()
                   ? CResMgr::modules::bible::icon_locked()
                   : CResMgr::modules::bible::icon_unlocked();

        case CSwordModuleInfo::Commentaries:
            return module.isLocked()
                   ? CResMgr::modules::commentary::icon_locked()
                   : CResMgr::modules::commentary::icon_unlocked();

        case CSwordModuleInfo::Lexicons:
            return module.isLocked()
                   ? CResMgr::modules::lexicon::icon_locked()
                   : CResMgr::modules::lexicon::icon_unlocked();

        case CSwordModuleInfo::Books:
            return module.isLocked()
                   ? CResMgr::modules::book::icon_locked()
                   : CResMgr::modules::book::icon_unlocked();

        case CSwordModuleInfo::Cult:
        case CSwordModuleInfo::Images:
        case CSwordModuleInfo::DailyDevotional:
        case CSwordModuleInfo::Glossary:
        case CSwordModuleInfo::UnknownCategory:
        default:
            return categoryIcon(cat);
    }
}

QIcon const & CSwordModuleInfo::categoryIcon(CSwordModuleInfo::Category category) {
    switch (category) {
        case CSwordModuleInfo::Bibles:
            return CResMgr::categories::bibles::icon();
        case CSwordModuleInfo::Commentaries:
            return CResMgr::categories::commentaries::icon();
        case CSwordModuleInfo::Books:
            return CResMgr::categories::books::icon();
        case CSwordModuleInfo::Cult:
            return CResMgr::categories::cults::icon();
        case CSwordModuleInfo::Images:
            return CResMgr::categories::images::icon();
        case CSwordModuleInfo::DailyDevotional:
            return CResMgr::categories::dailydevotional::icon();
        case CSwordModuleInfo::Lexicons:
            return CResMgr::categories::lexicons::icon();
        case CSwordModuleInfo::Glossary:
            return CResMgr::categories::glossary::icon();
        case CSwordModuleInfo::UnknownCategory:
        default:
            return BtIcons::instance().icon_null;
    }
}

QString CSwordModuleInfo::categoryName(const CSwordModuleInfo::Category & category) {
    switch (category) {
        case CSwordModuleInfo::Bibles:
            return tr("Bibles");
        case CSwordModuleInfo::Commentaries:
            return tr("Commentaries");
        case CSwordModuleInfo::Books:
            return tr("Books");
        case CSwordModuleInfo::Cult:
            return tr("Cults/Unorthodox");
        case CSwordModuleInfo::Images:
            return tr("Maps and Images");
        case CSwordModuleInfo::DailyDevotional:
            return tr("Daily Devotionals");
        case CSwordModuleInfo::Lexicons:
            return  tr("Lexicons and Dictionaries");
        case CSwordModuleInfo::Glossary:
            return tr("Glossaries");
        default:
            return tr("Unknown");
    }
}

QString CSwordModuleInfo::englishCategoryName(const CSwordModuleInfo::Category & category) {
    switch (category) {
        case CSwordModuleInfo::Bibles:
            return "Bibles";
        case CSwordModuleInfo::Commentaries:
            return "Commentaries";
        case CSwordModuleInfo::Books:
            return "Books";
        case CSwordModuleInfo::Cult:
            return "Cults/Unorthodox";
        case CSwordModuleInfo::Images:
            return "Maps and Images";
        case CSwordModuleInfo::DailyDevotional:
            return "Daily Devotionals";
        case CSwordModuleInfo::Lexicons:
            return  "Lexicons and Dictionaries";
        case CSwordModuleInfo::Glossary:
            return "Glossaries";
        default:
            return "Unknown";
    }
}

QString CSwordModuleInfo::getSimpleConfigEntry(const QString & name) const {
    QString ret = isUnicode()
                  ? QString::fromUtf8(m_module->getConfigEntry(name.toUtf8().constData()))
                  : QString::fromLatin1(m_module->getConfigEntry(name.toUtf8().constData()));

    return ret.isEmpty() ? QString::null : ret;
}

/// \note See http://www.crosswire.org/wiki/DevTools:conf_Files#Localization
QString CSwordModuleInfo::getFormattedConfigEntry(const QString & name) const {
    const QStringList localeNames(QLocale(CSwordBackend::instance()->booknameLanguage()).uiLanguages());
    for (int i = localeNames.size() - 1; i >= -1; --i) {
        sword::SWBuf RTF_Buffer =
                m_module->getConfigEntry(
                    QString(i >= 0 ? name + "_" + localeNames[i] : name)
                        .toUtf8().constData());
        if (RTF_Buffer.length() > 0) {
            sword::RTFHTML RTF_Filter;
            RTF_Filter.processText(RTF_Buffer, 0, 0);
            return isUnicode()
                   ? QString::fromUtf8(RTF_Buffer.c_str())
                   : QString::fromLatin1(RTF_Buffer.c_str());
        }
    }
    return QString::null;
}

bool CSwordModuleInfo::setHidden(bool hide) {
    if (m_hidden == hide)
        return false;

    m_hidden = hide;
    QStringList hiddenModules(btConfig().value<QStringList>("state/hiddenModules"));
    Q_ASSERT(hiddenModules.contains(m_cachedName) != hide);
    if (hide) {
        hiddenModules.append(m_cachedName);
    } else {
        hiddenModules.removeOne(m_cachedName);
    }
    btConfig().setValue("state/hiddenModules", hiddenModules);
    emit hiddenChanged(hide);
    return true;
}
