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

#include "backend/drivers/btmoduleset.h"

#include <QList>
#include <QWizardPage>

class BtBookshelfWizard;
class BtSourcesThread;
class QLabel;
class QProgressBar;
class QPushButton;
class QVBoxLayout;

class BtBookshelfSourcesProgressPage: public QWizardPage {

    Q_OBJECT

public:

    BtBookshelfSourcesProgressPage(QWidget *parent = 0);

    void initializePage();
    bool isComplete() const;
    int nextId() const;
    BtModuleSet selectedWorks() const;

private slots:
    void slotPercentComplete(int percentComplete);
    void slotShowMessage(const QString& msg);
    void slotStopInstall();
    void slotThreadFinished();

private:
    BtBookshelfWizard *btWizard();
    void initConnections();
    bool moduleIsInstalled(const CSwordModuleInfo *mInfo);
    void retranslateUi();
    void setupUi();
    void updateRemoteLibrariesList();
    void updateRemoteLibraryWorks();

    bool m_installCompleted;
    QLabel *m_msgLabel;
    QProgressBar *m_progressBar;
    QPushButton *m_stopButton;
    bool m_stopped;
    BtSourcesThread * m_thread;
    QVBoxLayout *m_verticalLayout;

    QList<CSwordModuleInfo*> m_modules;
};
#endif
