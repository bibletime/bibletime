/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "clanguagemgr.h"

#include "cswordmoduleinfo.h"
#include "cswordbackend.h"

#include "util/cpointers.h"


//Qt includes
#include <qptrlist.h>

//KDE includes
#include <klocale.h>

//initialize static language list
CLanguageMgr::LanguageList CLanguageMgr::m_langList;
CLanguageMgr::LanguageList CLanguageMgr::m_cleanupLangPtrs;

CLanguageMgr::Language::Language() : m_altAbbrevs(0) {
	m_abbrev = QString::null;
	m_englishName = QString::null;
	m_translatedName = QString::null;
};

CLanguageMgr::Language::Language(const Language& l) {
	m_abbrev = l.m_abbrev;
	m_englishName = l.m_englishName;
	m_translatedName = l.m_translatedName;

	if (l.m_altAbbrevs) { //copy alternative abbrevs in a new list
		m_altAbbrevs = new QStringList(*l.m_altAbbrevs);
		//*m_altAbbrevs = *l.m_altAbbrevs;
	}
	else {
		m_altAbbrevs = 0;
	}
}

CLanguageMgr::Language::Language( const QString& abbrev, const QString& name, const QString& translatedName, const QStringList& altAbbrevs ) : m_altAbbrevs(0) {
	m_abbrev = abbrev;
	m_englishName = name;
	m_translatedName = translatedName;

	if (altAbbrevs.count() > 0) {
		m_altAbbrevs = new QStringList();
		*m_altAbbrevs = altAbbrevs;
	}
};

CLanguageMgr::Language::~Language() {
	delete m_altAbbrevs;
};


/****************************************************/
/******************** CLanguageMgr ******************/
/****************************************************/
CLanguageMgr::CLanguageMgr() : m_langMap(0) {
	m_availableModulesCache.moduleCount = 0;

	init();
}

CLanguageMgr::~CLanguageMgr() {
	m_cleanupLangPtrs.setAutoDelete(true);
	m_cleanupLangPtrs.clear();

	m_langList.setAutoDelete(true);
	m_langList.clear();
}

const CLanguageMgr::LangMap& CLanguageMgr::availableLanguages() {
	ListCSwordModuleInfo mods = CPointers::backend()->moduleList();

	if ( m_availableModulesCache.moduleCount != mods.count() ) { //we have to refill the cached map
		m_availableModulesCache.availableLanguages.clear();
		m_availableModulesCache.moduleCount = mods.count();

		//collect the languages abbrevs of all modules
		QStrList abbrevs;
		char *abbrev;

		ListCSwordModuleInfo::iterator end_it = mods.end();

		for (ListCSwordModuleInfo::iterator it(mods.begin()); it != end_it; ++it) {
			//   for (CSwordModuleInfo* m = mods.first(); m; m = mods.next()) {
			abbrev = (*it)->module()->Lang();

			if (abbrev && !abbrevs.contains(abbrev)) {
				abbrevs.append( abbrev );
			}
		};

		//now create a map of available langs
		for ( abbrev = abbrevs.first(); abbrev; abbrev = abbrevs.next() ) {
			const Language* const lang = languageForAbbrev(abbrev);

			if (lang->isValid()) {
				m_availableModulesCache.availableLanguages.insert( abbrev, lang );
			}
			else { //invalid lang used by a modules, create a new language using the abbrev
				Language* newLang = new Language(abbrev, abbrev, abbrev);
				m_cleanupLangPtrs.append(newLang);

				m_availableModulesCache.availableLanguages.insert( abbrev, newLang );
			}
		};
	}

	return m_availableModulesCache.availableLanguages;
};

const CLanguageMgr::Language* const CLanguageMgr::languageForAbbrev( const QString& abbrev ) const {
	Language* lang = m_langMap.find(abbrev);

	if (lang) {
		return lang;
	}

	//try to search in the alternative abbrevs
	//const LangMapIterator end = m_langMap.constEnd();
	for ( LangMapIterator it( m_langMap ); it.current(); ++it ) {
		if (it.current()->alternativeAbbrevs() && it.current()->alternativeAbbrevs()->contains(abbrev)) {
			return it.current();
		}
	}

	// Invalid lang used by a modules, create a new language using the abbrev
	Language* newLang = new Language(abbrev, abbrev, abbrev); //return a language which holds the valid abbrev
	m_cleanupLangPtrs.append(newLang);

	return newLang;
};

