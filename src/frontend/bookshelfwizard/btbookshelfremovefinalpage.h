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

#include "btbookshelfwizardpage.h"

#include <QObject>
#include <QString>


class QLabel;
class QVBoxLayout;
class QWidget;

class BtBookshelfRemoveFinalPage final: public BtBookshelfWizardPage {

    Q_OBJECT

public: // methods:

    BtBookshelfRemoveFinalPage(QWidget * parent = nullptr);

    bool isComplete() const final override;
    void initializePage() final override;
    int nextId() const final override;

private: // methods:

    void retranslateUi();

private: // fields:

    QLabel * m_msgLabel;
    QVBoxLayout * m_verticalLayout;

}; /* class BtBookshelfRemoveFinalPage */
