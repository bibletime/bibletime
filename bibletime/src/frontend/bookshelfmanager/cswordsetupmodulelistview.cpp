/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "cswordsetupmodulelistview.h"
#include "btinstallmgr.h"

#include "backend/managers/cswordbackend.h"
#include "backend/drivers/cswordmoduleinfo.h"

#include "util/cresmgr.h"
#include "util/cpointers.h"
#include "util/ctoolclass.h"
#include "util/directoryutil.h"

//QT includes
#include <QToolTip>

//KDE includes
#include <klocale.h>

using namespace sword;

namespace BookshelfManager {

/** Tooltip implementation for QListView widgets.
	* @short Tooltip for InstallationManager listviews
	*/
class ToolTip
{
public:
	/** Constructor which takes the listview to operate on.
		* @param listview We operate on this widget to request tooltips from it'd child items.
		*/
		//TODO: this class is probably obsolete, QToolTip is now static, check if it can be removed
	ToolTip(CSwordSetupModuleListView* listview) : 
		m_parent( listview ) {}

	/** Reimplementation of QToolTip::maybeTip. It's requested if a new tooltip may be displayed.
		* @short Display a tooltip if we're over an item
		*/
	virtual void maybeTip(const QPoint& pos) {
		QTreeWidgetItem* i = m_parent->itemAt(pos);
		Q_ASSERT(i);

		const QRect rect = m_parent->visualItemRect(i);
		if (m_parent->showTooltip(i, pos, 0)) {
			const QString tipText = m_parent->tooltip(i, 0);
			QToolTip::showText(rect.topLeft(), tipText);
		}
	}

protected:
	CSwordSetupModuleListView* m_parent;
}; //class ToolTip

/** Listview specially made for the installation manager.
* @short InstallationManager module listviews
*/
CSwordSetupModuleListView::CSwordSetupModuleListView(QWidget *parent, bool is_remote, sword::InstallSource* installSource) : QTreeWidget(parent), m_is_remote( is_remote ) {
	new BookshelfManager::ToolTip(this);
	m_backend = installSource ? BTInstallMgr::Tool::backend(installSource) : CPointers::backend();

	setColumnCount(3);
	setHeaderLabels(QStringList() << i18n("Name") << i18n("Status") << i18n("Installed version") << (m_is_remote ? i18n("Remote version") : i18n("Location")) );
	setRootIsDecorated(false);
	setColumnWidth(0, 200);
	//setTooltipColumn(0);

	init();
}

CSwordSetupModuleListView::~CSwordSetupModuleListView() {
	if (m_is_remote) delete m_backend;
}

void CSwordSetupModuleListView::init() {
	m_categoryBible = new QTreeWidgetItem(this);
	m_categoryBible->setText(0, i18n("Bibles"));
	m_categoryBible->setIcon( 0, util::filesystem::DirectoryUtil::getIcon(CResMgr::mainIndex::closedFolder::icon) );
	m_categoryBible->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsTristate);
	m_categoryBible->setCheckState(0, Qt::Unchecked);
	addTopLevelItem(m_categoryBible);
	
	m_categoryCommentary = new QTreeWidgetItem(this);
	m_categoryCommentary->setText(0, i18n("Commentaries"));
	m_categoryCommentary->setIcon( 0, util::filesystem::DirectoryUtil::getIcon(CResMgr::mainIndex::closedFolder::icon) );
	m_categoryCommentary->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsTristate);
	m_categoryCommentary->setCheckState(0, Qt::Unchecked);
	addTopLevelItem(m_categoryCommentary);
	
	m_categoryLexicon = new QTreeWidgetItem(this);
	m_categoryLexicon->setText(0, i18n("Lexicons"));
	m_categoryLexicon->setIcon( 0, util::filesystem::DirectoryUtil::getIcon(CResMgr::mainIndex::closedFolder::icon) );
	m_categoryLexicon->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsTristate);
	m_categoryLexicon->setCheckState(0, Qt::Unchecked);
	addTopLevelItem(m_categoryLexicon);
	
	m_categoryBook = new QTreeWidgetItem(this);
	m_categoryBook->setText(0, i18n("Books"));
	m_categoryBook->setIcon( 0, util::filesystem::DirectoryUtil::getIcon(CResMgr::mainIndex::closedFolder::icon) );
	m_categoryBook->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsTristate);
	m_categoryBook->setCheckState(0, Qt::Unchecked);
	addTopLevelItem(m_categoryBook);
	
	m_categoryDevotionals = new QTreeWidgetItem(this);
	m_categoryDevotionals->setText(0, i18n("Daily Devotionals"));
	m_categoryDevotionals->setIcon( 0, util::filesystem::DirectoryUtil::getIcon(CResMgr::mainIndex::closedFolder::icon) );
	m_categoryDevotionals->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsTristate);
	m_categoryDevotionals->setCheckState(0, Qt::Unchecked);
	addTopLevelItem(m_categoryDevotionals);
	
	m_categoryGlossaries = new QTreeWidgetItem(this);
	m_categoryGlossaries->setText(0, i18n("Glossaries"));
	m_categoryGlossaries->setIcon( 0, util::filesystem::DirectoryUtil::getIcon(CResMgr::mainIndex::closedFolder::icon) );
	m_categoryGlossaries->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsTristate);
	m_categoryGlossaries->setCheckState(0, Qt::Unchecked);
	addTopLevelItem(m_categoryGlossaries);

	m_categoryBible->setExpanded(true);
	m_categoryCommentary->setExpanded(true);
	m_categoryLexicon->setExpanded(true);
	m_categoryBook->setExpanded(true);
	m_categoryDevotionals->setExpanded(true);
	m_categoryGlossaries->setExpanded(true);

	connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)), SLOT(slotItemClicked(void))); //items have to be clicked only once in double click mode
	connect(this, SIGNAL(itemPressed(QTreeWidgetItem*, int)), SLOT(slotItemClicked(void)));
}

