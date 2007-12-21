/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/


#include "cbookshelfindex.h"
#include "cbookshelfindex.moc"

#include "btindexitem.h"
#include "btindexmodule.h"
#include "btindexfolder.h"
#include "chidemodulechooserdialog.h"

#include "backend/btmoduletreeitem.h"
#include "backend/managers/creferencemanager.h"

#include "frontend/searchdialog/csearchdialog.h"
#include "frontend/cbtconfig.h"
#include "frontend/cinfodisplay.h"
#include "frontend/btaboutmoduledialog.h"
#include "frontend/cprinter.h"
#include "frontend/cdragdrop.h"

#include "util/cresmgr.h"
#include "util/directoryutil.h"

//Qt includes
//#include <QRegExp>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QHeaderView> //for hiding the header
#include <QTimer> // for delayed auto-opening of folders
#include <QList>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QInputDialog> // for unlocking key
#include <QDebug>

#include <boost/scoped_ptr.hpp>

//KDE includes
#include <klocale.h>
#include <kmenu.h>
#include <kaction.h>
#include <kactionmenu.h>

CBookshelfIndex::CBookshelfIndex(QWidget *parent)
	: QTreeWidget(parent),
	m_searchDialog(0),
	m_autoOpenFolder(0),
	m_autoOpenTimer(this),
	m_showHidden(false)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	initView();
	initConnections();
	initTree();
}

CBookshelfIndex::~CBookshelfIndex() {}



/** Initializes the view. */
void CBookshelfIndex::initView()
{	
	qDebug("CBookshelfIndex::initView");

	header()->hide();

	setFocusPolicy(Qt::WheelFocus);
	setAcceptDrops( true );
	setDragEnabled( true );
	setDropIndicatorShown( true );

	setItemsExpandable(true);
	viewport()->setAcceptDrops(true);
	setRootIsDecorated(false);
	setAllColumnsShowFocus(true);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	//setup the popup menu
	m_popup = new KMenu(viewport());
	m_popup->addTitle(i18n("Bookshelf"));

	initActions();

	qDebug("CBookshelfIndex::initView end");
}

