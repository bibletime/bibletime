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

#ifndef BTBOOKSHELFWIZARDPAGE_H
#define BTBOOKSHELFWIZARDPAGE_H

#include <QWizardPage>


class BtBookshelfWizard;

class BtBookshelfWizardPage: public QWizardPage {

    Q_OBJECT

public: /* Methods: */

    BtBookshelfWizardPage(QWidget * parent = nullptr);

    BtBookshelfWizard & btWizard() const noexcept;

}; /* class BtBookshelfWizardPage */

#endif
