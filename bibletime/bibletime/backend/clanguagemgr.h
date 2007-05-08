/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CLANGUAGEMGR_H
#define CLANGUAGEMGR_H

//Qt includes
#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qmap.h>
#include <qdict.h>

/** Manages the anguages of BibleTime and provides functions to work with them.
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
		inline const QString& abbrev() const;
		/** Returns the translated name.
		 * @return The translated name of the language.
		 */
		inline const QString& translatedName() const;
		//always define inlines in the header file, or make them not inline.
		/** The english name of the language.
		 * @return The english name of the chosen language.
		 */
		inline const QString& name() const {
			return m_englishName;
		}

		/** The alternative abbreviations which are avalable for this language.
		 * @return A pointer to the list of alternate abbreviations
		 */
		inline const QStringList* const alternativeAbbrevs() const {
			return m_altAbbrevs;
		};

		/**
		  * Returns true if this language object is valid, i.e. has an abbrev and name.
		  * @return True if the data is valid for this language.
		  */
		inline const bool isValid() const;

	private:
		QString m_abbrev;
		QString m_englishName;
		QString m_translatedName;
		QStringList* m_altAbbrevs;
	};

	typedef QPtrList<CLanguageMgr::Language> LanguageList;

	typedef QDict<Language> LangMap;
	typedef QDictIterator<Language> LangMapIterator;

	/** Constructor.
	*/
	CLanguageMgr();
	/** Destructor
	*/
	virtual ~CLanguageMgr();
	/**
	* Returns the standard languages available as standard. Does nothing for Sword.
	* @return A QDict<Language> map which contains all known languages
	*/
	inline const CLanguageMgr::LangMap* const languages() const;
	/**
	* Returns the languages which are available. The languages cover all available modules, but nothing more.
	* @return A map of all languages with modules available for them
	*/
	const CLanguageMgr::LangMap& availableLanguages();
	/** Language for abbreviation.
	* @param abbrev The language abbreviation
	* @return Pointer to a language for the given string abbreviation.
	*/
	const CLanguageMgr::Language* const languageForAbbrev( const QString& abbrev ) const;
	/** Language for english name.
	* @param abbrev The english language name.
	* @return Pointer to a language for the given name
	*/
	const CLanguageMgr::Language* const languageForName( const QString& language ) const;
	/** Language for translated language name.
	* @param abbrev The translated language name
	* @return Pointer to a language for the given translated language name
	*/
	const CLanguageMgr::Language* const languageForTranslatedName( const QString& language ) const;
	/** Default language so we don't return NULL pointers.
	* @return Pointer to the default language
	*/
	inline const CLanguageMgr::Language* const defaultLanguage() const;

private:
	void init();
	inline const QStringList makeStringList(const QString& abbrevs) {
		return QStringList::split( ";", abbrevs, false );
	}

	mutable LangMap m_langMap;
	Language m_defaultLanguage;

	//typedef QPtrList<CLanguageMgr::Language> LanguageList;
	static LanguageList m_langList;
	static LanguageList m_cleanupLangPtrs;

	struct ModuleCache {
		unsigned int moduleCount;
		LangMap availableLanguages;
	}
	m_availableModulesCache;
};


/** Returns true if this language object is valid, i.e. has an abbrev and name. */
inline const bool CLanguageMgr::Language::isValid() const {
	return (!abbrev().isEmpty() && !name().isEmpty());
}

inline const QString& CLanguageMgr::Language::abbrev() const {
	if (m_altAbbrevs && m_abbrev.isEmpty() && m_altAbbrevs->count()) { //no standard abbrev but alternative ones
		return m_altAbbrevs->first();
	}

	return m_abbrev;
}

inline const QString& CLanguageMgr::Language::translatedName() const {
	return m_translatedName;
}

inline const CLanguageMgr::LangMap* const CLanguageMgr::languages() const {
	return &m_langMap;
}

inline const CLanguageMgr::Language* const CLanguageMgr::defaultLanguage() const {
	return &m_defaultLanguage;
}

#endif
