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

#pragma once

#include <QHash>
#include <QList>
#include <QString>
#include <QStringList>


/**
  \brief Manages the languages and provides functions to work with them.
  \note This is a singleton.
*/
class CLanguageMgr {

public: /* Types: */

    /** \brief A language descriptor for CLanguageMgr. */
    class Language {

        friend class CLanguageMgr;
        friend class BtFontSettingsPage;

    public: /* Methods: */

        /** \returns the abbreviation of the this language. */
        QString const & abbrev() const {
            if (m_abbrev.isEmpty() && m_altAbbrevs.count())
                /* No standard abbrev but alternative ones */
                return m_altAbbrevs.first();
            return m_abbrev;
        }

        /** \returns the translated name of this language. */
        QString const & translatedName() const
        { return m_translatedName; }

        /** \returns the english name of this language. */
        QString const & englishName() const { return m_englishName; }

        /** \returns a list of alternative abbreviations for this language. */
        QStringList const & alternativeAbbrevs() const
        { return m_altAbbrevs; }

        /** \returns whether this language object is valid, i.e. has an
                     abbreviation and an english name. */
        bool isValid() const
        { return (!m_abbrev.isEmpty() && !m_englishName.isEmpty()); }

    private: /* Methods: */

        Language() = default;

        Language(char const * abbrev,
                 char const * englishName,
                 QString const & translatedName)
            : m_abbrev(abbrev)
            , m_englishName(QString::fromUtf8(englishName))
            , m_translatedName(translatedName)
        {}

        Language(QString const & abbrev,
                 QString const & englishName,
                 QString const & translatedName)
            : m_abbrev(abbrev)
            , m_englishName(englishName)
            , m_translatedName(translatedName)
        {}

        Language(char const * abbrev,
                 char const * englishName,
                 QString const & translatedName,
                 QStringList const & altAbbrevs)
            : m_abbrev(abbrev)
            , m_englishName(QString::fromUtf8(englishName))
            , m_translatedName(translatedName)
            , m_altAbbrevs(altAbbrevs)
        {}

    private: /* Fields: */

        QString const m_abbrev;
        QString const m_englishName;
        QString const m_translatedName;
        QStringList const m_altAbbrevs;

    }; /* class Language */

    using LanguageList = QList<Language *>;
    using LangMap = QHash<QString, Language const *>;


    /** \returns the singleton instance, creating it if one does not exist. */
    static CLanguageMgr * instance();

    /** Destroys the singleton instance, if one exists. */
    static void destroyInstance();

    CLanguageMgr();

    virtual ~CLanguageMgr();

    /** \returns a map which contains all known (standard) languages. */
    CLanguageMgr::LangMap const * languages() const { return &m_langMap; }

    /** \returns a map of available languages (those with modules present). */
    CLanguageMgr::LangMap const & availableLanguages();

    /**
       \param abbrev the language abbreviation
       \returns a pointer to the language of the given abbreviation, if present.
    */
    CLanguageMgr::Language const *
    languageForAbbrev(QString const & abbrev) const;

    /**
       \param language the translated language name
       \returns a pointer to the language of the given translated language name,
                if present.
    */
    CLanguageMgr::Language const *
    languageForTranslatedName(QString const & language) const;

    /** \returns a non-null pointer to the default language. */
    CLanguageMgr::Language const * defaultLanguage() const
    { return &m_defaultLanguage; }

private: /* Methods: */

    void init();

    QStringList makeStringList(QString const & abbrevs) {
        #if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
        return abbrevs.split(";", QString::KeepEmptyParts, Qt::CaseSensitive);
        #else
        return abbrevs.split(";", Qt::KeepEmptyParts, Qt::CaseSensitive );
        #endif
    }

private: /* Fields: */

    Language m_defaultLanguage;
    mutable LanguageList m_langList;
    mutable LangMap m_langMap;
    mutable LanguageList m_cleanupLangPtrs;

    struct ModuleCache {
        int moduleCount;
        LangMap availableLanguages;
    } m_availableModulesCache;

    static CLanguageMgr * m_instance;

}; /* class CLanguageMgr */
