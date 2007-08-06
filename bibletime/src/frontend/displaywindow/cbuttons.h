/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CBUTTONS_H
#define CBUTTONS_H

//BibleTime includes
#include "backend/managers/cswordbackend.h"
#include "backend/drivers/cswordmoduleinfo.h"

//QT includes
#include <QHash>
#include <QToolButton>


class KMenu;


/** This class manages the display options of the selected modules.
  * @author The BibleTime team
  */
class CDisplaySettingsButton : public QToolButton  {
	Q_OBJECT
public:
	CDisplaySettingsButton(CSwordBackend::DisplayOptions *displaySettings, CSwordBackend::FilterOptions *settings, const ListCSwordModuleInfo& useModules, QWidget *parent=0);
	void reset(const ListCSwordModuleInfo& useModules);
	/**
	* Sets the item at position pos to the satet given as 2nd paramter.
	*/
	void setItemStatus( const int pos, const bool checked );
	/**
	* Returns the number of usable menu items in the setttings menu.
	*/
	const int menuItemCount();
	/**
	* Returns the status of the item at position "index"
	*/
	const bool itemStatus( const int index );
	/**
	* Sets the status to changed. The signal changed will be emitted.
	*/
	void setChanged();

signals:
	void sigChanged(void);

protected slots:
	void optionToggled(QAction* action);

protected:
	CSwordBackend::FilterOptions*  m_moduleSettings;
	CSwordBackend::DisplayOptions* m_displaySettings;
	CSwordBackend::FilterOptions m_available;
	ListCSwordModuleInfo m_modules;

	QHash<QString, int> m_dict;

	KMenu* m_popup;

	int populateMenu();
	bool isOptionAvailable( const CSwordModuleInfo::FilterTypes option);
	int addMenuEntry( const QString name, const int* option, const bool available);
};

#endif
