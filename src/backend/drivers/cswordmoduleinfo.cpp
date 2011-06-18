/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/drivers/cswordmoduleinfo.h"

#include <QSharedPointer>
#include <CLucene.h>
#include <CLucene/util/Misc.h>
#include <CLucene/util/Reader.h>
#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QList>
#include <QRegExp>
#include <QSettings>
#include "backend/config/cbtconfig.h"
#include "backend/drivers/cswordlexiconmoduleinfo.h"
#include "backend/keys/cswordkey.h"
#include "backend/managers/clanguagemgr.h"
#include "backend/managers/cswordbackend.h"
#include "backend/rendering/centrydisplay.h"
#include "backend/cswordmodulesearch.h"
#include "btglobal.h"
#include "util/cresmgr.h"
#include "util/directory.h"
#include "util/exceptions.h"
#include "util/dialogutil.h"

// Sword includes:
#include <listkey.h>
#include <swbuf.h>
#include <swconfig.h>
#include <swkey.h>
#include <rtfhtml.h>
#include <versekey.h>


#if 0
namespace {

/** HELPER Method to dump all current EntryAttributes of a module. */
void dumpEntryAttributes(sword::SWModule *m) {
    qDebug() << "Attributes for key: " << m->getKeyText();
    sword::AttributeTypeList::iterator i1;
    sword::AttributeList::iterator i2;
    sword::AttributeValue::iterator i3;
    for (i1 = m->getEntryAttributes().begin(); i1 != m->getEntryAttributes().end(); i1++) {
        qDebug() << "[ " << i1->first << " ]";
        for (i2 = i1->second.begin(); i2 != i1->second.end(); i2++) {
            qDebug() << "\t[ " << i2->first << " ]";
            for (i3 = i2->second.begin(); i3 != i2->second.end(); i3++) {
                qDebug() << "\t\t" << i3->first << " = " << i3->second;
            }
        }
    }
}

} // anonymous namespace
#endif

//Increment this, if the index format changes
//Then indices on the user's systems will be rebuilt
const unsigned int INDEX_VERSION = 7;

//Maximum index entry size, 1MiB for now
//Lucene default is too small
const unsigned long BT_MAX_LUCENE_FIELD_LENGTH = 1024 * 1024;

CSwordModuleInfo::CSwordModuleInfo(sword::SWModule *module,
                                   CSwordBackend * const usedBackend,
                                   ModuleType type)
    : m_module(module),
      m_backend(usedBackend ? usedBackend : CSwordBackend::instance()),
      m_type(type),
      m_cancelIndexing(false),
      m_cachedName(QString::fromUtf8(module->Name())),
      m_cachedHasVersion(!QString((*m_backend->getConfig())[module->Name()]["Version"]).isEmpty())
{
    Q_ASSERT(module != 0);
    Q_ASSERT(usedBackend != 0);

    initCachedCategory();
    initCachedLanguage();

    m_hidden = CBTConfig::get(CBTConfig::hiddenModules).contains(name());

    if (backend()) {
        if (hasVersion() && (minimumSwordVersion() > sword::SWVersion::currentVersion)) {
            qWarning("The module \"%s\" requires a newer Sword library. Please update to \"Sword %s\".",
                     name().toUtf8().constData(), (const char *)minimumSwordVersion());

            /// \todo if this is the case, can we use the module at all?
        }
    }
}

CSwordModuleInfo::CSwordModuleInfo(const CSwordModuleInfo &o)
    : QObject(0), m_module(o.m_module), m_backend(o.m_backend),
      m_type(o.m_type), m_hidden(o.m_hidden),
      m_cancelIndexing(o.m_cancelIndexing), m_cachedName(o.m_cachedName),
      m_cachedCategory(o.m_cachedCategory),
      m_cachedLanguage(o.m_cachedLanguage),
      m_cachedHasVersion(o.m_cachedHasVersion)
{
    // Intentionally empty
}

bool CSwordModuleInfo::unlock(const QString & unlockKey) {
    if (!isEncrypted()) {
        return false;
    }

    bool unlocked = unlockKeyIsValid();

    CBTConfig::setModuleEncryptionKey(name(), unlockKey);

    /// \todo remove this comment once it is no longer needed
    /* There is currently a deficiency in sword 1.6.1 in that backend->setCipherKey() does
     * not work correctly for modules from which data was already fetched. Therefore we have to
     * reload the modules in bibletime.cpp
     */
    backend()->setCipherKey(m_module->Name(), unlockKey.toUtf8().constData());

    /// \todo write to Sword config as well

    if (unlockKeyIsValid() != unlocked) {
        emit unlockedChanged(!unlocked);
    }
    return true;
}

