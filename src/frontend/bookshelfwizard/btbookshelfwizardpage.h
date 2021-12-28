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

#include <QWizardPage>

#include <QObject>
#include <QString>


class BtBookshelfWizard;
class QWidget;

class BtBookshelfWizardPage: public QWizardPage {

    Q_OBJECT

public: // methods:

    BtBookshelfWizardPage(QWidget * parent = nullptr);

    BtBookshelfWizard & btWizard() const noexcept;

}; /* class BtBookshelfWizardPage */
