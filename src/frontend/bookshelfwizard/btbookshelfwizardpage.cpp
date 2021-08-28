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

#include "btbookshelfwizardpage.h"

#include <QWizard>
#include "btbookshelfwizard.h"
#include "../../util/btassert.h"


BtBookshelfWizardPage::BtBookshelfWizardPage(QWidget * parent)
    : QWizardPage(parent)
{}

BtBookshelfWizard & BtBookshelfWizardPage::btWizard() const noexcept {
    BtBookshelfWizard * const w =
            qobject_cast<BtBookshelfWizard *>(wizard());
    BT_ASSERT(w);
    return *w;
}