bool CSwordModuleInfo::isLocked() const {
    //still works, but the cipherkey is stored in CBTConfig.
    //Works because it is set in sword on program startup.

    return isEncrypted() && !unlockKeyIsValid();
}

bool CSwordModuleInfo::isEncrypted() const {
    /**
    * If we have the CipherKey entry the module
    * is encrypted but not necessarily locked
    */

    //This code is still right, though we do no longer write to the module config files any more
    std::map < sword::SWBuf, sword::ConfigEntMap, std::less < sword::SWBuf > >::iterator SectionMapIter;
    SectionMapIter = backend()->getConfig()->Sections.find(name().toUtf8().constData());
    if (SectionMapIter == backend()->getConfig()->Sections.end())
        return false;
    sword::ConfigEntMap config = SectionMapIter->second;
    sword::ConfigEntMap::iterator it = config.find("CipherKey");

    return it != config.end();
}

bool CSwordModuleInfo::unlockKeyIsValid() const {
    m_module->setPosition(sword::TOP);

    // This needs to use ::fromLatin1 because if the text is still locked,
    // a lot of garbage will show up. It will also work with properly decrypted
    // Unicode text, because all non-ASCII Unicode chars consist of bytes >127
    // and therefore contain no control (nonprintable) characters, which are all <127.
    QString test = isUnicode()
                   ? QString::fromUtf8(m_module->getRawEntryBuf().c_str())
                   : QString::fromLatin1( m_module->getRawEntryBuf().c_str() );

    if (test.isEmpty()) {
        return false;
    }

    for (int i = 0; i <= test.length() && i < 100; i++) {
        if ( !test[i].isPrint() && !test[i].isNull() ) {
            return false;
        }
    }

    return true;
}

QString CSwordModuleInfo::getGlobalBaseIndexLocation() {
    return util::directory::getUserIndexDir().absolutePath();
}

QString CSwordModuleInfo::getModuleBaseIndexLocation() const {
    return getGlobalBaseIndexLocation() + QString("/") + name().toLocal8Bit();
}

QString CSwordModuleInfo::getModuleStandardIndexLocation() const { //this for now returns the location of the main index
    return getModuleBaseIndexLocation() + QString("/standard");
}

bool CSwordModuleInfo::hasIndex() const {
    //this will return true only
    //if the index exists and has correct version information for both index and module
    QDir d;
    if (!d.exists( getModuleStandardIndexLocation() )) {
        return false;
    }

    //first check if the index version and module version are ok
    QSettings module_config(getModuleBaseIndexLocation() + QString("/bibletime-index.conf"), QSettings::IniFormat);

    if (hasVersion() &&
        module_config.value("module-version").toString() != config(CSwordModuleInfo::ModuleVersion))
    {
        return false;
    }

    if (module_config.value("index-version").toUInt() != INDEX_VERSION) {
        qDebug("%s: INDEX_VERSION is not compatible with this version of BibleTime.", name().toUtf8().constData());
        return false;
    }

    //then check if the index is there
    return lucene::index::IndexReader::indexExists(getModuleStandardIndexLocation().toAscii().constData());
}

