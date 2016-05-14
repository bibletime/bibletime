/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTBOOKSHELFWIZARDPAGE_H
#define BTBOOKSHELFWIZARDPAGE_H

#include <QWizardPage>

#include <utility>
#include "frontend/bookshelfwizard/btbookshelfwizard.h"
#include "util/btassert.h"


class BtBookshelfWizardPage: public QWizardPage {

    Q_OBJECT

public: /* Methods: */

    template <typename ... Args>
    BtBookshelfWizardPage(Args && ... args)
        : QWizardPage(std::forward<Args>(args)...)
    {}

    inline BtBookshelfWizard & btWizard() const noexcept {
        BtBookshelfWizard * const w =
                qobject_cast<BtBookshelfWizard *>(wizard());
        BT_ASSERT(w);
        return *w;
    }

}; /* class BtBookshelfWizardPage */

#endif
