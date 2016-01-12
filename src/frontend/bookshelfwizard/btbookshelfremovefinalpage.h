/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTBOOKSHELFREMOVEFINALPAGE
#define BTBOOKSHELFREMOVEFINALPAGE

#include "frontend/bookshelfwizard/btbookshelfwizardpage.h"

#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "backend/drivers/btmoduleset.h"


class BtBookshelfWizard;
class QLabel;
class QVBoxLayout;

class BtBookshelfRemoveFinalPage: public BtBookshelfWizardPage {

    Q_OBJECT

public:

    BtBookshelfRemoveFinalPage(
            QWidget *parent = 0);

    bool isComplete() const;
    void initializePage();
    int nextId() const;
    BtModuleSet selectedWorks() const;

private:
    bool moduleIsInstalled(const CSwordModuleInfo *mInfo);
    void removeWorks();
    void retranslateUi();
    void setupUi();

    QLabel *m_msgLabel;
    QVBoxLayout *m_verticalLayout;
};
#endif