bool CSwordModuleInfo::buildIndex() {

    m_cancelIndexing = false;

    try {
        //Without this we don't get strongs, lemmas, etc
        backend()->setFilterOptions ( CBTConfig::getFilterOptionDefaults() );
        //make sure we reset all important filter options which influcence the plain filters.
        // turn on these options, they are needed for the EntryAttributes population
        backend()->setOption( CSwordModuleInfo::strongNumbers,  true );
        backend()->setOption( CSwordModuleInfo::morphTags,  true );
        backend()->setOption( CSwordModuleInfo::footnotes,  true );
        backend()->setOption( CSwordModuleInfo::headings,  true );
        // we don't want the following in the text, the do not carry searchable information
        backend()->setOption( CSwordModuleInfo::morphSegmentation,  false );
        backend()->setOption( CSwordModuleInfo::scriptureReferences,  false );
        backend()->setOption( CSwordModuleInfo::redLetterWords,  false );

        // do not use any stop words
        const TCHAR* stop_words[]  = { NULL };
        lucene::analysis::standard::StandardAnalyzer an( (const TCHAR**)stop_words );
        QString index = getModuleStandardIndexLocation();

        QDir dir("/");
        dir.mkpath( getGlobalBaseIndexLocation() );
        dir.mkpath( getModuleBaseIndexLocation() );
        dir.mkpath( getModuleStandardIndexLocation() );

        if (lucene::index::IndexReader::indexExists(index.toAscii().constData())) {
            if (lucene::index::IndexReader::isLocked(index.toAscii().constData()) ) {
                lucene::index::IndexReader::unlock(index.toAscii().constData());
            }
        }

        QSharedPointer<lucene::index::IndexWriter> writer( new lucene::index::IndexWriter(index.toAscii().constData(), &an, true) ); //always create a new index
        writer->setMaxFieldLength(BT_MAX_LUCENE_FIELD_LENGTH);
        writer->setUseCompoundFile(true); //merge segments into a single file
        writer->setMinMergeDocs(1000);

        m_module->setPosition(sword::TOP);
        unsigned long verseLowIndex = m_module->Index();
        m_module->setPosition(sword::BOTTOM);
        unsigned long verseHighIndex = m_module->Index();

        //verseLowIndex is not 0 in all cases (i.e. NT-only modules)
        unsigned long verseIndex = verseLowIndex + 1;
        unsigned long verseSpan = verseHighIndex - verseLowIndex;

        //Index() is not implemented properly for lexicons, so we use a
        //workaround.
        if (type() == CSwordModuleInfo::Lexicon) {
            verseIndex = 0;
            verseLowIndex = 0;
            verseSpan = ((CSwordLexiconModuleInfo*)this)->entries().size();
        }

        emit indexingProgress(0);

        sword::SWKey* key = m_module->getKey();
        //VerseKey for bibles
        sword::VerseKey* vk = dynamic_cast<sword::VerseKey*>(key);

        if (vk) {
            // we have to be sure to insert the english key into the index, otherwise we'd be in trouble if the language changes
            vk->setLocale("en_US");
            //If we have a verse based module, we want to include the pre-chapter etc. headings in the search
            vk->Headings(1);
        }

        //holds UTF-8 data and is faster than QString.
        QByteArray textBuffer;

        // we start with the first module entry, key is automatically updated
        // because key is a pointer to the modules key
        m_module->setSkipConsecutiveLinks(true);

        wchar_t wcharBuffer[BT_MAX_LUCENE_FIELD_LENGTH + 1];

        m_module->setPosition(sword::TOP);
        while (!(m_module->Error()) && !m_cancelIndexing) {

            // Also index Chapter 0 and Verse 0, because they might have information in the entry attributes
            // We used to just put their content into the textBuffer and continue to the next verse, but
            // with entry attributes this doesn't work any more.
            // Hits in the search dialog will show up as 1:1 (instead of 0)

            QSharedPointer<lucene::document::Document> doc(new lucene::document::Document());

            //index the key
            lucene_utf8towcs(wcharBuffer, key->getText(), BT_MAX_LUCENE_FIELD_LENGTH);

            //doc->add(*lucene::document::Field::UnIndexed((const TCHAR*)_T("key"), (const TCHAR*)wcharBuffer));
            doc->add(*(new lucene::document::Field((const TCHAR*)_T("key"), (const TCHAR*)wcharBuffer, lucene::document::Field::STORE_YES | lucene::document::Field::INDEX_NO)));

            // index the main text
            //at this point we have to make sure we disabled the strongs and the other options
            //so the plain filters won't include the numbers somehow.
            lucene_utf8towcs(wcharBuffer, (const char*) textBuffer.append(m_module->StripText()), BT_MAX_LUCENE_FIELD_LENGTH);
            doc->add(*(new lucene::document::Field((const TCHAR*)_T("content"), (const TCHAR*)wcharBuffer, lucene::document::Field::STORE_NO | lucene::document::Field::INDEX_TOKENIZED)));
            textBuffer.resize(0); //clean up

            // index attributes
            sword::AttributeList::iterator attListI;
            sword::AttributeValue::iterator attValueI;
            // Footnotes
            for (attListI = m_module->getEntryAttributes()["Footnote"].begin();
                    attListI != m_module->getEntryAttributes()["Footnote"].end();
                    attListI++) {
                lucene_utf8towcs(wcharBuffer, attListI->second["body"], BT_MAX_LUCENE_FIELD_LENGTH);
                //doc->add(*lucene::document::Field::UnStored((const TCHAR*)_T("footnote"), wcharBuffer));
                doc->add(*(new lucene::document::Field((const TCHAR*)_T("footnote"), (const TCHAR*)wcharBuffer, lucene::document::Field::STORE_NO | lucene::document::Field::INDEX_TOKENIZED)));
            } // for attListI

            // Headings
            for (attValueI = m_module->getEntryAttributes()["Heading"]["Preverse"].begin();
                    attValueI != m_module->getEntryAttributes()["Heading"]["Preverse"].end();
                    attValueI++) {
                lucene_utf8towcs(wcharBuffer, attValueI->second, BT_MAX_LUCENE_FIELD_LENGTH);
                //doc->add(*lucene::document::Field::UnStored((const TCHAR*)_T("heading"), wcharBuffer));
                doc->add(*(new lucene::document::Field((const TCHAR*)_T("heading"), (const TCHAR*)wcharBuffer, lucene::document::Field::STORE_NO | lucene::document::Field::INDEX_TOKENIZED)));
            } // for attValueI

            // Strongs/Morphs
            for (attListI = m_module->getEntryAttributes()["Word"].begin();
                    attListI != m_module->getEntryAttributes()["Word"].end();
                    attListI++) {
                // for each attribute
                if (attListI->second["LemmaClass"] == "strong") {
                    lucene_utf8towcs(wcharBuffer, attListI->second["Lemma"], BT_MAX_LUCENE_FIELD_LENGTH);
                    //doc->add(*lucene::document::Field::UnStored((const TCHAR*)_T("strong"), wcharBuffer));
                    doc->add(*(new lucene::document::Field((const TCHAR*)_T("strong"), (const TCHAR*)wcharBuffer, lucene::document::Field::STORE_NO | lucene::document::Field::INDEX_TOKENIZED)));
                    //qWarning("Adding strong %s", attListI->second["Lemma"].c_str());
                }
                if (attListI->second.find("Morph") != attListI->second.end()) {
                    lucene_utf8towcs(wcharBuffer, attListI->second["Morph"], BT_MAX_LUCENE_FIELD_LENGTH);
                    //doc->add(*lucene::document::Field::UnStored((const TCHAR*)_T("morph"), wcharBuffer));
                    doc->add(*(new lucene::document::Field((const TCHAR*)_T("morph"), (const TCHAR*)wcharBuffer, lucene::document::Field::STORE_NO | lucene::document::Field::INDEX_TOKENIZED)));
                }
            } // for attListI

            writer->addDocument(doc.data());
            //Index() is not implemented properly for lexicons, so we use a
            //workaround.
            if (type() == CSwordModuleInfo::Lexicon) {
                verseIndex++;
            }
            else {
                verseIndex = m_module->Index();
            }

            if (verseIndex % 200 == 0) {
                int indexingProgressValue;
                if (verseSpan == 0) { //prevent division by zero
                    //m_indexingProgress.setValue( QVariant(0) );
                    indexingProgressValue = 0;
                }
                else {
                    //m_indexingProgress.setValue( QVariant((int)((100*(verseIndex-verseLowIndex))/(verseHighIndex-verseLowIndex))) );
                    indexingProgressValue = (int)((100 * (verseIndex - verseLowIndex)) / (verseSpan));
                }
                //m_indexingProgress.activate();
                emit indexingProgress(indexingProgressValue);
            }

            m_module->increment();
        } // while (!(m_module->Error()) && !m_cancelIndexing)

        if (!m_cancelIndexing) {
            writer->optimize();
        }
        writer->close();

        if (m_cancelIndexing) {
            deleteIndex();
            m_cancelIndexing = false;
        }
        else {
            QSettings module_config(getModuleBaseIndexLocation() + QString("/bibletime-index.conf"), QSettings::IniFormat);
            if (hasVersion()) module_config.setValue("module-version", config(CSwordModuleInfo::ModuleVersion) );
            module_config.setValue("index-version", INDEX_VERSION);
            emit hasIndexChanged(true);
        }
    }
    catch (CLuceneError &e) {
        qWarning() << "CLucene exception occurred while indexing:" << e.what();
        util::showWarning(0, QCoreApplication::tr("Indexing aborted"), QCoreApplication::tr("An internal error occurred while building the index: %1").arg(e.what()));
        deleteIndex();
        m_cancelIndexing = false;
        return false;
    }
    catch (...) {
        qWarning("CLucene exception occurred while indexing");
        util::showWarning(0, QCoreApplication::tr("Indexing aborted"), QCoreApplication::tr("An internal error occurred while building the index."));
        deleteIndex();
        m_cancelIndexing = false;
        return false;
    }

    return true;
}

