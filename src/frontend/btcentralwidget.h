/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTCENTRALWIDGET_H
#define BTCENTRALWIDGET_H

#include <QWidget>
class QKeyEvent;
class CMDIArea;
class BtFindWidget;

class BtCentralWidget: public QWidget {

public:
    BtCentralWidget(CMDIArea* mdiArea, BtFindWidget* findWidget, QWidget* parent=0);

protected:
    void keyPressEvent(QKeyEvent *e);

private:
    CMDIArea* m_mdiArea;
    BtFindWidget* m_findWidget;
};

#endif
