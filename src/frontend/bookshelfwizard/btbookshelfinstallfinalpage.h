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

#include "backend/drivers/btmoduleset.h"

#include <QList>
#include <QWizardPage>

class BtBookshelfWizard;
class BtInstallThread;
class QSwordModuleInfo;
class QLabel;
class QProgressBar;
class QPushButton;
class QVBoxLayout;

class BtBookshelfInstallFinalPage: public QWizardPage {

    Q_OBJECT

public:

    BtBookshelfInstallFinalPage(QWidget *parent = 0);

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

private:
    BtBookshelfWizard *btWizard();
    void initConnections();
    void installWorks();
    bool moduleIsInstalled(const CSwordModuleInfo *mInfo);
    void removeWorks();
    void retranslateUi();
    void setupUi();

    QLabel *m_msgLabel;
    QProgressBar *m_progressBar;
    QPushButton *m_stopButton;
    BtInstallThread *m_thread;
    QVBoxLayout *m_verticalLayout;

    bool m_installFailed;

    QList<CSwordModuleInfo*> m_modules;
    int m_lastStatus;
    bool m_installCompleted;
};
#endif
