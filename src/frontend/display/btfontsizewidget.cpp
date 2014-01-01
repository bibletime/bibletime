/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/display/btfontsizewidget.h"

#include <QCompleter>
#include <QFontDatabase>


BtFontSizeWidget::BtFontSizeWidget(QWidget* parent)
        : QComboBox(parent) {
    setEditable(true);
    completer()->setCompletionMode(QCompleter::PopupCompletion);

    QFontDatabase database;
    const QList<int> sizes = database.standardSizes();
    QStringList list;
    for ( QList<int>::ConstIterator it = sizes.begin(); it != sizes.end(); ++it )
        list.append( QString::number( *it ) );
    addItems(list);

    bool ok = connect(this, SIGNAL(currentIndexChanged(const QString&)),
                      this, SLOT(changed(const QString&)));
    Q_ASSERT(ok);
}

BtFontSizeWidget::~BtFontSizeWidget() {
}

void BtFontSizeWidget::changed(const QString& text) {
    emit fontSizeChanged(text.toInt());
}

void BtFontSizeWidget::setFontSize(int size) {
    int index = findText(QString::number(size));
    if (index >= 0)
        setCurrentIndex(index);
}

int BtFontSizeWidget::fontSize() const {
    return currentText().toInt();
}
