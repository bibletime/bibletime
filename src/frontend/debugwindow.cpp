/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2025 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "debugwindow.h"

#include <QApplication>


DebugWindow::DebugWindow()
    : QTextEdit(nullptr)
    , m_updateTimerId(startTimer(100))
{
    setWindowFlags(Qt::Dialog);
    setAttribute(Qt::WA_DeleteOnClose);
    setReadOnly(true);
    retranslateUi();
    show();
}

void DebugWindow::retranslateUi()
{ setWindowTitle(tr("What's this widget?")); }

void DebugWindow::timerEvent(QTimerEvent * const event) {
    if (event->timerId() == m_updateTimerId) {
        if (QObject const * w = QApplication::widgetAt(QCursor::pos())) {
            QString objectHierarchy;
            do {
                QMetaObject const * m = w->metaObject();
                QString classHierarchy;
                do {
                    if (!classHierarchy.isEmpty())
                        classHierarchy += QStringLiteral(": ");
                    classHierarchy += m->className();
                    m = m->superClass();
                } while (m);
                if (!objectHierarchy.isEmpty()) {
                    objectHierarchy
                            .append(QStringLiteral("<br/>"))
                            .append(tr("<b>child of:</b> %1").arg(
                                        classHierarchy));
                } else {
                    objectHierarchy.append(
                                tr("<b>This widget is:</b> %1").arg(
                                    classHierarchy));
                }
                w = w->parent();
            } while (w);
            setHtml(objectHierarchy);
        } else {
            setText(tr("No widget"));
        }
    } else {
        QTextEdit::timerEvent(event);
    }
}

