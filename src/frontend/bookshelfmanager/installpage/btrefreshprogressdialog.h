/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTREFRESHPROGRESSDIALOG_H
#define BTREFRESHPROGRESSDIALOG_H

#include <QProgressDialog>

#include "frontend/bookshelfmanager/btinstallmgr.h"


class BtRefreshProgressDialog: public QProgressDialog {
    Q_OBJECT

    public: /* Methods: */
        BtRefreshProgressDialog(sword::InstallSource &source,
                                QWidget *parent = 0, Qt::WindowFlags f = 0);

        bool runAndDelete();

    private slots:

        void slotPercentCompleted(int, int);
        void slotCanceled();

    private: /* Fields: */

        sword::InstallSource m_source;
        BtInstallMgr m_installMgr;
};

#endif // BTREFRESHPROGRESSDIALOG_H
