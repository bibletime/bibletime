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

#include "btfontsizewidget.h"

#include <QCompleter>
#include <QFontDatabase>
#include <QIntValidator>
#include <QNonConstOverload>
#include <QVariant>
#include "../../util/btconnect.h"


BtFontSizeWidget::BtFontSizeWidget(QWidget * parent)
        : QComboBox(parent)
        , m_validator(new QIntValidator(1, 99, this))
{
    setEditable(true);
    setValidator(m_validator);
    completer()->setCompletionMode(QCompleter::PopupCompletion);

    #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    for (int const size : QFontDatabase().standardSizes()) {
    #else
    for (int const size : QFontDatabase::standardSizes()) {
    #endif
        if (size > m_validator->top())
            m_validator->setTop(size);
        addItem(QString::number(size), QVariant(size));
    }

    BT_CONNECT(this, qOverload<QString const &>(&QComboBox::currentTextChanged),
               [this](QString const & text)
               { Q_EMIT fontSizeChanged(text.toInt()); });
}

void BtFontSizeWidget::setFontSize(int size) {
    if ((size < 1) || (size > m_validator->top()))
        size = 12;
    setCurrentText(QString::number(size));
}

int BtFontSizeWidget::fontSize() const {
    return currentText().toInt();
}
