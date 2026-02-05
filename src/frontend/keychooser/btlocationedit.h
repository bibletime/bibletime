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

#pragma once

#include <QLineEdit>


class BtLocationEdit final : public QLineEdit {

public: // Methods:

    BtLocationEdit(QWidget * const parent) : QLineEdit(parent) {}

protected: // Methods:

    void focusInEvent(QFocusEvent * event) override;
    void keyPressEvent(QKeyEvent * event) override;

}; // class BtLocationEdit
