/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "cswordmoduleinfo.h"

#include <memory>
#include <cassert>
#include <CLucene.h>
#include <optional>
#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QLocale>
#include <QScopeGuard>
#include <QSettings>
#include <QTextDocument>
#include <string>
#include <string_view>
#include <type_traits>
#include "../../util/btassert.h"
#include "../../util/cresmgr.h"
#include "../../util/directory.h"
#include "../../util/tool.h"
#include "../config/btconfig.h"
#include "../keys/cswordkey.h"
#include "../managers/cswordbackend.h"
#include "../cswordmodulesearch.h"
#include "cswordbiblemoduleinfo.h"
#include "cswordlexiconmoduleinfo.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <listkey.h>
#include <swbuf.h>
#include <swconfig.h>
#include <swkey.h>
#include <swmodule.h>
#include <swversion.h>
#include <rtfhtml.h>
#include <versekey.h>
#pragma GCC diagnostic pop


//Increment this, if the index format changes
//Then indices on the user's systems will be rebuilt
constexpr static unsigned const INDEX_VERSION = 7;

//Maximum index entry size, 1MiB for now
//Lucene default is too small
constexpr static unsigned long const BT_MAX_LUCENE_FIELD_LENGTH = 1024 * 1024;

namespace {

inline CSwordModuleInfo::Category retrieveCategory(
    CSwordModuleInfo::ModuleType const type,
    CSwordModuleInfo::Features const features,
    sword::SWModule & module)
{
    /// \todo Maybe we can use raw string comparsion instead of QString?
    QString const cat(module.getConfigEntry("Category"));

    // Category has to be checked before type:
    if (cat == QStringLiteral("Cults / Unorthodox / Questionable Material")) {
        return CSwordModuleInfo::Cult;
    } else if (cat == QStringLiteral("Daily Devotional")
               || features.testFlag(CSwordModuleInfo::FeatureDailyDevotion))
    {
        return CSwordModuleInfo::DailyDevotional;
    } else if (cat == QStringLiteral("Glossaries")
               || features.testFlag(CSwordModuleInfo::FeatureGlossary))
    {
        return CSwordModuleInfo::Glossary;
    } else if (cat == QStringLiteral("Images")
               || cat == QStringLiteral("Maps"))
    {
        return CSwordModuleInfo::Images;
    } else {
        switch (type) {
            case CSwordModuleInfo::Bible:
                return CSwordModuleInfo::Bibles;
            case CSwordModuleInfo::Commentary:
                return CSwordModuleInfo::Commentaries;
            case CSwordModuleInfo::Lexicon:
                return CSwordModuleInfo::Lexicons;
            case CSwordModuleInfo::GenericBook:
                return CSwordModuleInfo::Books;
            case CSwordModuleInfo::Unknown: // Fall thru
            default:
                return CSwordModuleInfo::UnknownCategory;
        }
    }
}

inline CSwordModuleInfo::Features retrieveFeatures(
    sword::SWModule const & module)
{
    // See https://wiki.crosswire.org/DevTools:conf_Files
    CSwordModuleInfo::Features features;
    for (auto featureEntries = module.getConfig().equal_range("Feature");
         featureEntries.first != featureEntries.second;
         ++featureEntries.first)
    {
        #define T(f) \
            if (featureEntries.first->second == #f) { \
                features |= CSwordModuleInfo::Feature ## f; \
            }
        T(StrongsNumbers)
        else T(GreekDef)
        else T(HebrewDef)
        else T(GreekParse)
        else T(HebrewParse)
        else T(DailyDevotion)
        else T(Glossary)
        else T(Images)
        else T(NoParagraphs)
        #undef T
    }
    return features;
}

static const TCHAR * stop_words[] = { nullptr };

class Analyzer : public lucene::analysis::standard::StandardAnalyzer {

public: // Methods:

    Analyzer() : lucene::analysis::standard::StandardAnalyzer(stop_words) {}

};

} // anonymous namespace

char const *
CSwordModuleInfo::FilterOption::valueToOnOff(int const value) noexcept
{ return value ? "On" : "Off"; }

char const *
CSwordModuleInfo::FilterOption::valueToReadings(int const value) noexcept {
    switch (value) {
    case 0: return "Primary Reading";
    case 1: return "Secondary Reading";
    default: return "All Readings";
    }
}

CSwordModuleInfo::FilterOption const CSwordModuleInfo::footnotes{
    "Footnotes",
    "Footnotes",
    QT_TRANSLATE_NOOP("QObject", "Footnotes")};

CSwordModuleInfo::FilterOption const CSwordModuleInfo::strongNumbers{
    "Strong's Numbers",
    "Strongs",
    QT_TRANSLATE_NOOP("QObject", "Strong's numbers")};

