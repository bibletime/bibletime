//
// C++ Interface: cconfigurationdialog
//
// Description: BibleTime Configuration dialog.
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 1999-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CCONFIGURATIONDIALOG_H
#define CCONFIGURATIONDIALOG_H

#include "frontend/bookshelfmanager/btconfigdialog.h"

class QWidget;
class QAbstractButton;

class CDisplaySettingsPage;
class CSwordSettingsPage;
class CLanguageSettingsPage;
class CAcceleratorSettingsPage;
class QDialogButtonBox;
class BtActionCollection;

class CConfigurationDialog : public BtConfigDialog {
	Q_OBJECT

public:
	CConfigurationDialog(QWidget *parent, BtActionCollection* actionCollection);
	virtual ~CConfigurationDialog();
protected slots:
	void slotButtonClicked(QAbstractButton *);
private:
	BtActionCollection* m_actionCollection;
	CDisplaySettingsPage* m_displayPage;
	CSwordSettingsPage* m_swordPage;
	CAcceleratorSettingsPage* m_acceleratorsPage;
	CLanguageSettingsPage* m_languagesPage;
	QDialogButtonBox* m_bbox;

signals:
	void signalSettingsChanged();
};


#endif