void CBookshelfIndex::initActions()
{

	// Each action has a type attached to it as a dynamic property, see actionenum.h.
	// Menuitem and its subitems can have the same type.
	// Actions can have also "singleItemAction" property if
	// it supports only one item.
	// See contextMenu() and BTIndexItem for how these properties are used later.

	// Actions are added to the popup menu and also to a list for easy foreach access.

	KActionMenu* actionMenu = 0;
	KAction* action = 0;

	// -------------------------Grouping --------------------------------------
	actionMenu = new KActionMenu(KIcon(CResMgr::mainIndex::grouping::icon), i18n("Grouping"), this);
	actionMenu->setDelayed(false);
	actionMenu->setProperty("indexActionType", QVariant(Grouping));
	m_actionList.append(actionMenu);

	m_groupingGroup = new QActionGroup(this);
	QObject::connect(m_groupingGroup, SIGNAL(triggered(QAction*)), this, SLOT(actionChangeGrouping(QAction*)) );

	action = newKAction(i18n("Category/Language"), CResMgr::mainIndex::grouping::icon, 0, 0, 0, this);
	action->setCheckable(true);
	action->setProperty("indexActionType", QVariant(Grouping));
	action->setProperty("grouping", BTModuleTreeItem::CatLangMod);
	actionMenu->addAction(action);
	m_groupingGroup->addAction(action);
	if (m_grouping == BTModuleTreeItem::CatLangMod) action->setChecked(true);
	m_actionList.append(action);

	action = newKAction(i18n("Category"),CResMgr::mainIndex::grouping::icon, 0, 0, 0, this);
	action->setCheckable(true);
	m_groupingGroup->addAction(action);
	if (m_grouping == BTModuleTreeItem::CatMod) action->setChecked(true);
	action->setProperty("indexActionType", QVariant(Grouping));
	action->setProperty("grouping", BTModuleTreeItem::CatMod);
	actionMenu->addAction(action);
	m_actionList.append(action);

	action = newKAction(i18n("Language/Category"),CResMgr::mainIndex::grouping::icon, 0, 0, 0, this);
	action->setCheckable(true);
	m_groupingGroup->addAction(action);
	if (m_grouping == BTModuleTreeItem::LangCatMod) action->setChecked(true);
	actionMenu->addAction(action);
	action->setProperty("indexActionType", QVariant(Grouping));
	action->setProperty("grouping", BTModuleTreeItem::LangCatMod);
	m_actionList.append(action);

	action = newKAction(i18n("Language"),CResMgr::mainIndex::grouping::icon, 0, 0, 0, this);
	action->setCheckable(true);
	m_groupingGroup->addAction(action);
	if (m_grouping == BTModuleTreeItem::LangMod) action->setChecked(true);
	actionMenu->addAction(action);
	action->setProperty("indexActionType", QVariant(Grouping));
	action->setProperty("grouping", BTModuleTreeItem::LangMod);
	m_actionList.append(action);

	action = newKAction(i18n("Modules Only"),CResMgr::mainIndex::grouping::icon, 0, 0, 0, this);
	action->setCheckable(true);
	m_groupingGroup->addAction(action);
	if (m_grouping == BTModuleTreeItem::Mod) action->setChecked(true);
	actionMenu->addAction(action);
	action->setProperty("indexActionType", QVariant(Grouping));
	action->setProperty("grouping", BTModuleTreeItem::Mod);
	m_actionList.append(action);
	
	m_popup->addAction(actionMenu);

	
	// -------------------Show hidden---------------------------
	qDebug("*****************create Show Hidden***************");
	action = newKAction(i18n("Show Hidden"),CResMgr::mainIndex::search::icon, 0, 0, 0, this);
	action->setProperty("indexActionType", QVariant(ShowAllModules));
	action->setCheckable(true);
	QObject::connect(action, SIGNAL(toggled(bool)), this, SLOT(actionShowModules(bool)));
	if (m_showHidden) action->setChecked(true); else action->setChecked(false);
	m_popup->addAction(action);
	m_actionList.append(action);

	// ------------Hide---------------------
	action = newKAction(i18n("Hide Modules..."),CResMgr::mainIndex::search::icon, 0, this, SLOT(actionHideModules()), this);
	action->setProperty("indexActionType", QVariant(HideModules));
	//action->setProperty("multiItemAction", QVariant(true));
	m_popup->addAction(action);
	m_actionList.append(action);


	m_popup->addSeparator();

	//------------------------------------------------------------
	//----------------- Actions for items ------------------------

	// -------------------------Edit module --------------------------------
	actionMenu = new KActionMenu(KIcon(CResMgr::mainIndex::editModuleMenu::icon), i18n("Edit"), this);
	actionMenu->setDelayed(false);
	actionMenu->setProperty("indexActionType", QVariant(EditModule));
	actionMenu->setProperty("singleItemAction", QVariant(true));
	m_actionList.append(actionMenu);

	action = newKAction(i18n("Plain text"),CResMgr::mainIndex::editModulePlain::icon, 0, this, SLOT(actionEditModulePlain()), this);
	actionMenu->addAction(action);
	m_actionList.append(action);
	action->setProperty("indexActionType", QVariant(EditModule));
	action->setProperty("singleItemAction", QVariant(true));

	action = newKAction(i18n("HTML"),CResMgr::mainIndex::editModuleHTML::icon, 0, this, SLOT(actionEditModuleHTML()), this);
	actionMenu->addAction(action);
	m_actionList.append(action);
	action->setProperty("indexActionType", QVariant(EditModule));
	action->setProperty("singleItemAction", QVariant(true));

	m_popup->addAction(actionMenu);

	
	// ------------------------ Misc actions -------------------------------------
	action = newKAction(i18n("Search"),CResMgr::mainIndex::search::icon, 0, this, SLOT(actionSearchInModules()), this);
	action->setProperty("indexActionType", QVariant(SearchModules));
	action->setProperty("multiItemAction", QVariant(true));
	m_popup->addAction(action);
	m_actionList.append(action);

	action = newKAction(i18n("Unlock"),CResMgr::mainIndex::unlockModule::icon, 0, this, SLOT(actionUnlockModule()), this);
	m_popup->addAction(action);
	action->setProperty("indexActionType", QVariant(UnlockModule));
	action->setProperty("singleItemAction", QVariant(true));
	m_actionList.append(action);

	action = newKAction(i18n("About"),CResMgr::mainIndex::aboutModule::icon, 0, this, SLOT(actionAboutModule()), this);
	m_popup->addAction(action);
	action->setProperty("singleItemAction", QVariant(true));
	action->setProperty("indexActionType", QVariant(AboutModule));
	m_actionList.append(action);

	
}

