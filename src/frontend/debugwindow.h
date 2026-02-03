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

#pragma once

#include <QTextEdit>

#include <QPointF>


class QQuickItem;
class QQuickWidget;

class DebugWindow : public QTextEdit {

    Q_OBJECT

public: // methods:

    DebugWindow();

    void retranslateUi();

    void timerEvent(QTimerEvent * const event) override;

    static QQuickItem const *
    quickItemInFocus(QQuickWidget const * quickWidget, QPointF cursorPosition);

private: // fields:

    int const m_updateTimerId;

}; // class DebugWindow
