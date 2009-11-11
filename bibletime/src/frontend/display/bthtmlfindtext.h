/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTHTMLFINDTEXT_H
#define BTHTMLFINDTEXT_H

/// \todo Qt Designer UI file!?
#include "ui_bthtmlfindtext.h"


class CMDIArea;
class QWebView;

class BtHtmlFindText : public QDialog {
        Q_OBJECT

    public:
        BtHtmlFindText(CMDIArea* mdiArea, QWidget *parent = 0, Qt::WindowFlags f = 0);
        ~BtHtmlFindText();
    public slots:
        void findNext();
        void findPrevious();
        void doHide();
    private:
        QWebView* getActiveWindowWebView();
        Ui_findTextDialog ui;
        CMDIArea* m_mdiArea;
};


#endif