void CSwordModuleInfo::deleteIndex() {
    deleteIndexForModule(name());
    emit hasIndexChanged(false);
}

void CSwordModuleInfo::deleteIndexForModule(const QString &name) {
    util::directory::removeRecursive( getGlobalBaseIndexLocation() + "/" + name );
}

unsigned long CSwordModuleInfo::indexSize() const {
    namespace DU = util::directory;
    return DU::getDirSizeRecursive( getModuleBaseIndexLocation() );
}


int CSwordModuleInfo::searchIndexed(const QString &searchedText,
                                    const sword::ListKey &scope,
                                    sword::ListKey &results) const
{
    char utfBuffer[BT_MAX_LUCENE_FIELD_LENGTH  + 1];
    wchar_t wcharBuffer[BT_MAX_LUCENE_FIELD_LENGTH + 1];

    // work around Swords thread insafety for Bibles and Commentaries
    QSharedPointer < CSwordKey > key(CSwordKey::createInstance(this));
    sword::SWKey* s = dynamic_cast < sword::SWKey * >(key.data());
    QList<sword::VerseKey*> list;

    if (s) {
        m_module->setKey(*s);
    }

    results.clear();

    try {
        // do not use any stop words
        const TCHAR* stop_words[]  = { NULL };
        lucene::analysis::standard::StandardAnalyzer analyzer( stop_words );
        lucene::search::IndexSearcher searcher(getModuleStandardIndexLocation().toAscii().constData());
        lucene_utf8towcs(wcharBuffer, searchedText.toUtf8().constData(), BT_MAX_LUCENE_FIELD_LENGTH);
        QSharedPointer<lucene::search::Query> q( lucene::queryParser::QueryParser::parse((const TCHAR*)wcharBuffer, (const TCHAR*)_T("content"), &analyzer) );

        QSharedPointer<lucene::search::Hits> h( searcher.search(q.data(), lucene::search::Sort::INDEXORDER) );

        /// \warning This is a workaround for Sword constness
        const bool useScope = (const_cast<sword::ListKey&>(scope).Count() > 0);
//        const bool isVerseModule = (type() == CSwordModuleInfo::Bible) || (type() == CSwordModuleInfo::Commentary);

        lucene::document::Document* doc = 0;
        QSharedPointer<sword::SWKey> swKey( module()->CreateKey() );


        for (int i = 0; i < h->length(); ++i) {
            doc = &h->doc(i);
            lucene_wcstoutf8(utfBuffer, (const wchar_t*)doc->get((const TCHAR*)_T("key")), BT_MAX_LUCENE_FIELD_LENGTH);

            swKey->setText(utfBuffer);

            // limit results based on scope
            //if (searchOptions & CSwordModuleSearch::useScope && scope.Count() > 0){
            if (useScope) {
                /// \warning This is a workaround for sword constness
                for (int j = 0; j < const_cast<sword::ListKey&>(scope).Count(); j++) {
                    /// \warning This is a workaround for sword constness
                    sword::ListKey &scope2 = const_cast<sword::ListKey&>(scope);
                    sword::VerseKey* vkey = dynamic_cast<sword::VerseKey*>(scope2.getElement(j));
                    if (vkey->LowerBound().compare(*swKey) <= 0 && vkey->UpperBound().compare(*swKey) >= 0) {
                        results.add(*swKey);
                    }
                }
            }
            else { // no scope, give me all buffers
                results.add(*swKey);
            }
        }
    }
    catch (...) {
        qWarning("CLucene exception occurred");
        util::showWarning(0, QCoreApplication::tr("Search aborted"), QCoreApplication::tr("An internal error occurred while executing your search."));
        return 0;
    }

    qDeleteAll(list);
    list.clear();

    return results.Count();
}

