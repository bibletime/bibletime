//
// C++ Interface: clanguagesettings
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 1999-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CLANGUAGESETTINGS_H
#define CLANGUAGESETTINGS_H


#include "util/cpointers.h"
#include "backend/config/cbtconfig.h"
#include "frontend/bookshelfmanager/btconfigdialog.h"

#include <QWidget>
#include <QMap>

class QComboBox;
class QCheckBox;
class KFontChooser;


/**
	@author The BibleTime team <info@bibletime.info>
*/
class CLanguageSettingsPage : public BtConfigPage, CPointers
{
Q_OBJECT
public:
    CLanguageSettingsPage(QWidget *parent);
    ~CLanguageSettingsPage();
	void save();
	QString iconName();
	QString label();
	QString header();

protected slots:
	/**
	* This slot is called when the "Use own font for language" button was clicked.
	*/
	void useOwnFontClicked(bool);
	/**
	* Called when a new font in the fonts page was selected.
	*/
	void newDisplayWindowFontSelected(const QFont &);
	/**
	* Called when the combobox contents is changed
	*/
	void newDisplayWindowFontAreaSelected(const QString&);

private:	
	QComboBox* m_swordLocaleCombo;
	QComboBox* m_usageCombo;
	QCheckBox* m_useOwnFontCheck;
	KFontChooser* m_fontChooser;

	QMap<QString,CBTConfig::FontSettingsPair> m_fontMap;
};

#endif
