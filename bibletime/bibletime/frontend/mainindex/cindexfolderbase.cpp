//
// C++ Implementation: cindexfolderbase
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//


CFolderBase::CFolderBase(CMainIndex* mainIndex, const Type type) : CItemBase(mainIndex, type) {}

CFolderBase::CFolderBase(CFolderBase* parentItem, const Type type) : CItemBase(parentItem, type) {}

CFolderBase::CFolderBase(CFolderBase* parentFolder, const QString& caption) : CItemBase(parentFolder) {
	setText(0, caption);
}

CFolderBase::~CFolderBase() {}

const bool CFolderBase::isFolder() {
	return true;
}

void CFolderBase::update() {
	CItemBase::update();
	if (isOpen() && childCount())
		setPixmap(0, SmallIcon(CResMgr::mainIndex::openedFolder::icon, 16));
	else
		setPixmap(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));
}

void CFolderBase::init() {
	CItemBase::init();
	setDropEnabled(false);
	setDragEnabled(false);
}

/** No descriptions */
void CFolderBase::setOpen( bool open ) {
	KListViewItem::setOpen(open);
	update();
}

/** The function which renames this folder. */
void CFolderBase::rename() {
	startRename(0);
}

/** Creates a new sub folder of this folder. */
void CFolderBase::newSubFolder() {
	if (dynamic_cast<CBookmarkFolder*>(this) || dynamic_cast<Bookmarks::SubFolder*>(this) ) {
		Bookmarks::SubFolder* f = new Bookmarks::SubFolder(this, i18n("New folder"));
		f->init();

		listView()->setCurrentItem(f);
		listView()->ensureItemVisible(f);
		f->rename();
	}
}

/** Reimplementation. Returns true if the drop is accepted. */
const bool CFolderBase::allowAutoOpen( const QMimeSource* ) const {
	return true;
}

/** Reimplementation. Returns false because folders have no use for drops (except for the bookmark folders) */
bool CFolderBase::acceptDrop(const QMimeSource*) const {
	return false;
}

Q3PtrList<Q3ListViewItem> CFolderBase::getChildList() {
	Q3PtrList<Q3ListViewItem> childs;
	if (!childCount()) //no childs available
		return childs;

	Q3ListViewItem* i = firstChild();
	while (i && (i->parent() == this)) {
		CItemBase* item = dynamic_cast<CItemBase*>(i);
		if (item) { //we found a valid item
			childs.append(item);

			CFolderBase* folder = dynamic_cast<CFolderBase*>(i);
			if (folder) {
				Q3PtrList<Q3ListViewItem> subChilds = folder->getChildList();
				for (Q3ListViewItem* ci = subChilds.first(); ci; ci = subChilds.next()) {
					childs.append(ci);
				}
			}
		}

		do {
			i = i->nextSibling();
		}
		while (i && (i->parent() != this));
	}

	return childs;
}
