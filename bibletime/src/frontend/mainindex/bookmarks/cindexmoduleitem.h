//
// C++ Interface: cindexmoduleitem
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CINDEXMODULEITEM_H
#define CINDEXMODULEITEM_H


#include "cindexitembase.h"


class CSwordModuleInfo;
class QMimeSource;
class QDropEvent;
class QTreeWidgetItem;
class CIndexTreeFolder;

class CIndexModuleItem : public CIndexItemBase {
public:
	CIndexModuleItem(CIndexTreeFolder* item, CSwordModuleInfo* module);
	virtual ~CIndexModuleItem();
	virtual CSwordModuleInfo* const module() const;
	virtual const QString toolTip();

	/**
	* Reimplementation from  CItemBase.
	*/
	virtual const bool enableAction( const MenuAction action );
	virtual void update();
	virtual void init();

protected: // Protected methods
	/**
	* Reimplementation to handle text drops on a module.
	* In this case open the searchdialog. In the case of a referebnce open the module at the given position.
	*/
	virtual bool acceptDrop( const QMimeSource* src ) const;
	virtual void dropped( QDropEvent* e, QTreeWidgetItem* after );

private:
	CSwordModuleInfo* m_module;
};

#endif
