/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTABOUTDIALOG_H
#define BTABOUTDIALOG_H

#include "frontend/htmldialogs/bttabhtmldialog.h"


class BtAboutDialog : public BtTabHtmlDialog {
        Q_OBJECT
    public:
        BtAboutDialog(QWidget *parent = 0, Qt::WindowFlags wflags = Qt::Dialog);
        ~BtAboutDialog();

        void initBtTab();
        void initContributorsTab();
        void initSwordTab();
        void initQtTab();
        void initLicenceTab();
};

#endif
