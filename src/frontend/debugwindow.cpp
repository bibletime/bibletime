/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2026 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "debugwindow.h"

#include <QApplication>
#include <QQuickWidget>
#include <QQuickItem>
#include <utility>


namespace {

QString classHierarchy(QObject const * const object) {
    auto const * metaObject = object->metaObject();
    QString result;
    do {
        if (!result.isEmpty())
            result += QStringLiteral(": ");
        result += metaObject->className();
        metaObject = metaObject->superClass();
    } while (metaObject);
    return result;
}

void writeObjectHierarchy(QString & objectHierarchy,
                          QObject const * object,
                          QString const & firstLine,
                          QString const & childLine)
{
    do {
        if (objectHierarchy.isEmpty()) {
            objectHierarchy.append(firstLine.arg(classHierarchy(object)));
        } else {
            objectHierarchy.append(QStringLiteral("<br/>"))
                           .append(childLine.arg(classHierarchy(object)));
        }
        if (auto const quickItem = qobject_cast<QQuickItem const *>(object)) {
            object = quickItem->parentItem();
        } else {
            object = object->parent();
        }
    } while (object);
}

} // anonymous namespace


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
        auto const cursorPosition = QCursor::pos();
        if (QObject const * w = QApplication::widgetAt(cursorPosition)) {
            auto const childLine = tr("<b>child of:</b> %1");
            QString objectHierarchy;
            auto const * quickWidget = qobject_cast<QQuickWidget const *>(w);
            if (quickWidget) {
                auto const * item =
                        quickItemInFocus(quickWidget, cursorPosition);
                writeObjectHierarchy(objectHierarchy,
                                     item,
                                     tr("<b>This Quick item is:</b> %1"),
                                     childLine);
            }
            writeObjectHierarchy(objectHierarchy,
                                 w,
                                 tr("<b>This widget is:</b> %1"),
                                 childLine);
            setHtml(objectHierarchy);
        } else {
            setText(tr("No widget"));
        }
    } else {
        QTextEdit::timerEvent(event);
    }
}

QQuickItem const *
DebugWindow::quickItemInFocus(QQuickWidget const * quickWidget,
                              QPointF cursorPosition)
{
    auto const * item = quickWidget->rootObject();
    auto quickPosition = item->mapFromGlobal(std::move(cursorPosition));
    for (;;) {
        if (auto const * const child =
                item->childAt(quickPosition.x(), quickPosition.y()))
        {
            quickPosition = item->mapToItem(child, quickPosition);
            item = child;
        } else {
            return item;
        }
    }
}