CSwordModuleInfo::FilterOption const CSwordModuleInfo::headings{
    "Headings",
    "Headings",
     QT_TRANSLATE_NOOP("QObject", "Headings")};

CSwordModuleInfo::FilterOption const CSwordModuleInfo::morphTags{
     "Morphological Tags",
     "Morph",
     QT_TRANSLATE_NOOP("QObject", "Morphological tags")};

CSwordModuleInfo::FilterOption const CSwordModuleInfo::lemmas{
    "Lemmas",
    "Lemma",
    QT_TRANSLATE_NOOP("QObject", "Lemmas")};

CSwordModuleInfo::FilterOption const CSwordModuleInfo::hebrewPoints{
    "Hebrew Vowel Points",
    "HebrewPoints",
    QT_TRANSLATE_NOOP("QObject", "Hebrew vowel points")};

CSwordModuleInfo::FilterOption const CSwordModuleInfo::hebrewCantillation{
    "Hebrew Cantillation",
    "Cantillation",
    QT_TRANSLATE_NOOP("QObject", "Hebrew cantillation marks")};

CSwordModuleInfo::FilterOption const CSwordModuleInfo::greekAccents{
    "Greek Accents",
    "GreekAccents",
    QT_TRANSLATE_NOOP("QObject", "Greek accents")};

CSwordModuleInfo::FilterOption const CSwordModuleInfo::scriptureReferences{
    "Cross-references",
    "Scripref",
    QT_TRANSLATE_NOOP("QObject", "Scripture cross-references")};

CSwordModuleInfo::FilterOption const CSwordModuleInfo::redLetterWords{
    "Words of Christ in Red",
    "RedLetterWords",
    QT_TRANSLATE_NOOP("QObject", "Red letter words")};

CSwordModuleInfo::FilterOption const CSwordModuleInfo::textualVariants{
    "Textual Variants",
    "Variants",
    QT_TRANSLATE_NOOP("QObject", "Textual variants"),
    &CSwordModuleInfo::FilterOption::valueToReadings};

CSwordModuleInfo::FilterOption const CSwordModuleInfo::morphSegmentation{
    "Morph Segmentation",
    "MorphSegmentation",
    QT_TRANSLATE_NOOP("QObject", "Morph segmentation")};

