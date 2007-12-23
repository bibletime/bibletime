/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "cmodulechooser.h"

#include "backend/keys/cswordkey.h"
#include "backend/keys/cswordversekey.h"
#include "frontend/cbtconfig.h"
#include "util/cresmgr.h"
#include "util/ctoolclass.h"
#include "util/directoryutil.h"

//Qt includes
#include <q3hbox.h>
#include <q3vbox.h>
#include <q3ptrlist.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qmap.h>
#include <qlineedit.h>
#include <q3textedit.h>
#include <qlabel.h>
#include <qsizepolicy.h>
#include <qpushbutton.h>
#include <q3header.h>
#include <qregexp.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3Frame>

//KDE includes
#include <kfiledialog.h>
#include <klocale.h>

namespace Search {
	namespace Options {
	
/****************************/
/****************************/
/****************************/

CModuleChooser::ModuleCheckBoxItem::ModuleCheckBoxItem(Q3ListViewItem* item, CSwordModuleInfo* module) : Q3CheckListItem(item, QString::null, Q3CheckListItem::CheckBox) {
	m_module = module;
	setText(0,m_module->name());
};

CModuleChooser::ModuleCheckBoxItem::~ModuleCheckBoxItem() {}
;

/** Returns the used module. */
CSwordModuleInfo* const CModuleChooser::ModuleCheckBoxItem::module() const {
	return m_module;
}


/****************************/
/****************************/
/****************************/

CModuleChooser::CModuleChooser(QWidget* parent) : KListView(parent) {
	initView();
	initTree();
}

CModuleChooser::~CModuleChooser() {}

void CModuleChooser::show() {
	KListView::show();

	//open module items
	Q3ListViewItemIterator it( this );
	for ( ; it.current(); ++it ) {
		if ( ModuleCheckBoxItem* i = dynamic_cast<ModuleCheckBoxItem*>(it.current()) ) {
			if (i->isOn()) {
				ensureItemVisible(i);
			};
		}
	}
}

/** Initializes this widget and the childs of it. */
void CModuleChooser::initView() {
	addColumn( i18n("Work") );
	setRootIsDecorated(true);
	//  header()->hide();
	setFullWidth(true);
}

/** Initializes the tree of this widget. */
void CModuleChooser::initTree() {
	ListCSwordModuleInfo mods = backend()->moduleList();
	/**
	* The next steps:
	* 1. Sort by type
	* 2. Sort the modules of this type by their language
	* 3. Create the subfolders for this
	*/

	QMap<CSwordModuleInfo::ModuleType, QString> typenameMap;
	typenameMap.insert(CSwordModuleInfo::Bible, i18n("Bibles"));
	typenameMap.insert(CSwordModuleInfo::Commentary, i18n("Commentaries"));
	typenameMap.insert(CSwordModuleInfo::Lexicon, i18n("Lexicons"));
	typenameMap.insert(CSwordModuleInfo::GenericBook, i18n("Books"));

	int type = CSwordModuleInfo::Bible;
	bool ok = true;
	bool addedDevotionals = false;
	bool addedGlossaries = false;
	bool addedLexs = false;
	bool incType = false;

	while (ok) {
		ListCSwordModuleInfo modsForType;
		QString typeFolderCaption = QString::null;
		incType = false;
		if (static_cast<CSwordModuleInfo::ModuleType>(type) == CSwordModuleInfo::Lexicon) {
			if (!addedLexs) {
				//         for (mods.first(); mods.current(); mods.next()) {
				ListCSwordModuleInfo::iterator end_it = mods.end();
				for (ListCSwordModuleInfo::iterator it(mods.begin()); it != end_it; ++it) {
					if (((*it)->type() == CSwordModuleInfo::Lexicon)
							&& ((*it)->category() != CSwordModuleInfo::DailyDevotional)
							&& ((*it)->category() != CSwordModuleInfo::Glossary)
					   ) {
						modsForType.append( *it );
					};
				};

				addedLexs = true;
				typeFolderCaption = QString::null;
			}
			else if (!addedDevotionals) {
				//         for (mods.first(); mods.current(); mods.next()) {
				ListCSwordModuleInfo::iterator end_it = mods.end();
				for (ListCSwordModuleInfo::iterator it(mods.begin()); it != end_it; ++it) {
					if ((*it)->category() == CSwordModuleInfo::DailyDevotional) {
						modsForType.append(*it);
					};
				};
				addedDevotionals = true;
				typeFolderCaption = i18n("Daily Devotionals");
			}
			else if (!addedGlossaries) {
				//         for (mods.first(); mods.current(); mods.next()) {
				ListCSwordModuleInfo::iterator end_it = mods.end();
				for (ListCSwordModuleInfo::iterator it(mods.begin()); it != end_it; ++it) {
					if ((*it)->category() == CSwordModuleInfo::Glossary) {
						modsForType.append(*it);
					};
				};
				addedGlossaries = true;
				typeFolderCaption = i18n("Glossaries");
			};

			if (addedLexs && addedDevotionals && addedGlossaries)
				incType = true;
		}
		else if (type == CSwordModuleInfo::Bible || type == CSwordModuleInfo::Commentary || type == CSwordModuleInfo::GenericBook) {
			//       for (mods.first(); mods.current(); mods.next()) {
			ListCSwordModuleInfo::iterator end_it = mods.end();
			for (ListCSwordModuleInfo::iterator it(mods.begin()); it != end_it; ++it) {
				if ((*it)->type() == type) {
					modsForType.append(*it);
				};
			};
			incType = true;
		}
		else
			ok = false;

		if (typeFolderCaption.isEmpty()) {
			typeFolderCaption = typenameMap[static_cast<CSwordModuleInfo::ModuleType>(type)];
		}

		//get the available languages of the selected modules
		QStringList langs;
		//     for (modsForType.first(); modsForType.current(); modsForType.next()) {
		ListCSwordModuleInfo::iterator end_it = modsForType.end();
		for (ListCSwordModuleInfo::iterator it(modsForType.begin()); it != end_it; ++it) {
			if ( !langs.contains(QString( (*it)->module()->Lang() ))) {
				langs.append( (*it)->module()->Lang() );
			}
		};
		langs.sort();

		//go through the list of languages and create subfolders for each language and the modules of the language
		Q3ListViewItem* typeFolder = 0;
		if (modsForType.count()) {
			typeFolder = new Q3ListViewItem(this, typeFolder, typeFolderCaption);
		}
		else {
			if (incType) {
				type++;
			}
			continue;
		};


		QString language = QString::null;
		CLanguageMgr* langMgr = languageMgr();
		for ( QStringList::Iterator it = langs.begin(); it != langs.end(); ++it ) {
			language = langMgr->languageForAbbrev(*it)->translatedName();
			if (language.isEmpty()) {
				language = (*it);
			}

			Q3ListViewItem* langFolder = new Q3ListViewItem(typeFolder,language);
			langFolder->setPixmap(0, util::filesystem::DirectoryUtil::getIcon(CResMgr::mainIndex::closedFolder::icon));

			//create the module items of this lang folder
			//       for (modsForType.first(); modsForType.current(); modsForType.next()) {
			ListCSwordModuleInfo::iterator end_modItr = modsForType.end();
			for (ListCSwordModuleInfo::iterator mod_Itr(modsForType.begin()); mod_Itr != end_modItr; ++mod_Itr) {
				if (QString( (*mod_Itr)->module()->Lang() ) == (*it) ) { //found correct language
					ModuleCheckBoxItem* i = new ModuleCheckBoxItem(langFolder, *mod_Itr);
					i->setPixmap(0, CToolClass::getIconForModule(*mod_Itr));
				};
			};
		};
		typeFolder->setPixmap(0, util::filesystem::DirectoryUtil::getIcon(CResMgr::mainIndex::closedFolder::icon));

		if (incType) {
			++type;
		}
	};
}

/** Returns a list of selected modules. */
ListCSwordModuleInfo CModuleChooser::modules() {
	ListCSwordModuleInfo mods;
	Q3ListViewItemIterator it( this );
	for ( ; it.current(); ++it ) {
		if ( ModuleCheckBoxItem* i = dynamic_cast<ModuleCheckBoxItem*>(it.current()) ) {
			//add the module if the box is checked
			if (i->isOn()) {
				mods.append(i->module());
			};
		};
	};

	return mods;
}

/** Sets the list of modules and updates the state of the checkbox items. */
void CModuleChooser::setModules( ListCSwordModuleInfo modules ) {
	//  qWarning("CModuleChooser::setModules( ListCSwordModuleInfo modules )");
	Q3ListViewItemIterator it( this );
	for ( ; it.current(); ++it ) {
		if ( ModuleCheckBoxItem* i = dynamic_cast<ModuleCheckBoxItem*>(it.current()) ) {
			i->setOn(modules.contains(i->module())); //set the status for the module checkbox item
			//      if (i->isOn()) { //if it's checked, show the item
			//        qWarning("show item!");
			//        ensureItemVisible(i);
			//      }
		}
	};
}

/****************************/

CModuleChooserDialog::CModuleChooserDialog( QWidget* parentDialog, ListCSwordModuleInfo modules ) :
KDialogBase(Plain, i18n("Choose work(s)"), Ok, Ok, parentDialog, "CModuleChooser", false, true) {
	initView();
	initConnections();

	m_moduleChooser->setModules(modules);
};

CModuleChooserDialog::~CModuleChooserDialog() {}
;

/** Initializes the view of this dialog */
void CModuleChooserDialog::initView() {
	setButtonOKText(i18n("Use chosen work(s)"));

	Q3Frame* page = plainPage();
	Q3HBoxLayout* layout = new Q3HBoxLayout(page);
	m_moduleChooser = new CModuleChooser(page);
	m_moduleChooser->setMinimumSize(320,400);
	layout->addWidget(m_moduleChooser);
}

/** Initializes the connections of this dialog. */
void CModuleChooserDialog::initConnections() {}

/** Reimplementation to handle the modules. */
void CModuleChooserDialog::slotOk() {
	emit modulesChanged( m_moduleChooser->modules() );

	KDialogBase::slotOk();
}

	} //end of namespace Search::Options
} //end of namespace Search