void CSwordSetupModuleListView::finish() {
	if (!m_categoryBible->childCount()) delete m_categoryBible;
	if (!m_categoryCommentary->childCount()) delete m_categoryCommentary;
	if (!m_categoryBook->childCount()) delete m_categoryBook;
	if (!m_categoryLexicon->childCount()) delete m_categoryLexicon;
	if (!m_categoryDevotionals->childCount()) delete m_categoryDevotionals;
	if (!m_categoryGlossaries->childCount()) delete m_categoryGlossaries;
}

void CSwordSetupModuleListView::clear() {
	QTreeWidget::clear();
	init();
}

void CSwordSetupModuleListView::addModule(CSwordModuleInfo* module, QString localVersion) {

	QTreeWidgetItem* parent = 0;
	switch ( module->type() ) {
	case CSwordModuleInfo::Bible:
		parent = m_categoryBible;
		break;
	case CSwordModuleInfo::Commentary:
		parent = m_categoryCommentary;
		break;
	case CSwordModuleInfo::Lexicon:
		parent = m_categoryLexicon;
		break;
	case CSwordModuleInfo::GenericBook:
		parent = m_categoryBook;
		break;
	default:
		parent = 0;
		break;
	}
	Q_ASSERT(parent);

	//handling for special module types
	if ((parent == m_categoryLexicon) && (module->category() == CSwordModuleInfo::Glossary)) {
		parent = m_categoryGlossaries;
	}
	else if ((parent == m_categoryLexicon) && (module->category() == CSwordModuleInfo::DailyDevotional)) {
		parent = m_categoryDevotionals;
	}

	//now we know the category, find the right language group in that category
	const CLanguageMgr::Language* const lang = module->language();
	QString langName = lang->translatedName();
	if (!lang->isValid()) {
		langName = QString(module->module()->Lang());
	}

	QTreeWidgetItem * langFolder = 0;
	if (parent) {
		for(int i = 0; i < parent->childCount(); i++)
		{
			if (parent->child(i)->text(0) == langName) { //found right folder
				langFolder = parent->child(i);
				break;
			}
		}
	}

	if (!langFolder) { //not yet there
		langFolder = new QTreeWidgetItem(parent);
		langFolder->setText(0, langName);
		langFolder->setIcon( 0, util::filesystem::DirectoryUtil::getIcon(CResMgr::mainIndex::closedFolder::icon) );
		langFolder->setExpanded(false);
		langFolder->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsTristate);
		langFolder->setCheckState(0, Qt::Unchecked);
	}

	Q_ASSERT(langFolder);

	QTreeWidgetItem* newItem = 0;
	newItem = new QTreeWidgetItem(langFolder);
	newItem->setText(0, module->name());
	newItem->setIcon(0, CToolClass::getIconForModule(module));
	newItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsTristate);
	newItem->setCheckState(0, Qt::Unchecked);
	
	if (m_is_remote) {
		newItem->setText(1, localVersion.isEmpty() ? i18n("New") : i18n("Updated"));
	} 
	else {
		newItem->setText(1, i18n("Installed") );
	}

	newItem->setText(2, localVersion);
	if (m_is_remote) {
		newItem->setText(3, module->config(CSwordModuleInfo::ModuleVersion));
	}
	else {
		newItem->setText(3, module->config(CSwordModuleInfo::AbsoluteDataPath));
	}
}

//Actually finds the items that are checked, not selected.
QStringList CSwordSetupModuleListView::selectedModules() {
	QStringList moduleList;
	
	QTreeWidgetItemIterator it( this );
	while ( (*it) ) {
		if ((*it)->checkState(0) == Qt::Checked && !(*it)->text(1).isEmpty() ) {
			moduleList << (*it)->text(0);
		}
		++it;
	}
	return moduleList;
}

void CSwordSetupModuleListView::slotItemClicked(void) {
		emit selectedModulesChanged();
}

bool CSwordSetupModuleListView::showTooltip(QTreeWidgetItem* i, const QPoint&, int) const {
	//TODO:check
	return i;
}

QString CSwordSetupModuleListView::tooltip(QTreeWidgetItem* i, int /*column*/) const {
	//QString ret;
	//Q3CheckListItem* checkItem = dynamic_cast<Q3CheckListItem*>( i );
	//if (checkItem && (checkItem->type() == Q3CheckListItem::CheckBox)) {
	const QString moduleName = i->text(0);
	CSwordModuleInfo* module = m_backend->findModuleByName(moduleName);
	return CToolClass::moduleToolTip(module);
}

} //NAMESPACE