sword::SWVersion CSwordModuleInfo::minimumSwordVersion() const {
    return sword::SWVersion(config(CSwordModuleInfo::MinimumSwordVersion).toUtf8().constData());
}

QString CSwordModuleInfo::config(const CSwordModuleInfo::ConfigEntry entry) const {
    switch (entry) {

        case AboutInformation:
            return getFormattedConfigEntry("About");

        case CipherKey: {
            if (CBTConfig::getModuleEncryptionKey(name()).isNull()) { //fall back!
                return QString(m_module->getConfigEntry("CipherKey"));
            }
            else {
                return CBTConfig::getModuleEncryptionKey(name());
            }
        }

        case AbsoluteDataPath: {
            QString path( getSimpleConfigEntry("AbsoluteDataPath") );
            path.replace(QRegExp("/./"), "/"); // make /abs/path/./modules/ looking better
            //make sure we have a trailing slash!

            if (path.right(1) != "/") {
                path.append('/');
            }

            return path;
        }

        case DataPath: { //make sure we remove the dataFile part if it's a Lexicon
            QString path(getSimpleConfigEntry("DataPath"));

            if ((type() == CSwordModuleInfo::GenericBook) || (type() == CSwordModuleInfo::Lexicon)) {
                int pos = path.lastIndexOf("/"); //last slash in the string

                if (pos != -1) {
                    path = path.left(pos + 1); //include the slash
                }
            }

            return path;
        }

        case Description:
            return getFormattedConfigEntry("Description");

        case ModuleVersion: {
            QString version(getSimpleConfigEntry("Version"));

            if (version.isEmpty()) {
                version = "1.0";
            }

            return version;
        }

        case MinimumSwordVersion: {
            const QString minimumVersion(getSimpleConfigEntry("MinimumVersion"));
            return !minimumVersion.isEmpty() ? minimumVersion : QString("0.0");
        }

        case TextDir: {
            const QString dir(getSimpleConfigEntry("Direction"));
            return !dir.isEmpty() ? dir : QString("LtoR");
        }

        case DisplayLevel: {
            const QString level(getSimpleConfigEntry("DisplayLevel"));
            return !level.isEmpty() ? level : QString("1");
        }

        case GlossaryFrom: {
            if (category() != Glossary) {
                return QString::null;
            };

            const QString lang(getSimpleConfigEntry("GlossaryFrom"));

            return !lang.isEmpty() ? lang : QString::null;
        }

        case GlossaryTo: {
            if (category() != Glossary) {
                return QString::null;
            };

            const QString lang(getSimpleConfigEntry("GlossaryTo"));

            return !lang.isEmpty() ? lang : QString::null;
        }

        case Markup: {
            const QString markup(getSimpleConfigEntry("SourceType"));
            return !markup.isEmpty() ? markup : QString("Unknown");
        }

        case DistributionLicense:
            return getSimpleConfigEntry("DistributionLicense");

        case DistributionSource:
            return getSimpleConfigEntry("DistributionSource");

        case DistributionNotes:
            return getSimpleConfigEntry("DistributionNotes");

        case TextSource:
            return getSimpleConfigEntry("TextSource");

        case CopyrightNotes:
            return getSimpleConfigEntry("CopyrightNotes");

        case CopyrightHolder:
            return getSimpleConfigEntry("CopyrightHolder");

        case CopyrightDate:
            return getSimpleConfigEntry("CopyrightDate");

        case CopyrightContactName:
            return getSimpleConfigEntry("CopyrightContactName");

        case CopyrightContactAddress:
            return getSimpleConfigEntry("CopyrightContactAddress");

        case CopyrightContactEmail:
            return getSimpleConfigEntry("CopyrightContactEmail");

        default:
            return QString::null;
    }
}

