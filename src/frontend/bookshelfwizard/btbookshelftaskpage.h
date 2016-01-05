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

#ifndef BTBOOKSHELFTASKPAGE
#define BTBOOKSHELFTASKPAGE

#include "frontend/bookshelfwizard/btbookshelfworkspage.h"

#include <QWizardPage>

class QGroupBox;
class QLabel;
class QRadioButton;
class QVBoxLayout;

class BtBookshelfTaskPage: public QWizardPage {

    Q_OBJECT

public:
    BtBookshelfTaskPage(QWidget *parent = 0);

    int nextId() const;
    WizardTaskType taskType() const;

private:
    void retranslateUi();
    void setupRadioButtons();
    void setupUi();

    QLabel *m_warningLabel;
    QGroupBox *m_groupBox;
    QVBoxLayout *m_verticalLayout;
    QRadioButton *m_installRadioButton;
    QRadioButton *m_updateRadioButton;
    QRadioButton *m_removeRadioButton;
};

#endif
