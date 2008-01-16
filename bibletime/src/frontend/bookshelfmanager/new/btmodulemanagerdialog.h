/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/


#ifndef BTMODULEMANAGERDIALOG_H
#define BTMODULEMANAGERDIALOG_H

#include "btconfigdialog.h"

class QWidget;

class BtModuleManagerDialog : public BtConfigDialog
{
	Q_OBJECT

public:
	BtModuleManagerDialog(QWidget* parent);
	~BtModuleManagerDialog(){}

	void slotClose();
};


#endif