/** Convenience function for creating a new KAction.
* Should be replaced with something better; it was easier to make a new function
* than to modify all KAction constructors.
*/
KAction* CBookshelfIndex::newKAction(const QString& text, const QString& pix, const int shortcut, const QObject* receiver, const char* slot, QObject* parent)
{
	KAction* action = new KAction(KIcon(pix), text, parent);
	QObject::connect(action, SIGNAL(triggered()), receiver, slot);
	return action;
}

/** Initialize the SIGNAL<->SLOT connections */
void CBookshelfIndex::initConnections()
{
	qDebug("CBookshelfIndex::initConnections");
	
	//Strangely itemActivated only didn't let open a tree by clicking even though the relevant
	//code in slotExecuted was executed. Therefore itemClicked is necessary.
	QObject::connect(this, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(slotExecuted(QTreeWidgetItem*)));
	QObject::connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(slotExecuted(QTreeWidgetItem*)));

	QObject::connect(this, SIGNAL(dropped(QDropEvent*, QTreeWidgetItem*, QTreeWidgetItem*)),
		SLOT(dropped(QDropEvent*, QTreeWidgetItem*, QTreeWidgetItem*)));
 	
	QObject::connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
			SLOT(contextMenu(const QPoint&)));
	QObject::connect(&m_autoOpenTimer, SIGNAL(timeout()), this, SLOT(autoOpenTimeout()));

	QObject::connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotModifySelection()));

	qDebug("CBookshelfIndex::initConnections");
}

void CBookshelfIndex::slotModifySelection()
{
	// This creates recursion if a folder is selected, but not infinite.
	//qDebug("CBookshelfIndex::slotModifySelection");
	QList<QTreeWidgetItem*> selection = selectedItems();
	foreach (QTreeWidgetItem* item, selection) {
		BTIndexFolder* folder = dynamic_cast<BTIndexFolder*>(item);
		if (folder) {
			item->setSelected(false);
			break;
		}
	}
}

/**
* Hack to get single click and selection working. See slotExecuted.
*/
void CBookshelfIndex::mouseReleaseEvent(QMouseEvent* event) {
	//qDebug("CBookshelfIndex::mouseReleaseEvent");
	//qDebug() << event->type() << event->modifiers();
	m_mouseReleaseEventModifiers = event->modifiers();
	QTreeWidget::mouseReleaseEvent(event);
}

/** Called when an item is clicked with mouse or activated with keyboard. */
void CBookshelfIndex::slotExecuted( QTreeWidgetItem* i )
{
	qDebug("CBookshelfIndex::slotExecuted");

	//HACK: checking the modifier keys from the last mouseReleaseEvent
	//depends on executing order: mouseReleaseEvent first, then itemClicked signal
	int modifiers = m_mouseReleaseEventModifiers;
	m_mouseReleaseEventModifiers = Qt::NoModifier;
	if (modifiers != Qt::NoModifier) {
		return;
	}

	BTIndexItem* btItem = dynamic_cast<BTIndexItem*>(i);
	if (!btItem) {
		qWarning("item was not BTIndexItem!");
		return;
	}

	if (BTIndexModule* m = dynamic_cast<BTIndexModule*>(i))  { //clicked on a module
		CSwordModuleInfo* mod = m->moduleInfo();
		ListCSwordModuleInfo modules;
		modules.append(mod);
		qDebug("will emit createReadDisplayWindow");
		emit createReadDisplayWindow(modules, QString::null);
	} else {
		i->setExpanded( !i->isExpanded() );
	}
}

