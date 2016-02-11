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

#ifndef BTBOOKSHELFINSTALLFINALPAGE
#define BTBOOKSHELFINSTALLFINALPAGE

#include "btbookshelfwizardpage.h"

#include <QList>
#include "backend/drivers/btmoduleset.h"


class BtBookshelfWizard;
class BtInstallThread;
class QLabel;
class QProgressBar;
class QPushButton;
class QSwordModuleInfo;
class QVBoxLayout;

class BtBookshelfInstallFinalPage: public BtBookshelfWizardPage {

    Q_OBJECT

public: /* Methods: */

    BtBookshelfInstallFinalPage(QWidget * parent = 0);

    void initializePage();
    bool isComplete() const;
    int nextId() const;
    BtModuleSet selectedWorks() const;

private slots:

    void slotInstallStarted(int moduleIndex);
    void slotOneItemCompleted(int moduleIndex, bool status);
    void slotStatusUpdated(int moduleIndex, int status);
    void slotStopInstall();
    void slotThreadFinished();

private: /* Methods: */

    void retranslateUi();

private: /* Fields: */

    QLabel * m_msgLabel;
    QLabel * m_msgLabel2;
    QProgressBar * m_progressBar;
    QPushButton * m_stopButton;
    BtInstallThread * m_thread;
    QVBoxLayout * m_verticalLayout;

    bool m_installFailed;
    bool m_installCompleted;

    QList<CSwordModuleInfo *> m_modules;
    int m_lastStatus;

}; /* class BtBookshelfInstallFinalPage */

#endif
