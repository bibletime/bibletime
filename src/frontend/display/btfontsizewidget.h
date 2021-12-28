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

#pragma once

#include <QComboBox>

#include <QObject>
#include <QString>


class QIntValidator;
class QWidget;

class BtFontSizeWidget: public QComboBox {

    Q_OBJECT

public: // methods:

    BtFontSizeWidget(QWidget * parent = nullptr);

    int fontSize() const;

public Q_SLOTS:

    void setFontSize(int size);

Q_SIGNALS:

    void fontSizeChanged(int);

private: // fields:

    QIntValidator * const m_validator;

}; /* class BtFontSizeWidget { */