const CLanguageMgr::Language* const CLanguageMgr::languageForName( const QString& name ) const {
	for ( LangMapIterator it( m_langMap ); it.current(); ++it ) {
		if (it.current()->name() == name) {
			return it.current();
		}
	}

	return &m_defaultLanguage;//invalid language
};

const CLanguageMgr::Language* const CLanguageMgr::languageForTranslatedName( const QString& name ) const {
	for ( LangMapIterator it( m_langMap ); it.current(); ++it ) {
	if (it.current()->translatedName() == name) {
			return it.current();
		}
	}

	return &m_defaultLanguage; //invalid language
};

void CLanguageMgr::init() {
	//if we've already inserted all items we do not proceed

	if (m_langMap.count() > 0) {
		return;
	}

	/*
	* Chris explained in an eMail how language codes are build:
	 
	Preference order for locale codes are:
	 
	ISO 639-1
	ISO 639-2
	Ethnologue (http://www.ethnologue.org/)
	 
	We intend to always follow OSIS locale conventions, which state that
	Ethnologue codes will be encoded in the format x-E-??? where ???
	represents the 3-letter Ethnologue code in capital letters (though
	capitalization really doesn't matter here).
	 
	Some older modules maintain a former format for Ethnolgoue codes of
	xx-???, including the AleWiesler module.
	 
	--Chris
	 
	*/

	//  m_langList.append( new Language("aa"  , "Afar"        , i18n("Afar")) );
	//  m_langList.append( new Language("ab"  , "Abkhazian"   , i18n("Abkhazian")) );
	//  m_langList.append( new Language("ae"  , "Avestan"     , i18n("Avestan")) );
	m_langList.append( new Language("af"  , "Afrikaans"   , i18n("Afrikaans")) );

	//  m_langList.append( new Language("am"  , "Amharic"     , i18n("Amharic")) );
	m_langList.append( new Language("ang", "English, Old (ca.450-1100)", i18n("English, Old (ca.450-1100)")) );

	m_langList.append( new Language("ar"  , "Arabic"      , i18n("Arabic")) );

	//  m_langList.append( new Language("as"  , "Assamese"    , i18n("Assamese")) );
	m_langList.append( new Language("az"  , "Azerbaijani" , i18n("Azerbaijani")) );

	//  m_langList.append( new Language("ba"  , "Bashkir"     , i18n("Bashkir")) );
	m_langList.append( new Language("be"  , "Belarusian"  , i18n("Belarusian")) );

	m_langList.append( new Language("bg"  , "Bulgarian"   , i18n("Bulgarian")) );

	//  m_langList.append( new Language("bh"  , "Bihari"      , i18n("Bihari")) );
	//  m_langList.append( new Language("bi"  , "Bislama"     , i18n("Bislama")) );
	//  m_langList.append( new Language("bn"  , "Bengali"     , i18n("Bengali")) );
	//  m_langList.append( new Language("bo"  , "Tibetan"     , i18n("Tibetan")) );
	m_langList.append( new Language("br"  , "Breton"      , i18n("Breton")) );

	m_langList.append( new Language("bs"  , "Bosnian"     , i18n("Bosnian")) );

	m_langList.append( new Language("ca"  , "Catalan"     , i18n("Catalan")) );

	//  m_langList.append( new Language("ce"  , "Chechen"     , i18n("Chechen")) );
	m_langList.append( new Language("ceb" , "Cebuano"     , i18n("Cebuano")) );

	m_langList.append( new Language("ch"  , "Chamorro"    , i18n("Chamorro")) );

	//  m_langList.append( new Language("co"  , "Corsican"    , i18n("Corsican")) );
	m_langList.append( new Language("cop" , "Coptic"         , i18n("Coptic")) );

	m_langList.append( new Language("cs"  , "Czech"       , i18n("Czech")) );

	m_langList.append( new Language("cu"  , "Church Slavic" , i18n("Church Slavic")) );

	//  m_langList.append( new Language("cv"  , "Chuvash"     , i18n("Chuvash")) );
	m_langList.append( new Language("cy"  , "Welsh"       , i18n("Welsh")) );

	m_langList.append( new Language("da"  , "Danish"      , i18n("Danish")) );

	m_langList.append( new Language("de"  , "German"      , i18n("German")) );

	//  m_langList.append( new Language("dz"  , "Dzongkha"    , i18n("Dzongkha")) );

	m_langList.append( new Language("el"  , "Greek, Modern (1453-)" , i18n("Greek, Modern (1453-)"), makeStringList("gre;ell")) );

	m_langList.append( new Language("en"  , "English"     , i18n("English")) );

	m_langList.append( new Language("en_US","American English"     , i18n("American English")) );

	m_langList.append( new Language("enm" , "English, Middle (1100-1500)", i18n("English, Middle (1100-1500)")) );

	m_langList.append( new Language("eo"  , "Esperanto"   , i18n("Esperanto")) );

	m_langList.append( new Language("es"  , "Spanish"     , i18n("Spanish")) );

	m_langList.append( new Language("et"  , "Estonian"    , i18n("Estonian")) );

	m_langList.append( new Language("eu"  , "Basque"      , i18n("Basque")) );

	//  m_langList.append( new Language("fa"  , "Persian"     , i18n("Persian")) );
	m_langList.append( new Language("fi"  , "Finnish"     , i18n("Finnish")) );

	//  m_langList.append( new Language("fj"  , "Fijian"      , i18n("Fijian")) );
	//  m_langList.append( new Language("fo"  , "Faroese"     , i18n("Faroese")) );
	m_langList.append( new Language("fr"  , "French"      , i18n("French")) );

	m_langList.append( new Language("fy"  , "Frisian"     , i18n("Frisian")) );

	m_langList.append( new Language("ga"  , "Irish"       , i18n("Irish")) );

	m_langList.append( new Language("gd"  , "Gaelic (Scots)", i18n("Gaelic (Scots)")) );

	//  m_langList.append( new Language("gl"  , "Gallegan"    , i18n("Gallegan")) );
	//  m_langList.append( new Language("gn"  , "Guarani"     , i18n("Guarani")) );
	//  m_langList.append( new Language("gn"  , "Gujarati"    , i18n("Gujarati")) );
	m_langList.append( new Language("got"  , "Gothic"      , i18n("Gothic")) );

	m_langList.append( new Language("gv"   , "Manx"        , i18n("Manx")) );

	m_langList.append( new Language("grc"  , "Greek, Ancient (to 1453)" , i18n("Greek, Ancient (to 1453)")) );

	m_langList.append( new Language("he"  , "Hebrew"      , i18n("Hebrew")) );

	m_langList.append( new Language("hau" , "Hausa"      , i18n("Hausa")) );

	m_langList.append( new Language("haw" , "Hawaiian"    , i18n("Hawaiian")) );

	//  m_langList.append( new Language("hi"  , "Hindi"       , i18n("Hindi")) );
	//  m_langList.append( new Language("ho"  , "Hiri Motu"   , i18n("Hiri Motu")) );
	m_langList.append( new Language("hr"  , "Croatian"    , i18n("Croatian")) );

	m_langList.append( new Language("hu"  , "Hungarian"   , i18n("Hungarian")) );

	m_langList.append( new Language("hy"  , "Armenian"    , i18n("Armenian")) );

	//  m_langList.append( new Language("hz"  , "Herero"      , i18n("Herero")) );

	//  m_langList.append( new Language("ia"  , "Interlingua" , i18n("Interlingua")) );
	m_langList.append( new Language("id"  , "Indonesian"  , i18n("Indonesian")) );

	//  m_langList.append( new Language("ie"  , "Interlingue" , i18n("Interlingue")) );
	//  m_langList.append( new Language("ik"  , "Inupiaq"     , i18n("Inupiaq")) );
	m_langList.append( new Language("is"  , "Icelandic"   , i18n("Icelandic")) );

	m_langList.append( new Language("it"  , "Italian"     , i18n("Italian")) );

	//  m_langList.append( new Language("iu"  , "Inuktitut"   , i18n("Inuktitut")) );

	m_langList.append( new Language("ja"  , "Japanese"    , i18n("Japanese")) );

	m_langList.append( new Language("ka"   , "Georgian"  , i18n("Georgian")) );

	//  m_langList.append( new Language("x-E-KAB"   , "Kabyle"    , i18n("Kabyle")) );
	//  m_langList.append( new Language("ki"  , "Kikuyu"      , i18n("Kikuyu")) );
	//  m_langList.append( new Language("kj"  , "Kuanyama"    , i18n("Kuanyama")) );
	//  m_langList.append( new Language("kk"  , "Kazakh"      , i18n("Kazakh")) );
	//  m_langList.append( new Language("kl"  , "Kalaallisut" , i18n("Kalaallisut")) );
	//  m_langList.append( new Language("km"  , "Khmer"       , i18n("Khmer")) );
	//  m_langList.append( new Language("kn"  , "Kannada"     , i18n("Kannada")) );
	m_langList.append( new Language("ko"  , "Korean"      , i18n("Korean")) );

	//  m_langList.append( new Language("ks"  , "Kashmiri"    , i18n("Kashmiri")) );
	m_langList.append( new Language("ku"  , "Kurdish"     , i18n("Kurdish")) );

	//  m_langList.append( new Language("kv"  , "Komi"        , i18n("Komi")) );
	//  m_langList.append( new Language("kw"  , "Cornish"     , i18n("Cornish")) );
	m_langList.append( new Language("ky"  , "Kirghiz"     , i18n("Kirghiz")) );

	m_langList.append( new Language("la"  , "Latin"       , i18n("Latin")) );

	//  m_langList.append( new Language("lb"  , "Letzeburgesch" , i18n("Letzeburgesch")) );
	//  m_langList.append( new Language("ln"  , "Lingala"     , i18n("Lingala")) );
	//  m_langList.append( new Language("lo"  , "Lao"         , i18n("Lao")) );
	m_langList.append( new Language("lt"  , "Lithuanian"  , i18n("Lithuanian")) );

	m_langList.append( new Language("lv"  , "Latvian"     , i18n("Latvian")) );

	//  m_langList.append( new Language("mg"  , "Malagasy"    , i18n("Malagasy")) );
	//  m_langList.append( new Language("mh"  , "Marshall"    , i18n("Marshall")) );
	m_langList.append( new Language("mi"  , "Maori"       , i18n("Maori")) );

	m_langList.append( new Language("mk"  , "Macedonian"  , i18n("Macedonian")) );

	//  m_langList.append( new Language("ml"  , "Malayalam"   , i18n("Malayalam")) );
	//  m_langList.append( new Language("mn"  , "Mongolian"   , i18n("Mongolian")) );
	//  m_langList.append( new Language("mo"  , "Moldavian"   , i18n("Moldavian")) );
	m_langList.append( new Language("mos"  , "More"   , i18n("More")) );

	//  m_langList.append( new Language("mr"  , "Marathi"     , i18n("Marathi")) );
	m_langList.append( new Language("ms"  , "Malay"       , i18n("Malay")) );

	m_langList.append( new Language("mt"  , "Maltese"     , i18n("Maltese")) );

	//  m_langList.append( new Language("my"  , "Burmese"     , i18n("Burmese")) );

	//  m_langList.append( new Language("na"  , "Nauru"       , i18n("Nauru")) );
	//  m_langList.append( new Language("nb"  , "Norwegian Bokmål"       , i18n("Norwegian Bokmål")) );
	//  m_langList.append( new Language("nd"  , "Ndebele, North" , i18n("Ndebele, North")) );
	m_langList.append( new Language("nds"  , "Low German; Low Saxon" , i18n("Low German; Low Saxon")) );

	//  m_langList.append( new Language("ne"  , "Nepali"      , i18n("Nepali")) );
	//  m_langList.append( new Language("ng"  , "Ndonga"      , i18n("Ndonga")) );
	m_langList.append( new Language("nl"  , "Dutch"       , i18n("Dutch")) );

	//  m_langList.append( new Language("nn"  , "Norwegian Nynorsk"     , i18n("Norwegian Nynorsk")) );
	m_langList.append( new Language("no"  , "Norwegian"   , i18n("Norwegian")) );

	//  m_langList.append( new Language("nr"  , "Ndebele, South"     , i18n("Ndebele, South")) );
	//  m_langList.append( new Language("nv"  , "Navajo"      , i18n("Navajo")) );
	//  m_langList.append( new Language("ny"  , "Chichewa; Nyanja"      , i18n("Chichewa; Nyanja")) );

	//  m_langList.append( new Language("oc"  , "Occitan (post 1500); Provençal"     , i18n("Occitan (post 1500); Provençal")) );
	//  m_langList.append( new Language("om"  , "Oromo"       , i18n("Oromo")) );
	//  m_langList.append( new Language("or"  , "Oriya"       , i18n("Oriya")) );
	//  m_langList.append( new Language("os"  , "Ossetian; Ossetic"     , i18n("Ossetian; Ossetic")) );

	//  m_langList.append( new Language("pa"  , "Panjabi"     , i18n("Panjabi")) );
	m_langList.append( new Language("pap"  , "Papiamento"     , i18n("Papiamento")) );

	//  m_langList.append( new Language("pi"  , "Pali"        , i18n("Pali")) );
	m_langList.append( new Language("pl"  , "Polish"      , i18n("Polish")) );

	//  m_langList.append( new Language("ps"  , "Pushto"      , i18n("Pushto")) );
	m_langList.append( new Language("pt"  , "Portuguese"  , i18n("Portuguese")) );

	m_langList.append( new Language("pt_BR"  , "Brasilian Portuguese"  , i18n("Brasilian Portuguese")) );//added by ourself

	//  m_langList.append( new Language("qu"  , "Quechua"     , i18n("Quechua")) );

	//  m_langList.append( new Language("rm"  , "Raeto-Romance"    , i18n("Raeto-Romance")) );
	//  m_langList.append( new Language("rn"  , "Rundi"       , i18n("Rundi")) );

	m_langList.append( new Language("ro"  , "Romanian"    , i18n("Romanian")) );

	m_langList.append( new Language("ru"  , "Russian"     , i18n("Russian")) );

	//  m_langList.append( new Language("rw"  , "Kinyarwanda" , i18n("Kinyarwanda")) );

	//  m_langList.append( new Language("sa"  , "Sanskrit"    , i18n("Sanskrit")) );
	//  m_langList.append( new Language("sc"  , "Sardinian"   , i18n("Sardinian")) );
	m_langList.append( new Language("sco"  , "Scots"   , i18n("Scots")) );

	//  m_langList.append( new Language("sd"  , "Sindhi"      , i18n("Sindhi")) );
	//  m_langList.append( new Language("se"  , "Northern Sami" , i18n("Northern Sami")) );
	//  m_langList.append( new Language("sg"  , "Sango"       , i18n("Sango")) );
	//  m_langList.append( new Language("si"  , "Sinhalese"   , i18n("Sinhalese")) );
	m_langList.append( new Language("sk"  , "Slovak"      , i18n("Slovak")) );

	m_langList.append( new Language("sl"  , "Slovenian"   , i18n("Slovenian")) );

	//  m_langList.append( new Language("sm"  , "Samoan"      , i18n("Samoan")) );
	//  m_langList.append( new Language("sn"  , "Shona"       , i18n("Shona")) );
	m_langList.append( new Language("so"  , "Somali"      , i18n("Somali")) );

	m_langList.append( new Language("sq"  , "Albanian"    , i18n("Albanian")) );

	//  m_langList.append( new Language("sr"  , "Serbian"     , i18n("Serbian")) );
	//  m_langList.append( new Language("ss"  , "Swati"       , i18n("Swati")) );
	//  m_langList.append( new Language("st"  , "Sotho, Southern"     , i18n("Sotho, Southern")) );
	//  m_langList.append( new Language("su"  , "Sundanese"   , i18n("Sundanese")) );
	m_langList.append( new Language("sv"  , "Swedish"     , i18n("Swedish")) );

	m_langList.append( new Language("sw"  , "Swahili"     , i18n("Swahili")) );

	m_langList.append( new Language("syr"  , "Syriac"     , i18n("Syriac")) );

	m_langList.append( new Language("ta"  , "Tamil"       , i18n("Tamil")) );

	//  m_langList.append( new Language("te"  , "Telugu"      , i18n("Telugu")) );
	//  m_langList.append( new Language("tg"  , "Tajik"       , i18n("Tajik")) );
	m_langList.append( new Language("th"  , "Thai"        , i18n("Thai")) );

	//  m_langList.append( new Language("tk"  , "Turkmen"     , i18n("Turkmen")) );
	m_langList.append( new Language("tl"  , "Tagalog"     , i18n("Tagalog")) );

	m_langList.append( new Language("tn"  , "Tswana"      , i18n("Tswana")) );

	m_langList.append( new Language("tr"  , "Turkish"     , i18n("Turkish")) );

	//  m_langList.append( new Language("ts"  , "Tsonga"      , i18n("Tsonga")) );
	//  m_langList.append( new Language("tt"  , "Tatar"       , i18n("Tatar")) );
	//  m_langList.append( new Language("tw"  , "Twi"         , i18n("Twi")) );
	m_langList.append( new Language("ty"  , "Tahitian"    , i18n("Tahitian")) );

	//
	//  m_langList.append( new Language("ug"  , "Uighur"      , i18n("Uighur")) );
	m_langList.append( new Language("uk"  , "Ukrainian"   , i18n("Ukrainian")) );

	//  m_langList.append( new Language("ur"  , "Urdu"        , i18n("Urdu")) );
	//  m_langList.append( new Language("uz"  , "Uzbek"       , i18n("Uzbek")) );

	m_langList.append( new Language("vi"  , "Vietnamese"  , i18n("Vietnamese")) );

	//  m_langList.append( new Language("vo"  , "Volapük"     , i18n("Volapük")) );
	//
	//  m_langList.append( new Language("wo"  , "Wolof"       , i18n("Wolof")) );

	m_langList.append( new Language("xh"  , "Xhosa"       , i18n("Xhosa")) );

	m_langList.append( new Language("x-E-BAR"  , "Bavarian"     , i18n("Bavarian")) );

	m_langList.append( new Language("x-E-DJE"  , "Zarma"     , i18n("Zarma")) );

	m_langList.append( new Language("x-E-GSW"  , "Alemannisch"  , i18n("Alemannisch")) );

	m_langList.append( new Language("x-E-HAT"  , "Haitian Creole French" , i18n("Haitian Creole French")) );

	m_langList.append( new Language("x-E-ITZ"  , QString::fromUtf8("Itzá")         , i18n("Itzá")) );

	m_langList.append( new Language("x-E-JIV"  , "Shuar"        , i18n("Shuar")) );

	m_langList.append( new Language("x-E-KEK"  , QString::fromUtf8("Kekchí")       , i18n("Kekchí")) );

	m_langList.append( new Language("x-E-KAB"  , "Kabyle"      , i18n("Kabyle")) );

	m_langList.append( new Language("x-E-LMO"  , "Lombard"      , i18n("Lombard")) );

	m_langList.append( new Language("x-E-MKJ"  , "Macedonian"   , i18n("Macedonian")) );

	m_langList.append( new Language("x-E-PDG"  , "Tok Pisin"    , i18n("Tok Pisin")) );

	m_langList.append( new Language("x-E-PPK"  , "Uma"          , i18n("Uma")) );

	m_langList.append( new Language("x-E-RMY"  , "Romani, Vlax" , i18n("Romani, Vlax")) );

	m_langList.append( new Language("x-E-SAJ"  , "Sango"        , i18n("Sango")) );

	m_langList.append( new Language("x-E-SRN"  , "Sranan"       , i18n("Sranan")) );

	//  m_langList.append( new Language("yi"  , "Yiddish"     , i18n("Yiddish")) );
	//
	//  m_langList.append( new Language("za"  , "Zhuang"      , i18n("Zhuang")) );
	m_langList.append( new Language("zh"  , "Chinese"     , i18n("Chinese")) );

	m_langList.append( new Language("zu"  , "Zulu"        , i18n("Zulu")) );

	for ( Language* lang = m_langList.first(); lang; lang = m_langList.next() ) {
		m_langMap.insert( lang->abbrev(), lang);
	};
};
