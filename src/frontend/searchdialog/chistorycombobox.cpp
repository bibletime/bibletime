/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "chistorycombobox.h"

#include <QCompleter>
#include <QString>


namespace Search {

CHistoryComboBox::CHistoryComboBox( QWidget* parent)
        : QComboBox(parent) {
    setEditable(true);
    completer()->setCompletionMode(QCompleter::PopupCompletion);
}

CHistoryComboBox::~CHistoryComboBox() {
}

void CHistoryComboBox::addToHistory(const QString& text) {
    int index = findText(text);
    if ( index >= 0)
        removeItem(index);
    insertItem(1, text);
    setCurrentIndex(1);
}

QStringList CHistoryComboBox::historyItems() const {
    QStringList items;
    for (int i = 0; i < count(); i++) {
        QString text = itemText(i);
        if (text.size() > 0)
            items << text;
    }
    return items;
}
} //end of namespace Search

