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

#ifndef BTBOOKSHELFSOURCESPROGRESSPAGE
#define BTBOOKSHELFSOURCESPROGRESSPAGE

#include "frontend/bookshelfwizard/btbookshelfwizardpage.h"


class BtSourcesThread;
class QLabel;
class QProgressBar;
class QPushButton;

class BtBookshelfSourcesProgressPage: public BtBookshelfWizardPage {

    Q_OBJECT

public: /* Methods: */

    BtBookshelfSourcesProgressPage(QWidget * parent = 0);

    void initializePage();
    bool isComplete() const;
    int nextId() const;

private slots:

    void slotStopInstall();
    void slotThreadFinished();

private: /* Methods: */

    void retranslateUi();

private: /* Methods: */

    bool m_installCompleted = false;
    QLabel * m_msgLabel;
    QProgressBar * m_progressBar;
    QPushButton * m_stopButton;
    BtSourcesThread * m_thread;

};

#endif
