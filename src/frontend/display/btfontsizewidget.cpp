/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btfontsizewidget.h"

#include <QCompleter>
#include <QFontDatabase>
#include <QValidator>
#include "../../util/btconnect.h"


BtFontSizeWidget::BtFontSizeWidget(QWidget * parent)
        : QComboBox(parent)
        , m_validator(new QIntValidator(1, 99, this))
{
    setEditable(true);
    setValidator(m_validator);
    completer()->setCompletionMode(QCompleter::PopupCompletion);

    Q_FOREACH (int const size, QFontDatabase().standardSizes()) {
        if (size > m_validator->top())
            m_validator->setTop(size);
        addItem(QString::number(size), QVariant(size));
    }

    BT_CONNECT(this, SIGNAL(currentTextChanged(QString const &)),
               this, SLOT(changed(QString const &)));
}

void BtFontSizeWidget::changed(QString const & text) {
    emit fontSizeChanged(text.toInt());
}

void BtFontSizeWidget::setFontSize(int size) {
    if ((size < 1) || (size > m_validator->top()))
        size = 12;
    setCurrentText(QString::number(size));
}

int BtFontSizeWidget::fontSize() const {
    return currentText().toInt();
}