bool CSwordModuleInfo::has(const CSwordModuleInfo::Feature feature) const {
    switch (feature) {

            //         case StrongsNumbers:
            //         return m_module->getConfig().has("Feature", "StrongsNumber");

        case GreekDef:
            return m_module->getConfig().has("Feature", "GreekDef");

        case HebrewDef:
            return m_module->getConfig().has("Feature", "HebrewDef");

        case GreekParse:
            return m_module->getConfig().has("Feature", "GreekParse");

        case HebrewParse:
            return m_module->getConfig().has("Feature", "HebrewParse");
    }

    return false;
}

bool CSwordModuleInfo::has(const CSwordModuleInfo::FilterTypes option) const {
    //BAD workaround to see if the filter is GBF, OSIS or ThML!
    const QString name = backend()->configOptionName(option);

    if (m_module->getConfig().has("GlobalOptionFilter", QString("OSIS").append(name).toUtf8().constData())) {
        return true;
    }

    if (m_module->getConfig().has("GlobalOptionFilter", QString("GBF").append(name).toUtf8().constData())) {
        return true;
    }

    if (m_module->getConfig().has("GlobalOptionFilter", QString("ThML").append(name).toUtf8().constData())) {
        return true;
    }

    if (m_module->getConfig().has("GlobalOptionFilter", QString("UTF8").append(name).toUtf8().constData())) {
        return true;
    }

    if (m_module->getConfig().has("GlobalOptionFilter", name.toUtf8().constData())) {
        return true;
    }

    return false;
}

CSwordModuleInfo::TextDirection CSwordModuleInfo::textDirection() const {
    if (config(TextDir) == "RtoL")
        return CSwordModuleInfo::RightToLeft;

    return CSwordModuleInfo::LeftToRight;
}

void CSwordModuleInfo::write(CSwordKey *key, const QString &newText) {
    module()->setKey(key->key().toUtf8().constData());

    //don't store a pointer to the const char* value somewhere because QCString doesn't keep the value of it
    module()->setEntry(isUnicode() ? newText.toUtf8().constData() : newText.toLocal8Bit().constData());
}

bool CSwordModuleInfo::deleteEntry(CSwordKey * const key) {
    module()->setKey(isUnicode() ? key->key().toUtf8().constData() : key->key().toLocal8Bit().constData());

    if (module()) {
        module()->deleteEntry();
        return true;
    }

    return false;
}

