/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CBUTTONS_H
#define CBUTTONS_H

//BibleTime includes
#include "backend/cswordbackend.h"
#include "backend/cswordmoduleinfo.h"

//QT includes
#include <qdict.h>

//KDE includes
#include <ktoolbarbutton.h>

class KPopupMenu;

/** This class manages the transliteration of the selected modules.
  * @author The BibleTime team
  */
// class CTransliterationButton : public KToolBarButton  {
//    Q_OBJECT
// public:
//  CTransliterationButton(CSwordBackend::FilterOptions* displayOptions, QWidget *parent, const char *name=0);
//  ~CTransliterationButton();
//   /**
//   * Setup the menu entries.
//   */
//   void populateMenu();
//   /**
//   * Resets the buttons with the list of used modules.
//   */
//   void reset( ListCSwordModuleInfo& modules );
//
// protected slots: // Protected slots
//   void optionSelected(int);
//
// private:
//   KPopupMenu* m_popup;
//   CSwordBackend::FilterOptions* m_filterOptions;
//
// signals:
//   void sigChanged();
// };

/** This class manages the display options of the selected modules.
  * @author The BibleTime team
  */
class CDisplaySettingsButton : public KToolBarButton  {
	Q_OBJECT
public:
	CDisplaySettingsButton(CSwordBackend::DisplayOptions *displaySettings, CSwordBackend::FilterOptions *settings, const ListCSwordModuleInfo& useModules, QWidget *parent=0, const char *name=0);
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
	void optionToggled(int ID);

protected:
	CSwordBackend::FilterOptions*  m_moduleSettings;
	CSwordBackend::DisplayOptions* m_displaySettings;
	CSwordBackend::FilterOptions m_available;
	ListCSwordModuleInfo m_modules;

	QDict<int> m_dict;

	KPopupMenu* m_popup;

	int populateMenu();
	bool isOptionAvailable( const CSwordModuleInfo::FilterTypes option);
	int addMenuEntry( const QString name, const int* option, const bool available);
};

#endif
