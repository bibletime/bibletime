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

#include "frontend/btcentralwidget.h"

#include "frontend/cmdiarea.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/displaywindow/cdisplaywindow.h"
#include <QAction>
#include <QKeyEvent>
#include <QKeySequence>


    BtCentralWidget::BtCentralWidget(CMDIArea* mdiArea, BtFindWidget* findWidget, QWidget* parent)
        : QWidget(parent), m_mdiArea(mdiArea), m_findWidget(findWidget) {
    }

void BtCentralWidget::keyPressEvent(QKeyEvent *e) {

//    const QString text = e->text();
//    if (text.isEmpty()) {
        QWidget::keyPressEvent(e);
//        return;
//    }
//    CDisplayWindow* activeDisplayWindow = m_mdiArea->getActiveDisplayWindow();
//    if (activeDisplayWindow != 0) {

//        BtActionCollection* actionCollection =  activeDisplayWindow->actionCollection();
//        QAction* findAction = actionCollection->action("findText");
//        QKeySequence keySequence = findAction->shortcut();
//        QString key = keySequence.toString();
//        if (key == text)
//            return;
//    }

//    if (text.startsWith(QLatin1Char('/'))) {
//        if (!m_findWidget->isVisible()) {
//            m_findWidget->showAndClear();
//        } else {
//            m_findWidget->show();
//        }
//    } else {
//        QWidget::keyPressEvent(e);
//    }
}
