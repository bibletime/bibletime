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

#ifndef BTINSTALLPROGRESSDIALOG_H
#define BTINSTALLPROGRESSDIALOG_H

#include <QDialog>

#include <QString>


class BtInstallThread;
class CSwordModuleInfo;
class QProgressBar;
class QTreeWidget;
class QTreeWidgetItem;

class BtInstallProgressDialog: public QDialog {

    Q_OBJECT

public: /* Methods: */

    BtInstallProgressDialog(const QList<CSwordModuleInfo *> & modules,
                            const QString & destination,
                            QWidget * parent = 0,
                            Qt::WindowFlags flags = 0);
    ~BtInstallProgressDialog();

public slots:

    void slotStopInstall();
    void slotInstallStarted(int moduleIndex);
    void slotDownloadStarted(int moduleIndex);
    void slotStatusUpdated(int moduleIndex, int status);
    void slotOneItemCompleted(int moduleIndex, bool status);
    void slotThreadFinished();

protected: /* Methods: */

    /**
      Handles closing by the window close button, Cancel (Stop) All button, or
      completing the downloads.
    */
    virtual void closeEvent(QCloseEvent * event);

private: /* Methods: */

    QProgressBar * getOrCreateProgressBar(int moduleIndex);
    QProgressBar * getOrCreateProgressBar(QTreeWidgetItem * item);

private: /* Fields: */

    QTreeWidget * m_statusWidget;
    QPushButton * m_stopAllButton;
    BtInstallThread * m_thread;
    int m_nextInstallIndex;

};

#endif