/** Reimplementation. Returns the drag object for the current selection. */
QMimeData* CBookshelfIndex::dragObject()
{
	//TODO: we have to add a mime type "module" if we want to for example enable draggin a module to a displaywindow
	return 0;
}


/** Reimplementation from QTreeWidget. Returns true if the drag is acceptable for the widget. */
void CBookshelfIndex::dragEnterEvent( QDragEnterEvent* event ) const
{
	qDebug("CBookshelfIndex::dragEnterEvent");
	//TODO: accept only text
	// But I doubt if searching the text in module by dragging is intuitive or if anyone really uses it
	event->acceptProposedAction();
}

void CBookshelfIndex::dragMoveEvent( QDragMoveEvent* event ) const
{
	const QPoint pos = event->pos();
 
	BTIndexItem* i = dynamic_cast<BTIndexItem*>(itemAt(pos));
	//TODO: implement accepting drop in item
	if (i && i->acceptDrop(event->mimeData()) ) {
		event->acceptProposedAction();
	} else {
	
	}
}

void CBookshelfIndex::dropEvent( QDropEvent* event ) const
{
	//TODO: see dragMoveEvent
}

/** No descriptions */
void CBookshelfIndex::initTree() {
	qDebug("CBookshelfIndex::initTree");
	
	//first clean the tree
	clear();
	m_grouping = (BTModuleTreeItem::Grouping)CBTConfig::get(CBTConfig::bookshelfGrouping);
	
	BTModuleTreeItem::HiddenOff hiddenFilter;
	QList<BTModuleTreeItem::Filter*> filters;
	if (!m_showHidden) {
		filters.append(&hiddenFilter);
	}
	BTModuleTreeItem root(filters, m_grouping);
	addToTree(&root, this->invisibleRootItem());
}

void CBookshelfIndex::addToTree(BTModuleTreeItem* item, QTreeWidgetItem* widgetItem)
{
	foreach (BTModuleTreeItem* i, item->children()) {
		if (i->type() == BTModuleTreeItem::Module) {
			addToTree(i, new BTIndexModule(i, widgetItem));
		}
		else
			addToTree(i, new BTIndexFolder(i, widgetItem));
	}
	// Possible TODO: if item is Language and it's under Category and Category is Glossaries,
	// add the second language name - but how to add other language group?
	// do we have to modify btmoduletreeitem?
}


/** No descriptions */
void CBookshelfIndex::emitModulesChosen( ListCSwordModuleInfo modules, QString key ) {
	emit createReadDisplayWindow(modules, key);
}



