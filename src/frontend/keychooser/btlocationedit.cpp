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

#include "btlocationedit.h"

#include <QFocusEvent>
#include <QKeyCombination>
#include <QKeyEvent>
#include <QKeySequence>
#include "../../backend/config/btconfig.h"


void BtLocationEdit::focusInEvent(QFocusEvent * const event) {
    Qt::FocusReason reason = event->reason();
    if (reason == Qt::OtherFocusReason)
        selectAll();

    QWidget::focusInEvent(event);
}

void BtLocationEdit::keyPressEvent(QKeyEvent * const event) {
    auto const shortcuts =
            btConfig().getShortcuts(QStringLiteral("Displaywindow shortcuts"));
    if (auto const it = shortcuts.find("openLocation"); it != shortcuts.end()) {
        for (auto const & keySequence : it.value()) {
            if (keySequence.count() == 1) {
                if (event->keyCombination() == keySequence[0]) {
                    selectAll();
                    return;
                }
            }
        }
    } else if (event->keyCombination()
               == QKeyCombination(Qt::ControlModifier, Qt::Key_L))
    {
        selectAll();
        return;
    }
    QLineEdit::keyPressEvent(event);
}
