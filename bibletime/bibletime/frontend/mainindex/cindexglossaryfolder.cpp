//
// C++ Implementation: cindexglossaryfolder
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
CGlossaryFolder::CGlossaryFolder(CMainIndex* mainIndex, const Type type, const QString& fromLanguage, const QString& toLanguage)
: CTreeFolder(mainIndex, type, fromLanguage) {
	m_fromLanguage = fromLanguage;
	m_toLanguage = toLanguage;
}

CGlossaryFolder::CGlossaryFolder(CFolderBase* item, const Type type, const QString& fromLanguage, const QString& toLanguage)
: CTreeFolder(item, type, fromLanguage) {
	m_fromLanguage = fromLanguage;
	m_toLanguage = toLanguage;
}

CGlossaryFolder::~CGlossaryFolder() {}

void CGlossaryFolder::initTree() {
	if (type() == Unknown)
		return;

	//get all modules by using the lexicon type
	ListCSwordModuleInfo allModules =CPointers::backend()->moduleList();
	ListCSwordModuleInfo usedModules;
	//   for (CSwordModuleInfo* m = allModules.first(); m; m = allModules.next()) {

	ListCSwordModuleInfo::iterator end_it = allModules.end();
	for (ListCSwordModuleInfo::iterator it(allModules.begin()); it != end_it; ++it) {
		if ((*it)->type() == CSwordModuleInfo::Lexicon) { //found a module, check if the type is correct (devotional etc.)
			if ((type() == GlossaryModuleFolder) && ((*it)->category() != CSwordModuleInfo::Glossary)) { //not a glossary
				continue;
			}
			//found a glossary
			//ToDo: this is ugly code
			if (language() == QString::fromLatin1("*")
					|| (language() != QString::fromLatin1("*")
						&& (*it)->config(CSwordModuleInfo::GlossaryFrom) == fromLanguage()
						&& (*it)->config(CSwordModuleInfo::GlossaryTo) == toLanguage()
					   )
			   ) { //right type and language!
				usedModules.append(*it);
			}
		}
	}

	//we have now all modules we want to have
	if (language() == QString::fromLatin1("*")) { //create subfolders for each language
		typedef std::pair<QString, QString> LanguagePair;
		typedef Q3ValueList<LanguagePair> LanguagePairList;

		LanguagePairList usedLangs;
		//     for (CSwordModuleInfo* m = usedModules.first(); m; m = usedModules.next()) {
		ListCSwordModuleInfo::iterator end_it = usedModules.end();
		for (ListCSwordModuleInfo::iterator it(usedModules.begin()); it != end_it; ++it) {
			LanguagePair langPair(
				(*it)->config(CSwordModuleInfo::GlossaryFrom),
				(*it)->config(CSwordModuleInfo::GlossaryTo)
			);

			if (!usedLangs.contains(langPair)) {
				usedLangs.append(langPair);
			}
		}

		LanguagePairList::iterator lang_it;
		for (lang_it = usedLangs.begin(); lang_it != usedLangs.end(); ++lang_it) {
			addGroup(type(), (*lang_it).first, (*lang_it).second);
		}
	}
	else if (usedModules.count() > 0) { //create subitems with the given type and languages
		//     for (CSwordModuleInfo* m = usedModules.first(); m; m = usedModules.next()) {
		ListCSwordModuleInfo::iterator end_it = usedModules.end();
		for (ListCSwordModuleInfo::iterator it(usedModules.begin()); it != end_it; ++it) {
			addModule(*it);
		}
	}

	sortChildItems(0,true);
}

void CGlossaryFolder::init() {
	if (language() == "*") {
		setText(0,i18n("Glossaries"));
	}
	else {
		const CLanguageMgr::Language* const fromLang = CPointers::languageMgr()->languageForAbbrev( m_fromLanguage );
		const CLanguageMgr::Language* const toLang = CPointers::languageMgr()->languageForAbbrev( m_toLanguage );

		QString fromLangString  = fromLang->translatedName();
		QString toLangString    = toLang->translatedName();

		if (fromLangString.isEmpty()) { //use abbrev!
			fromLangString = m_fromLanguage;
		};
		if (toLangString.isEmpty()) { //use abbrev!
			toLangString = m_toLanguage;
		};

		setText(0, fromLangString + " - " + toLangString );
	}
	initTree();
	update();
}

void CIndexGlossaryFolder::addGroup(const Type /*type*/, const QString& /*fromLanguage*/)
{}

/** Returns the language this glossary folder maps from. */
const QString& CGlossaryFolder::fromLanguage() const {
	return m_fromLanguage;
}

/** Returns the language this glossary folder maps to. */
const QString& CGlossaryFolder::toLanguage() const {
	return m_toLanguage;
}

void CGlossaryFolder::addGroup(const Type type, const QString& fromLanguage, const QString& toLanguage) {
	CTreeFolder* i = new CGlossaryFolder(this, type, fromLanguage, toLanguage);
	i->init();
	if (!i->childCount()) {
		delete i;
	}
}