void CSwordModuleInfo::initCachedCategory() {
    /// \todo Maybe we can use raw string comparsion instead of QString?
    const QString cat(m_module->getConfigEntry("Category"));

    /// \warning cat has to be checked before type() !!!
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
        switch (type()) {
            case Bible:       m_cachedCategory = Bibles; break;
            case Commentary:  m_cachedCategory = Commentaries; break;
            case Lexicon:     m_cachedCategory = Lexicons; break;
            case GenericBook: m_cachedCategory = Books; break;
            case Unknown: // Fall thru
            default:          m_cachedCategory = UnknownCategory; break;
        }
    }
}

void CSwordModuleInfo::initCachedLanguage() {
    CLanguageMgr *lm = CLanguageMgr::instance();
    if (category() == Glossary) {
        /*
          Special handling for glossaries, we use the "from language" as
          language for the module.
        */
        m_cachedLanguage = lm->languageForAbbrev(config(GlossaryFrom));
    } else {
        m_cachedLanguage = lm->languageForAbbrev(m_module->Lang());
    }
}

Rendering::CEntryDisplay * CSwordModuleInfo::getDisplay() const {
    return dynamic_cast<Rendering::CEntryDisplay *>(m_module->getDisplay());
}

QString CSwordModuleInfo::aboutText() const {
    QString text;
    text += "<table>";

    text += QString("<tr><td><b>%1</b></td><td>%2</td><tr>")
            .arg(tr("Version"))
            .arg(hasVersion() ? config(CSwordModuleInfo::ModuleVersion) : tr("unknown"));

    text += QString("<tr><td><b>%1</b></td><td>%2</td></tr>")
            .arg(tr("Markup"))
            .arg(!QString(m_module->getConfigEntry("SourceType")).isEmpty() ? QString(m_module->
                    getConfigEntry("SourceType")) : tr("unknown"));

    text += QString("<tr><td><b>%1</b></td><td>%2</td></tr>")
            .arg(tr("Location"))
            .arg(config(CSwordModuleInfo::AbsoluteDataPath));

    text += QString("<tr><td><b>%1</b></td><td>%2</td></tr>")
            .arg(tr("Language"))
            .arg(language()->translatedName());

    if (m_module->getConfigEntry("Category"))
        text += QString("<tr><td><b>%1</b></td><td>%2</td></tr>")
                .arg(tr("Category"))
                .arg(m_module->getConfigEntry("Category"));

    if (m_module->getConfigEntry("LCSH"))
        text += QString("<tr><td><b>%1</b></td><td>%2</td></tr>")
                .arg(tr("LCSH"))
                .arg(m_module->getConfigEntry("LCSH"));

    text += QString("<tr><td><b>%1</b></td><td>%2</td></tr>")
            .arg(tr("Writable"))
            .arg(isWritable() ? tr("yes") : tr("no"));

    if (isEncrypted())
        text += QString("<tr><td><b>%1</b></td><td>%2</td></tr>")
                .arg(tr("Unlock key"))
                .arg(config(CSwordModuleInfo::CipherKey));

    QString options;

    unsigned int opts;

    for (opts = CSwordModuleInfo::filterTypesMIN; opts <= CSwordModuleInfo::filterTypesMAX; ++opts) {
        if (has(static_cast < CSwordModuleInfo::FilterTypes > (opts))) {
            if (!options.isEmpty()) {
                options += QString::fromLatin1(", ");
            }

            options += CSwordBackend::translatedOptionName(static_cast < CSwordModuleInfo::FilterTypes > (opts));
        }
    }

    if (!options.isEmpty()) {
        text += QString("<tr><td><b>%1</b></td><td>%2</td></tr>")
                .arg(tr("Features"))
                .arg(options);
    }

    text += "</table><hr>";

    if (category() == Cult) //clearly say the module contains cult/questionable materials
        text += QString("<br/><b>%1</b><br/><br/>")
                .arg(tr("Take care, this work contains cult / questionable material!"));

    text += QString("<b>%1:</b><br/>%2</font>")
            .arg(tr("About"))
            .arg(config(AboutInformation));

    typedef QList<CSwordModuleInfo::ConfigEntry> ListConfigEntry;

    ListConfigEntry entries;

    entries.append(DistributionLicense);
    entries.append(DistributionSource);
    entries.append(DistributionNotes);
    entries.append(TextSource);
    entries.append(CopyrightNotes);
    entries.append(CopyrightHolder);
    entries.append(CopyrightDate);
    entries.append(CopyrightContactName);
    entries.append(CopyrightContactAddress);
    entries.append(CopyrightContactEmail);

    typedef QMap<CSwordModuleInfo::ConfigEntry, QString> MapConfigEntry;

    MapConfigEntry entryMap;

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

    for (ListConfigEntry::iterator it(entries.begin()); it != entries.end(); ++it) {
        QString t( config(*it) );

        if (!t.isEmpty()) {
            text += QString("<tr><td><b>%1</b></td><td>%2</td></tr>")
                    .arg(entryMap[*it])
                    .arg(config(*it));
        }

    }

    text += "</table></font>";

    return text;
}