/** Shows the context menu at the given position. */
void CBookshelfIndex::contextMenu(const QPoint& p) {
	qDebug("CBookshelfIndex::contextMenu");
	qDebug() << "list of actions: ";
	foreach (KAction* action, m_actionList) {
		qDebug() << action->text();
	}
	//setup menu entries depending on current selection
	QTreeWidgetItem* i = itemAt(p);
	qDebug() << "item at point: " << i;
	QList<QTreeWidgetItem *> items = selectedItems();
	//The item which was clicked may not be selected - but don't add folder
	if (i  && !dynamic_cast<BTIndexFolder*>(i) && !items.contains(i))
		items.append(i);

	// disable those menu entries which are item dependent,
	// dis/enable some others
	foreach (KAction* action, m_actionList) {
		IndexAction actionType = (IndexAction)action->property("indexActionType").toInt();
		if ( actionType == Grouping ) {
			action->setEnabled(true);
		}
		else if (actionType == ShowAllModules) {
			//enabled only if there are hidden modules
			if (!CBTConfig::get(CBTConfig::hiddenModules).empty()) {
				action->setEnabled(true);
			} else {
				action->setEnabled(false);
			}
			action->setChecked(m_showHidden);
			
		}
		else if (actionType == SearchModules) {
			action->setText(i18n("Search"));
			if (items.count() > 0)
				action->setEnabled(true);
			else action->setEnabled(false);
		}
		else if (actionType == HideModules) {
			//action->setText(i18n("Hide"));
			action->setEnabled(true);
		}
		else action->setEnabled(false);
	}
	

	if (items.count() == 0) { 
		//special handling for no selection: do nothing
	}

	if (items.count() == 1) { 
		//special handling for one selected item
		qDebug("there was one selected item");
		BTIndexItem* btItem = dynamic_cast<BTIndexItem*>(items.at(0));

		if (btItem && !dynamic_cast<BTIndexFolder*>(btItem)) {
			foreach (KAction* action, m_actionList) {
				if ( (IndexAction)action->property("indexActionType").toInt() == AboutModule ) {
					qDebug() << "enabling action" << action->text();
					action->setEnabled(true);
				}
				else if ((IndexAction)action->property("indexActionType").toInt() == SearchModules ) {
					// Change the text of the menu item to reflect the module name
					BTIndexModule* modItem = dynamic_cast<BTIndexModule*>(btItem);
					if (modItem) {
						CSwordModuleInfo* info = modItem->moduleInfo();
						action->setText(QString(i18n("Search in")).append(QString(" ").append(info->name())));
					}
				}
				else {
					qDebug() << "ask item" << items.at(0)->text(0) << "to enable the action" << action->text();
					btItem->enableAction(action);
				}
				qDebug() << "action enabled is: " << action->isEnabled();
			}
		}
	}
	else { // more than one item
		foreach (KAction* action, m_actionList) {
			// Change the text of some menu items to reflect multiple selection
			if ((IndexAction)action->property("indexActionType").toInt() == SearchModules ) {
				action->setText( QString(i18n("Search in Selected")) );
			}
			// Enable items
			foreach(QTreeWidgetItem* item, items) {
				BTIndexItem* btItem = dynamic_cast<BTIndexItem*>(item);
				if (btItem && !action->property("singleItemAction").isValid()) {
					btItem->enableAction(action);
				}
			}
		}
	}
	
	//finally, open the popup
	m_popup->exec(mapToGlobal(p));
}

void CBookshelfIndex::actionChangeGrouping(QAction* action)
{
	BTModuleTreeItem::Grouping grouping = (BTModuleTreeItem::Grouping)action->property("grouping").toInt();
	m_grouping = grouping;
	CBTConfig::set(CBTConfig::bookshelfGrouping, grouping);
	initTree();
}


/** Opens the searchdialog for the selected modules. */
void CBookshelfIndex::actionSearchInModules() {
	QList<QTreeWidgetItem *> items = selectedItems();
	QListIterator<QTreeWidgetItem *> it(items);
	ListCSwordModuleInfo modules;
	while(it.hasNext()) {
		if (BTIndexModule* i = dynamic_cast<BTIndexModule*>(it.next())) {
			if (i->moduleInfo()) {
				modules.append(i->moduleInfo());
			}
		}
	}

	if (modules.isEmpty()) { //get a list of useful default modules for the search if no modules were selected
		CSwordModuleInfo* m = CBTConfig::get(CBTConfig::standardBible);
		if (m) {
			modules.append(m);
		}
	}

	Search::CSearchDialog::openDialog(modules, QString::null);
}

/** Unlocks the current module. */
void CBookshelfIndex::actionUnlockModule() {
	if (BTIndexModule* i = dynamic_cast<BTIndexModule*>(currentItem())) {
		bool ok = false;
		const QString unlockKey =
			QInputDialog::getText(
				this,
				i18n("BibleTime - Unlock work"),
				i18n("Enter the unlock key for this work."),
				QLineEdit::Normal,
				i->moduleInfo()->config(CSwordModuleInfo::CipherKey),
				&ok
			);
		
		if (ok) {
			i->moduleInfo()->unlock( unlockKey );
			emit signalSwordSetupChanged();
		}
	}
}

void CBookshelfIndex::actionShowModules(bool checked)
{
	qDebug("CBookshelfIndex::actionShowModules");
	m_showHidden = checked;
	initTree();
}