CSwordModuleInfo::CSwordModuleInfo(sword::SWModule & module,
                                   CSwordBackend & backend,
                                   ModuleType type)
    : m_swordModule(module)
    , m_backend(backend)
    , m_type(type)
    , m_cancelIndexing(false)
    , m_cachedName(QString::fromUtf8(module.getName()))
    , m_cachedFeatures(retrieveFeatures(module))
    , m_cachedCategory(retrieveCategory(type, m_cachedFeatures, module))
    , m_cachedLanguage(
        Language::fromAbbrev(
              util::tool::fixSwordBcp47(
                  m_cachedCategory == Glossary
                  /* Special handling for glossaries, we use the "from language" as
                     language for the module: */
                  ? config(GlossaryFrom)
                  : module.getLanguage())))
    , m_cachedGlossaryTargetLanguage(
        m_cachedCategory == Glossary
        ? Language::fromAbbrev(
              util::tool::fixSwordBcp47(module.getLanguage()))
        : std::shared_ptr<Language const>())
    , m_cachedHasVersion(
          ((*m_backend.getConfig())[module.getName()]["Version"]).size() > 0)
{
    m_hidden = btConfig().value<QStringList>(
                   QStringLiteral("state/hiddenModules"))
               .contains(m_cachedName);

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

bool CSwordModuleInfo::unlock(const QString & unlockKey) {
    if (!isEncrypted())
        return false;

    bool unlocked = unlockKeyIsValid();

    btConfig().setModuleEncryptionKey(m_cachedName, unlockKey);

    /// \todo remove this comment once it is no longer needed
    /* There is currently a deficiency in sword 1.8.1 in that
       backend->setCipherKey() does not work correctly for modules from which
       data was already fetched. Therefore we have to reload the modules in
       bibletime.cpp */
    m_backend.raw().setCipherKey(m_swordModule.getName(),
                                 unlockKey.toUtf8().constData());

    /// \todo write to Sword config as well

    if (unlockKeyIsValid() != unlocked)
        Q_EMIT unlockedChanged(!unlocked);
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
    auto const & sections = m_backend.getConfig()->getSections();
    auto const it(sections.find(m_cachedName.toUtf8().constData()));
    if (it == sections.end())
        return false;

    const sword::ConfigEntMap & config = it->second;
    return config.find("CipherKey") != config.end();
}

bool CSwordModuleInfo::unlockKeyIsValid() const {
    sword::SWKey * const key = m_swordModule.getKey();
    sword::VerseKey * const vk = dynamic_cast<sword::VerseKey *>(key);
    if (vk)
        vk->setIntros(false);
    m_swordModule.setPosition(sword::TOP);

    /* This needs to use ::fromLatin1 because if the text is still locked, a lot
       of garbage will show up. It will also work with properly decrypted
       Unicode text, because all non-ASCII Unicode chars consist of bytes >127
       and therefore contain no control (nonprintable) characters, which are all
       <127. */
    const QString test(isUnicode()
                       ? QString::fromUtf8(m_swordModule.getRawEntry())
                       : QString::fromLatin1(m_swordModule.getRawEntry()));

    if (test.isEmpty())
        return false;

    for (int i = 0; i < test.length() && i < 100; i++)
        if (!test[i].isPrint() && !test[i].isNull())
            return false;
    return true;
}

QString CSwordModuleInfo::getUnlockInfo() {
    return m_swordModule.getConfigEntry("UnlockInfo");
}

QString CSwordModuleInfo::getGlobalBaseIndexLocation() {
    return util::directory::getUserIndexDir().absolutePath();
}

QString CSwordModuleInfo::getModuleBaseIndexLocation() const {
    return QStringLiteral("%1/%2").arg(getGlobalBaseIndexLocation(),
                                       m_cachedName.toLocal8Bit());
}

QString CSwordModuleInfo::getModuleStandardIndexLocation() const {
    // This for now returns the location of the main index
    return getModuleBaseIndexLocation() + QStringLiteral("/standard");
}

bool CSwordModuleInfo::hasIndex() const {
    { // Is this a directory?
        QFileInfo fi(getModuleStandardIndexLocation());
        if (!fi.isDir())
            return false;
    }

    // Are the index version and module version OK?
    QSettings module_config(getModuleBaseIndexLocation()
                            + QStringLiteral("/bibletime-index.conf"),
                            QSettings::IniFormat);

    if (m_cachedHasVersion
        && module_config.value(QStringLiteral("module-version")).toString()
           != config(CSwordModuleInfo::ModuleVersion))
    {
        return false;
    }

    if (module_config.value(QStringLiteral("index-version")).toUInt()
        != INDEX_VERSION)
    {
        qDebug("%s: INDEX_VERSION is not compatible with this version of "
               "BibleTime.",
               m_cachedName.toUtf8().constData());
        return false;
    }

    // Is the index there?
    return lucene::index::IndexReader::indexExists(getModuleStandardIndexLocation()
                                                   .toLatin1().constData());
}

bool CSwordModuleInfo::hasImportantFilterOption() const {
    if (m_type == CSwordModuleInfo::Bible) {
        if (has(CSwordModuleInfo::strongNumbers) ||
            has(CSwordModuleInfo::morphTags) ||
            has(CSwordModuleInfo::footnotes) ||
                has(CSwordModuleInfo::headings))
            return true;
    }
    return false;
}

inline void CSwordModuleInfo::setImportantFilterOptions(bool enable) {
    m_backend.setOption(CSwordModuleInfo::strongNumbers, enable);
    m_backend.setOption(CSwordModuleInfo::morphTags, enable);
    m_backend.setOption(CSwordModuleInfo::footnotes, enable);
    m_backend.setOption(CSwordModuleInfo::headings, enable);
}

void CSwordModuleInfo::buildIndex() {
    auto cleanup =
            qScopeGuard(
                [this]() noexcept
                { m_cancelIndexing.store(false, std::memory_order_relaxed); });
#define CANCEL_INDEXING (m_cancelIndexing.load(std::memory_order_relaxed))

    try {
        // Without this we don't get strongs, lemmas, etc.
        m_backend.setFilterOptions(btConfig().getFilterOptions());
        /* Make sure we reset all important filter options which influcence the
           plain filters. Turn on these options, they are needed for the
           EntryAttributes population */
        setImportantFilterOptions(true);

        /* We don't want the following in the text, the do not carry searchable
           information. */
        m_backend.setOption(CSwordModuleInfo::morphSegmentation, false);
        m_backend.setOption(CSwordModuleInfo::scriptureReferences, false);
        m_backend.setOption(CSwordModuleInfo::redLetterWords, false);

        // Do not use any stop words:
        Analyzer analyzer;
        const QString index(getModuleStandardIndexLocation());

        QDir dir(QStringLiteral("/"));
        dir.mkpath(getGlobalBaseIndexLocation());
        dir.mkpath(getModuleBaseIndexLocation());
        dir.mkpath(getModuleStandardIndexLocation());

        if (lucene::index::IndexReader::indexExists(index.toLatin1().constData()))
            if (lucene::index::IndexReader::isLocked(index.toLatin1().constData()))
                lucene::index::IndexReader::unlock(index.toLatin1().constData());

        // Always create a new index:
        auto writer =
            std::make_optional<lucene::index::IndexWriter>(
                index.toLatin1().constData(),
                &analyzer,
                true);
        writer->setMaxFieldLength(BT_MAX_LUCENE_FIELD_LENGTH);
        writer->setUseCompoundFile(true); // Merge segments into a single file

        CSwordBibleModuleInfo *bm = qobject_cast<CSwordBibleModuleInfo*>(this);

        unsigned long verseLowIndex;
        unsigned long verseHighIndex;

        if(bm)
        {
            verseLowIndex = bm->lowerBound().index();
            verseHighIndex = bm->upperBound().index();
        }
        else
        {
            m_swordModule.setPosition(sword::TOP);
            verseLowIndex = m_swordModule.getIndex();
            m_swordModule.setPosition(sword::BOTTOM);
            verseHighIndex = m_swordModule.getIndex();
        }

        // verseLowIndex is not 0 in all cases (i.e. NT-only modules)
        unsigned long verseIndex = verseLowIndex + 1;
        unsigned long verseSpan = verseHighIndex - verseLowIndex;

        // Index() is not implemented properly for lexicons, so work around it:
        if (m_type == CSwordModuleInfo::Lexicon) {
            verseIndex = 0;
            verseLowIndex = 0;
            verseSpan = static_cast<CSwordLexiconModuleInfo *>(this)->entries().size();
        }

        Q_EMIT indexingProgress(0);

        sword::SWKey * const key = m_swordModule.getKey();
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
        m_swordModule.setSkipConsecutiveLinks(true);

        auto const sPwcharBuffer =
            std::make_unique<wchar_t[]>(BT_MAX_LUCENE_FIELD_LENGTH + 1);
        wchar_t * const wcharBuffer = sPwcharBuffer.get();
        BT_ASSERT(wcharBuffer);

        if(bm && vk) // Implied that vk could be null due to cast above
            vk->setIndex(bm->lowerBound().index());
        else
            m_swordModule.setPosition(sword::TOP);

        bool importantFilterOption = hasImportantFilterOption();

        while (!(m_swordModule.popError()) && !CANCEL_INDEXING) {

            /* Also index Chapter 0 and Verse 0, because they might have
               information in the entry attributes. We used to just put their
               content into the textBuffer and continue to the next verse, but
               with entry attributes this doesn't work any more. Hits in the
               search dialog will show up as 1:1 (instead of 0). */

            lucene::document::Document doc;

            //index the key
            lucene_utf8towcs(wcharBuffer, key->getText(), BT_MAX_LUCENE_FIELD_LENGTH);

            doc.add(*(new lucene::document::Field(static_cast<const TCHAR *>(_T("key")),
                                                  static_cast<const TCHAR *>(wcharBuffer),
                                                  lucene::document::Field::STORE_YES
                                                  | lucene::document::Field::INDEX_NO)));

            if (importantFilterOption) {
                // Index text including strongs, morph, footnotes, and headings.
                setImportantFilterOptions(true);
                textBuffer.append(m_swordModule.stripText());
                lucene_utf8towcs(wcharBuffer,
                                 static_cast<const char *>(textBuffer),
                                 BT_MAX_LUCENE_FIELD_LENGTH);
                doc.add(*(new lucene::document::Field(static_cast<const TCHAR *>(_T("content")),
                                                      static_cast<const TCHAR *>(wcharBuffer),
                                                      lucene::document::Field::STORE_NO
                                                      | lucene::document::Field::INDEX_TOKENIZED)));
                textBuffer.clear();
            }

            // Index text without strongs, morph, footnotes, and headings.
            setImportantFilterOptions(false);
            textBuffer.append(m_swordModule.stripText());
            lucene_utf8towcs(wcharBuffer,
                             static_cast<const char *>(textBuffer),
                             BT_MAX_LUCENE_FIELD_LENGTH);
            doc.add(*(new lucene::document::Field(static_cast<const TCHAR *>(_T("content")),
                                                  static_cast<const TCHAR *>(wcharBuffer),
                                                  lucene::document::Field::STORE_NO
                                                  | lucene::document::Field::INDEX_TOKENIZED)));
            textBuffer.clear();

            for (auto & vp : m_swordModule.getEntryAttributes()["Footnote"]) {
                lucene_utf8towcs(wcharBuffer, vp.second["body"], BT_MAX_LUCENE_FIELD_LENGTH);
                doc.add(*(new lucene::document::Field(static_cast<const TCHAR *>(_T("footnote")),
                                                      static_cast<const TCHAR *>(wcharBuffer),
                                                      lucene::document::Field::STORE_NO
                                                      | lucene::document::Field::INDEX_TOKENIZED)));
            }

            // Headings
            for (auto & vp
                 : m_swordModule.getEntryAttributes()["Heading"]["Preverse"])
            {
                lucene_utf8towcs(wcharBuffer, vp.second, BT_MAX_LUCENE_FIELD_LENGTH);
                doc.add(*(new lucene::document::Field(static_cast<const TCHAR *>(_T("heading")),
                                                      static_cast<const TCHAR *>(wcharBuffer),
                                                      lucene::document::Field::STORE_NO
                                                      | lucene::document::Field::INDEX_TOKENIZED)));
            }

            // Strongs/Morphs
            for (auto const & vp : m_swordModule.getEntryAttributes()["Word"]) {
                auto const & attrs = vp.second;
                auto const partCountIter(attrs.find("PartCount"));
                int partCount = (partCountIter != attrs.end())
                                ? QString(partCountIter->second).toInt()
                                : 0;
                for (int i=0; i<partCount; i++) {

                    sword::SWBuf lemmaKey = "Lemma";
                    if (partCount > 1)
                        lemmaKey.appendFormatted(".%d", i+1);
                    auto const lemmaIter(attrs.find(lemmaKey));
                    if (lemmaIter != attrs.end()) {
                        lucene_utf8towcs(wcharBuffer, lemmaIter->second, BT_MAX_LUCENE_FIELD_LENGTH);
                        doc.add(*(new lucene::document::Field(static_cast<const TCHAR *>(_T("strong")),
                                                              static_cast<const TCHAR *>(wcharBuffer),
                                                              lucene::document::Field::STORE_NO
                                                              | lucene::document::Field::INDEX_TOKENIZED)));
                    }

                }

                auto const morphIter(attrs.find("Morph"));
                if (morphIter != attrs.end()) {
                    lucene_utf8towcs(wcharBuffer, morphIter->second, BT_MAX_LUCENE_FIELD_LENGTH);
                    doc.add(*(new lucene::document::Field(static_cast<const TCHAR *>(_T("morph")),
                                                          static_cast<const TCHAR *>(wcharBuffer),
                                                          lucene::document::Field::STORE_NO
                                                          | lucene::document::Field::INDEX_TOKENIZED)));
                }
            }

            writer->addDocument(&doc);
            //Index() is not implemented properly for lexicons, so we use a
            //workaround.
            if (m_type == CSwordModuleInfo::Lexicon) {
                verseIndex++;
            } else {
                verseIndex = m_swordModule.getIndex();
            }

            if (verseIndex % 200 == 0) {
                if (verseSpan == 0) { // Prevent division by zero
                    Q_EMIT indexingProgress(0);
                } else {
                    Q_EMIT indexingProgress(
                            static_cast<int>(
                                    (100 * (verseIndex - verseLowIndex))
                                    / verseSpan));
                }
            }

            m_swordModule.increment();
        } // while (!(m_module.Error()) && !CANCEL_INDEXING)

        if (!CANCEL_INDEXING)
            writer->optimize();
        writer->close();
        writer.reset();

        if (CANCEL_INDEXING) {
            deleteIndex();
        } else {
            QSettings module_config(getModuleBaseIndexLocation()
                                    + QStringLiteral("/bibletime-index.conf"),
                                    QSettings::IniFormat);
            if (m_cachedHasVersion)
                module_config.setValue(QStringLiteral("module-version"),
                                       config(CSwordModuleInfo::ModuleVersion));
            module_config.setValue(QStringLiteral("index-version"),
                                   INDEX_VERSION);
            Q_EMIT hasIndexChanged(true);
        }
    // } catch (CLuceneError & e) {
    } catch (...) {
        deleteIndex();
        throw;
    }
}

void CSwordModuleInfo::deleteIndex() {
    deleteIndexForModule(m_cachedName);
    Q_EMIT hasIndexChanged(false);
}

void CSwordModuleInfo::deleteIndexForModule(const QString & name) {
    QDir(QStringLiteral("%1/%2").arg(getGlobalBaseIndexLocation(), name))
            .removeRecursively();
}

::qint64 CSwordModuleInfo::indexSize() const {
    namespace DU = util::directory;
    return DU::getDirSizeRecursive(getModuleBaseIndexLocation());
}

CSwordModuleSearch::ModuleResultList
CSwordModuleInfo::searchIndexed(QString const & searchedText,
                                sword::ListKey const & scope) const
{
    auto const sPutfBuffer =
        std::make_unique<char[]>(BT_MAX_LUCENE_FIELD_LENGTH  + 1);
    auto const sPwcharBuffer =
        std::make_unique<wchar_t[]>(BT_MAX_LUCENE_FIELD_LENGTH  + 1);
    char * const utfBuffer = sPutfBuffer.get();
    BT_ASSERT(utfBuffer);
    wchar_t * const wcharBuffer = sPwcharBuffer.get();
    BT_ASSERT(wcharBuffer);

    // work around Swords thread insafety for Bibles and Commentaries
    m_swordModule.setKey(createKey()->asSwordKey());

    // do not use any stop words
    Analyzer analyzer;
    lucene::search::IndexSearcher searcher(getModuleStandardIndexLocation().toLatin1().constData());
    lucene_utf8towcs(wcharBuffer, searchedText.toUtf8().constData(), BT_MAX_LUCENE_FIELD_LENGTH);
    std::unique_ptr<lucene::search::Query> q(lucene::queryParser::QueryParser::parse(static_cast<const TCHAR *>(wcharBuffer),
                                                                                     static_cast<const TCHAR *>(_T("content")),
                                                                                     &analyzer));

    std::unique_ptr<lucene::search::Hits> h(
                searcher.search(q.get(), lucene::search::Sort::INDEXORDER()));

    const bool useScope = (scope.getCount() > 0);

    lucene::document::Document * doc = nullptr;
    std::unique_ptr<sword::SWKey> swKey(m_swordModule.createKey());

    sword::VerseKey * const vk = dynamic_cast<sword::VerseKey *>(swKey.get());
    if (vk)
        vk->setIntros(true);

    CSwordModuleSearch::ModuleResultList results;
    for (size_t i = 0; i < h->length(); ++i) {
        doc = &h->doc(i);
        lucene_wcstoutf8(utfBuffer,
                         static_cast<const wchar_t *>(doc->get(static_cast<const TCHAR *>(_T("key")))),
                         BT_MAX_LUCENE_FIELD_LENGTH);

        swKey->setText(utfBuffer);

        // Limit results based on scope:
        if (useScope) {
            for (int j = 0; j < scope.getCount(); j++) {
                if (auto const * const vkey =
                            dynamic_cast<sword::VerseKey const *>(
                                scope.getElement(j)))
                {
                    if (vkey->getLowerBound().compare(*swKey) <= 0
                        && vkey->getUpperBound().compare(*swKey) >= 0)
                        results.emplace_back(swKey->clone());
                }
            }
        } else { // No scope, give me all buffers
            results.emplace_back(swKey->clone());
        }
    }

    return results;
}

sword::SWVersion CSwordModuleInfo::minimumSwordVersion() const {
    return sword::SWVersion(config(CSwordModuleInfo::MinimumSwordVersion)
                            .toUtf8().constData());
}

QString CSwordModuleInfo::config(const CSwordModuleInfo::ConfigEntry entry) const {
    switch (entry) {

        case AboutInformation:
            return getFormattedConfigEntry(QStringLiteral("About"));

        case CipherKey: {
            if (btConfig().getModuleEncryptionKey(m_cachedName).isNull()) {
                return QString(m_swordModule.getConfigEntry("CipherKey")); // Fallback
            } else {
                return btConfig().getModuleEncryptionKey(m_cachedName);
            }
        }

        case AbsoluteDataPath: {
            auto path(getSimpleConfigEntry(QStringLiteral("AbsoluteDataPath")));
            if (!path.endsWith('/')) /// \todo is this needed?
                path.append('/');

            return path;
        }

        case DataPath: {
            QString path(getSimpleConfigEntry(QStringLiteral("DataPath")));

            // Make sure we remove the dataFile part if it's a Lexicon
            if (m_type == CSwordModuleInfo::GenericBook
                || m_type == CSwordModuleInfo::Lexicon)
            {
                int pos = path.lastIndexOf('/'); // Last slash in the string
                if (pos != -1)
                    path = path.left(pos + 1); // Include the slash
            }
            return path;
        }

        case Description:
            return getFormattedConfigEntry(QStringLiteral("Description"));

        case ModuleVersion: {
            auto const version(getSimpleConfigEntry(QStringLiteral("Version")));
            return version.isEmpty() ? QStringLiteral("1.0") : version;
        }

        case MinimumSwordVersion: {
            auto const minimumVersion =
                    getSimpleConfigEntry(QStringLiteral("MinimumVersion"));
            return minimumVersion.isEmpty()
                    ? QStringLiteral("0.0")
                    : minimumVersion;
        }

        case TextDir: {
            auto const dir = getSimpleConfigEntry(QStringLiteral("Direction"));
            return dir.isEmpty() ? QStringLiteral("LtoR") : dir;
        }

        case DisplayLevel: {
            auto const level =
                    getSimpleConfigEntry(QStringLiteral("DisplayLevel"));
            return level.isEmpty() ? QStringLiteral("1") : level;
        }

        case GlossaryFrom: {
            if (m_cachedCategory != Glossary)
                return {};

            return getSimpleConfigEntry(QStringLiteral("GlossaryFrom"));
        }

        case GlossaryTo: {
            if (m_cachedCategory != Glossary)
                return {};

            return getSimpleConfigEntry(QStringLiteral("GlossaryTo"));
        }

        case Markup: {
            auto const markup =
                    getSimpleConfigEntry(QStringLiteral("SourceType"));
            return markup.isEmpty() ? QStringLiteral("Unknown") : markup;
        }

        case DistributionLicense:
            return getFormattedConfigEntry(
                        QStringLiteral("DistributionLicense"));

        case DistributionSource:
            return getFormattedConfigEntry(
                        QStringLiteral("DistributionSource"));

        case DistributionNotes:
            return getFormattedConfigEntry(QStringLiteral("DistributionNotes"));

        case TextSource:
            return getFormattedConfigEntry(QStringLiteral("TextSource"));

        case CopyrightNotes:
            return getFormattedConfigEntry(QStringLiteral("CopyrightNotes"));

        case CopyrightHolder:
            return getFormattedConfigEntry(QStringLiteral("CopyrightHolder"));

        case CopyrightDate:
            return getFormattedConfigEntry(QStringLiteral("CopyrightDate"));

        case CopyrightContactName:
            return getFormattedConfigEntry(
                        QStringLiteral("CopyrightContactName"));

        case CopyrightContactAddress:
            return getFormattedConfigEntry(
                        QStringLiteral("CopyrightContactAddress"));

        case CopyrightContactEmail:
            return getFormattedConfigEntry(
                        QStringLiteral("CopyrightContactEmail"));

        default:
            return {};
    }
}

bool CSwordModuleInfo::has(CSwordModuleInfo::FilterOption const & option) const {
    /// \todo This is a BAD workaround to see if the filter is GBF, OSIS or ThML!
    auto const & originalOptionName = option.configOptionName;
    using namespace std::string_literals;
    std::string const optionNames[] = {
        "OSIS"s + originalOptionName,
        "GBF"s + originalOptionName,
        "ThML"s + originalOptionName,
        "UTF8"s + originalOptionName,
        originalOptionName
    };
    for (auto [it, end] =
                m_swordModule.getConfig().equal_range("GlobalOptionFilter");
         it != end;
         ++it)
    {
        auto const & valueBuf = it->second;
        std::string_view const value(valueBuf.c_str(), valueBuf.size());
        for (auto const & optionName : optionNames)
            if (value == optionName)
                return true;
    }
    return false;
}

CSwordModuleInfo::TextDirection CSwordModuleInfo::textDirection() const {
    return (config(TextDir) == QStringLiteral("RtoL"))
            ? RightToLeft
            : LeftToRight;
}

char const * CSwordModuleInfo::textDirectionAsHtml() const
{ return textDirection() == RightToLeft ? "rtl" : "ltr"; }

void CSwordModuleInfo::write(CSwordKey * key, const QString & newText) {
    m_swordModule.setKey(key->key().toUtf8().constData());

    /* Don't store a pointer to the const char* value somewhere because QCString
      doesn't keep the value of it. */
    m_swordModule.setEntry(isUnicode()
                      ? newText.toUtf8().constData()
                      : newText.toLocal8Bit().constData());
}

void CSwordModuleInfo::deleteEntry(CSwordKey * const key) {
    BT_ASSERT(key);
    m_swordModule.setKey(isUnicode()
                    ? key->key().toUtf8().constData()
                    : key->key().toLocal8Bit().constData());
    m_swordModule.deleteEntry();
}

QString CSwordModuleInfo::aboutText() const {
    static auto const row(
                QStringLiteral("<tr><td><b>%1</b></td><td>%2</td></tr>"));

    auto text(QStringLiteral("<table>"));

    text += row
            .arg(tr("Version"))
            .arg(m_cachedHasVersion
                 ? config(CSwordModuleInfo::ModuleVersion).toHtmlEscaped()
                 : tr("unknown"));

    {
        const QString sourceType(m_swordModule.getConfigEntry("SourceType"));
        text += row
                .arg(tr("Markup"))
                .arg(!sourceType.isEmpty()
                     ? sourceType.toHtmlEscaped()
                     : tr("unknown"));
    }

    text += row
            .arg(tr("Location"))
            .arg(config(CSwordModuleInfo::AbsoluteDataPath).toHtmlEscaped());

    text += row
            .arg(tr("Language"))
            .arg(m_cachedLanguage->translatedName().toHtmlEscaped());

    if (char const * const e = m_swordModule.getConfigEntry("Category"))
        text += row.arg(tr("Category"))
                   .arg(QString{e}.toHtmlEscaped());

    if (char const * const e = m_swordModule.getConfigEntry("LCSH"))
        text += row.arg(tr("LCSH"))
                   .arg(QString{e}.toHtmlEscaped());

    text += row
            .arg(tr("Writable"))
            .arg(isWritable() ? tr("yes") : tr("no"));

    if (isEncrypted()) {
        text += row
                .arg(tr("Unlock key"))
                .arg(config(CSwordModuleInfo::CipherKey).toHtmlEscaped());
        if (char const * const e = m_swordModule.getConfigEntry("UnlockInfo"))
            text += row.arg(tr("Unlock info")).arg(QString(e).toHtmlEscaped());
    }

    QString options;
    static constexpr auto const allFilterOptions = {
        &footnotes,
        &strongNumbers,
        &headings,
        &morphTags,
        &lemmas,
        &hebrewPoints,
        &hebrewCantillation,
        &greekAccents,
        &scriptureReferences,
        &redLetterWords,
        &textualVariants,
        &morphSegmentation,
    };
    for (auto const * const filterOption : allFilterOptions) {
        if (has(*filterOption)) {
            if (!options.isEmpty())
                options += QStringLiteral(", ");
            options += QObject::tr(filterOption->translatableOptionName);
        }
    }

    if (!options.isEmpty())
        text += row
                .arg(tr("Features"))
                .arg(options.toHtmlEscaped());

    text += QStringLiteral("</table><hr>");

    // Clearly say the module contains cult/questionable materials
    if (m_cachedCategory == Cult)
        text += QStringLiteral("<br/><b>%1</b><br/><br/>")
                .arg(tr("Take care, this work contains cult / questionable "
                        "material!"));

    text += QStringLiteral("<b>%1:</b><br/>%2")
            .arg(tr("About"))
            .arg(config(AboutInformation)); // May contain HTML, don't escape

    text += QStringLiteral("<hr><table>");

    struct Entry { ConfigEntry const type; char const * const text; };
    auto const entries = {
        Entry{DistributionLicense, QT_TR_NOOP("Distribution license")},
        Entry{DistributionSource, QT_TR_NOOP("Distribution source")},
        Entry{DistributionNotes, QT_TR_NOOP("Distribution notes")},
        Entry{TextSource, QT_TR_NOOP("Text source")},
        Entry{CopyrightNotes, QT_TR_NOOP("Copyright notes")},
        Entry{CopyrightHolder, QT_TR_NOOP("Copyright holder")},
        Entry{CopyrightDate, QT_TR_NOOP("Copyright date")},
        Entry{CopyrightContactName, QT_TR_NOOP("Copyright contact name")},
        Entry{CopyrightContactAddress, QT_TR_NOOP("Copyright contact address")},
        Entry{CopyrightContactEmail, QT_TR_NOOP("Copyright contact email")},
    };
    for (auto const & entry : entries) {
        auto const value = config(entry.type);
        if (!value.isEmpty())
            text += row.arg(tr(entry.text).toHtmlEscaped())
                       .arg(value.toHtmlEscaped());
    }

    return text + QStringLiteral("</table>");
}

bool CSwordModuleInfo::isUnicode() const noexcept
{ return m_swordModule.isUnicode(); }

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

QString CSwordModuleInfo::getSimpleConfigEntry(const QString & name) const {
    auto const * const value =
            m_swordModule.getConfigEntry(name.toUtf8().constData());
    return isUnicode() ? QString::fromUtf8(value) : QString::fromLatin1(value);
}

/// \note See http://www.crosswire.org/wiki/DevTools:conf_Files#Localization
QString CSwordModuleInfo::getFormattedConfigEntry(const QString & name) const {
    auto const localeNames(
                QLocale(CSwordBackend::instance().booknameLanguage())
                    .uiLanguages());
    for (int i = localeNames.size() - 1; i >= -1; --i) {
        sword::SWBuf RTF_Buffer;
        if (i < 0) {
            RTF_Buffer =
                    m_swordModule.getConfigEntry(name.toUtf8().constData());
        } else {
            RTF_Buffer =
                    m_swordModule.getConfigEntry(
                        QStringLiteral("%1_%2")
                        .arg(name, localeNames[i])
                        .toUtf8().constData());
        }
        if (RTF_Buffer.length() > 0) {
            sword::RTFHTML RTF_Filter;
            RTF_Filter.processText(RTF_Buffer, nullptr, nullptr);
            return isUnicode()
                   ? QString::fromUtf8(RTF_Buffer.c_str())
                   : QString::fromLatin1(RTF_Buffer.c_str());
        }
    }
    return {};
}

bool CSwordModuleInfo::setHidden(bool hide) {
    if (m_hidden == hide)
        return false;

    m_hidden = hide;
    static auto const configKey = QStringLiteral("state/hiddenModules");
    QStringList hiddenModules(btConfig().value<QStringList>(configKey));
    BT_ASSERT(hiddenModules.contains(m_cachedName) != hide);
    if (hide) {
        hiddenModules.append(m_cachedName);
    } else {
        hiddenModules.removeOne(m_cachedName);
    }
    btConfig().setValue(configKey, hiddenModules);
    Q_EMIT hiddenChanged(hide);
    return true;
}
