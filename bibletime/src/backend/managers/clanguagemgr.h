/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CLANGUAGEMGR_H
#define CLANGUAGEMGR_H

#include <QHash>
#include <QList>
#include <QString>
#include <QStringList>


/** Manages the languages of BibleTime and provides functions to work with them.
  * @author The BibleTime team
  */
class CLanguageMgr {

    public:
        /** Language container.
         * This class (Language) contains the information about the chosen language.
         */
        class Language {
            public:
                /** Default constructor of a language object.
                 * Uses the abbreviation parameter to lookup the
                 * language name and to be able to return the name, flag etc.
                 * Possible values for abbrev are de, en, fr, it etc.
                 */
                Language();
                /** Copy constructor.
                 */
                Language(const Language&);
                /** Constructor which takes all necessary data.
                */
                Language(const QString& abbrev, const QString& englishName, const QString& translatedName, const QStringList& altAbbrevs = QStringList());
                /** Destructor.
                 */
                ~Language();
                /** Returns the abbreviation.
                 * @return The abbreviation of the chosen language.
                 */
                inline const QString& abbrev() const {
                    if (m_abbrev.isEmpty() && m_altAbbrevs.count()) { //no standard abbrev but alternative ones
                        return m_altAbbrevs.first();
                    }
                    return m_abbrev;
                }
                /** Returns the translated name.
                 * @return The translated name of the language.
                 */
                inline const QString& translatedName() const {
                    return m_translatedName;
                }
                /** The english name of the language.
                 * @return The english name of the chosen language.
                 */
                inline const QString& name() const {
                    return m_englishName;
                }
                /** The alternative abbreviations which are avalable for this language.
                 * @return The List of alternate abbreviations
                 */
                inline const QStringList alternativeAbbrevs() const {
                    return m_altAbbrevs;
                }
                /**
                 * Returns true if this language object is valid, i.e. has an abbrev and name.
                 * @return True if the data is valid for this language.
                 */
                inline bool isValid() const {
                    return (!abbrev().isEmpty() && !name().isEmpty());
                }

            private:
                QString m_abbrev;
                QString m_englishName;
                QString m_translatedName;
                QStringList m_altAbbrevs;
        };

        typedef QList<Language*> LanguageList;
        typedef QHash<QString, const Language*> LangMap;
        typedef QHash<QString, const Language*>::const_iterator LangMapIterator;

        /** Constructor.
        */
        CLanguageMgr();
        /** Destructor
        */
        virtual ~CLanguageMgr();
        /**
        * Returns the standard languages available as standard. Does nothing for Sword.
        * @return A LangMap map which contains all known languages
        */
        inline const CLanguageMgr::LangMap* languages() const {
            return &m_langMap;
        }
        /**
        * Returns the languages which are available. The languages cover all available modules, but nothing more.
        * @return A map of all languages with modules available for them
        */
        const CLanguageMgr::LangMap& availableLanguages();
        /** Language for abbreviation.
        * @param abbrev The language abbreviation
        * @return Pointer to a language for the given string abbreviation.
        */
        const CLanguageMgr::Language* languageForAbbrev( const QString& abbrev ) const;
        /** Language for english name.
        * @param abbrev The english language name.
        * @return Pointer to a language for the given name
        */
        const CLanguageMgr::Language* languageForName( const QString& language ) const;
        /** Language for translated language name.
        * @param abbrev The translated language name
        * @return Pointer to a language for the given translated language name
        */
        const CLanguageMgr::Language* languageForTranslatedName( const QString& language ) const;
        /** Default language so we don't return NULL pointers.
        * @return Pointer to the default language
        */
        inline const CLanguageMgr::Language* defaultLanguage() const {
            return &m_defaultLanguage;
        }

    private:
        void init();
        inline const QStringList makeStringList(const QString& abbrevs) {
            return abbrevs.split( ";", QString::KeepEmptyParts, Qt::CaseSensitive );
        }

        Language m_defaultLanguage;
        mutable LanguageList m_langList;
        mutable LangMap m_langMap;
        mutable LanguageList m_cleanupLangPtrs;

        struct ModuleCache {
            unsigned int moduleCount;
            LangMap availableLanguages;
        }
        m_availableModulesCache;
};

#endif