void CBookshelfIndex::actionHideModules()
{
	qDebug("CBookshelfIndex::actionHideModules");
	QString current;
	if (BTIndexModule* i = dynamic_cast<BTIndexModule*>(currentItem())) {
		current = i->text(0);
	}

	QString title(i18n("Hide/Unhide Modules"));
	QString label(i18n("Select the modules to be hidden."));
	CHideModuleChooserDialog* dlg = new CHideModuleChooserDialog(this, title, label, current);
	connect(dlg, SIGNAL(modulesChanged(ListCSwordModuleInfo)),
			this, SLOT(setHiddenModules(ListCSwordModuleInfo)));
	int code = dlg->exec();
	if (code == QDialog::Accepted) {
		initTree();
	}
}


/** Shows information about the current module. */
void CBookshelfIndex::actionAboutModule() {
	if (BTIndexModule* i = dynamic_cast<BTIndexModule*>(currentItem())) {
		//TODO: show "about" in mag or at least in scrollable view
//		KMessageBox::about(this, i->moduleInfo()->aboutText(), i->moduleInfo()->config(CSwordModuleInfo::Description), false);
		BTAboutModuleDialog* dialog = new BTAboutModuleDialog(this, i->moduleInfo());
		dialog->show();
		dialog->raise();
	}
}

/** Reimplementation. Takes care of movable items. */
void CBookshelfIndex::startDrag(Qt::DropActions supportedActions) {

}

/** Reimplementation to support the items dragEnter and dragLeave functions. */
void CBookshelfIndex::contentsDragMoveEvent( QDragMoveEvent* event ) {
// 	//  qWarning("void CBookshelfIndex:: drag move event ( QDragLeaveEvent* e )");
// 	CIndexItemBase* i = dynamic_cast<CIndexItemBase*>( itemAt( contentsToViewport(event->pos())) );
// 	if (i) {
// 		if (i->allowAutoOpen(event) || (i->acceptDrop(event) && i->isFolder() && i->allowAutoOpen(event) && !i->isOpen() && autoOpen()) ) {
// 			if (m_autoOpenFolder != i)  {
// 				m_autoOpenTimer.stop();
// 			}
// 			
// 			m_autoOpenFolder = i;
// 			m_autoOpenTimer.start( 400 );
// 		}
// 		else {
// 			m_autoOpenFolder = 0;
// 		}
// 	}
// 	else {
// 		m_autoOpenFolder = 0;
// 	}
// 
// 	QTreeWidget::contentsDragMoveEvent(event);
}


void CBookshelfIndex::autoOpenTimeout() {
	m_autoOpenTimer.stop();
	if (m_autoOpenFolder && !m_autoOpenFolder->isExpanded() && m_autoOpenFolder->childCount()) {
		m_autoOpenFolder->setExpanded(true);
	}
}

/** No descriptions */
void CBookshelfIndex::contentsDragLeaveEvent( QDragLeaveEvent* e ) {
// 	m_autoOpenTimer.stop();
// 	QTreeWidget::contentsDragLeaveEvent(e);
}


/** Opens an editor window to edit the modules content. */
void CBookshelfIndex::actionEditModulePlain() {
	ListCSwordModuleInfo modules;
	QList<QTreeWidgetItem *> items = selectedItems();
	QListIterator<QTreeWidgetItem *> it(items);
	//loop through items
	while(it.hasNext()) {
		if (BTIndexModule* i = dynamic_cast<BTIndexModule*>(it.next())) {
			modules.append(i->moduleInfo());
		}
	}
	if (modules.count() == 1) {
		emit createWriteDisplayWindow(modules.first(), QString::null, CDisplayWindow::PlainTextWindow);
	};
}

/** Opens an editor window to edit the modules content. */
void CBookshelfIndex::actionEditModuleHTML() {
	ListCSwordModuleInfo modules;
	QList<QTreeWidgetItem *> items = selectedItems();
	QListIterator<QTreeWidgetItem *> it(items);
	while(it.hasNext()) {
		if (BTIndexModule* i = dynamic_cast<BTIndexModule*>(it.next())) {
			modules.append(i->moduleInfo());
		}
	}
	
	if (modules.count() == 1) {
		emit createWriteDisplayWindow(modules.first(), QString::null, CDisplayWindow::HTMLWindow);
	}
}

/** Reloads the main index's Sword dependend things like modules */
void CBookshelfIndex::reloadSword() {
	//reload the modules
	qDebug("CBookshelfIndex::reloadSword");
	initTree();
}
