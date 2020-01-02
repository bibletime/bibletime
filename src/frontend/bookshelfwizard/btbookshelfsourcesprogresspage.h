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

#ifndef BTBOOKSHELFSOURCESPROGRESSPAGE
#define BTBOOKSHELFSOURCESPROGRESSPAGE

#include "btbookshelfwizardpage.h"


class BtSourcesThread;
class QLabel;
class QProgressBar;
class QPushButton;

class BtBookshelfSourcesProgressPage final: public BtBookshelfWizardPage {

    Q_OBJECT

public: /* Methods: */

    BtBookshelfSourcesProgressPage(QWidget * parent = 0);
    inline ~BtBookshelfSourcesProgressPage() noexcept override
    { destroyThread(); }

    void destroyThread() noexcept;

    void initializePage() final override;
    bool isComplete() const final override;
    int nextId() const final override;

public slots:
    void slotStopInstall();

private slots:

    void slotThreadFinished();

private: /* Methods: */

    void retranslateUi();

private: /* Methods: */

    bool m_installCompleted = false;
    QLabel * m_msgLabel;
    QProgressBar * m_progressBar;
    QPushButton * m_stopButton;
    BtSourcesThread * m_thread = nullptr;

};

#endif