QIcon CSwordModuleInfo::moduleIcon(const CSwordModuleInfo *module) {
    const QString &filename = moduleIconFilename(module);
    if (filename.isEmpty()) return QIcon();
    return util::directory::getIcon(filename);
}

const QString &CSwordModuleInfo::moduleIconFilename(
        const CSwordModuleInfo *module)
{
    const CSwordModuleInfo::Category cat(module->category());
    switch (cat) {
        case CSwordModuleInfo::Bibles:
            if (module->isLocked()) {
                return CResMgr::modules::bible::icon_locked;
            }
            else {
                return CResMgr::modules::bible::icon_unlocked;
            }
        case CSwordModuleInfo::Commentaries:
            if (module->isLocked()) {
                return CResMgr::modules::commentary::icon_locked;
            }
            else {
                return CResMgr::modules::commentary::icon_unlocked;
            }
        case CSwordModuleInfo::Lexicons:
            if (module->isLocked()) {
                return CResMgr::modules::lexicon::icon_locked;
            }
            else {
                return CResMgr::modules::lexicon::icon_unlocked;
            }
        case CSwordModuleInfo::Books:
            if (module->isLocked()) {
                return CResMgr::modules::book::icon_locked;
            }
            else {
                return CResMgr::modules::book::icon_unlocked;
            }
        case CSwordModuleInfo::Cult:
        case CSwordModuleInfo::Images:
        case CSwordModuleInfo::DailyDevotional:
        case CSwordModuleInfo::Glossary:
        case CSwordModuleInfo::UnknownCategory:
        default:
            return categoryIconFilename(cat);
    }
}

QIcon CSwordModuleInfo::categoryIcon(const CSwordModuleInfo::Category &category)
{
    QString filename = categoryIconFilename(category);
    if (filename.isEmpty()) return QIcon();
    return util::directory::getIcon(filename);
}

const QString &CSwordModuleInfo::categoryIconFilename(
        const CSwordModuleInfo::Category &category)
{
    static const QString noFilename;

    switch (category) {
        case CSwordModuleInfo::Bibles:
            return CResMgr::categories::bibles::icon;
        case CSwordModuleInfo::Commentaries:
            return CResMgr::categories::commentaries::icon;
        case CSwordModuleInfo::Books:
            return CResMgr::categories::books::icon;
        case CSwordModuleInfo::Cult:
            return CResMgr::categories::cults::icon;
        case CSwordModuleInfo::Images:
            return CResMgr::categories::images::icon;
        case CSwordModuleInfo::DailyDevotional:
            return CResMgr::categories::dailydevotional::icon;
        case CSwordModuleInfo::Lexicons:
            return CResMgr::categories::lexicons::icon;
        case CSwordModuleInfo::Glossary:
            return CResMgr::categories::glossary::icon;
        case CSwordModuleInfo::UnknownCategory:
        default:
            return noFilename;
    }
}

QString CSwordModuleInfo::categoryName(
    const CSwordModuleInfo::Category &category) {
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

QString CSwordModuleInfo::getSimpleConfigEntry(const QString& name) const {
    QString ret = isUnicode()
                  ? QString::fromUtf8(m_module->getConfigEntry(name.toUtf8().constData()))
                  : QString::fromLatin1(m_module->getConfigEntry(name.toUtf8().constData()));

    return ret.isEmpty() ? QString::null : ret;
}

QString CSwordModuleInfo::getFormattedConfigEntry(const QString& name) const {
    sword::SWBuf RTF_Buffer(m_module->getConfigEntry(name.toUtf8().constData()));
    sword::RTFHTML RTF_Filter;
    RTF_Filter.processText(RTF_Buffer, 0, 0);
    QString ret = isUnicode()
                  ? QString::fromUtf8(RTF_Buffer.c_str())
                  : QString::fromLatin1(RTF_Buffer.c_str());

    return ret.isEmpty() ? QString::null : ret;
}

bool CSwordModuleInfo::setHidden(bool hide) {
    if (m_hidden == hide) return false;

    m_hidden = hide;
    QStringList hiddenModules(CBTConfig::get(CBTConfig::hiddenModules));
    if (hide) {
        Q_ASSERT(!hiddenModules.contains(name()));
        hiddenModules.append(name());
    }
    else {
        Q_ASSERT(hiddenModules.contains(name()));
        hiddenModules.removeOne(name());
    }
    CBTConfig::set(CBTConfig::hiddenModules, hiddenModules);
    emit hiddenChanged(hide);
    return true;
}

