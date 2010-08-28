/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTINSTALLPROGRESSDIALOG_H
#define BTINSTALLPROGRESSDIALOG_H

#include <QDialog>

#include <QMultiMap>
#include <QString>


class BtInstallThread;
class CSwordModuleInfo;
class QTreeWidget;
class QTreeWidgetItem;

class BtInstallProgressDialog : public QDialog {
        Q_OBJECT
    public:
        BtInstallProgressDialog(const QSet<CSwordModuleInfo*> &modules,
                                const QString &destination, QWidget *parent = 0,
                                Qt::WindowFlags flags = 0);

        ~BtInstallProgressDialog();

    public slots:
        void slotOneItemCompleted(QString module, QString source, int status);
        void slotOneItemStopped(QString module, QString source);
        void slotStopInstall();
        void slotStatusUpdated(QString module, int status);
        void slotDownloadStarted(QString module);
        void slotInstallStarted(QString module, QString);

    protected:
        /**
        * Handles closing by the window close button, Cancel (Stop) All button, or completing
        * the downloads.
        */
        virtual void closeEvent(QCloseEvent* event);

//signals:
//	void swordSetupChanged();

    private:

        /// \todo using maps is tedious and error prone. Find better solution for handling the modules
        // and their states.
        QMultiMap<QString, BtInstallThread*> m_waitingThreads;
        QMultiMap<QString, BtInstallThread*> m_runningThreads;
        QMap<QString, BtInstallThread*> m_threadsByModule;
        //QList<BtInstallThread*> m_doneThreads;

        QTreeWidget* m_statusWidget;

    private:
        QTreeWidgetItem* getItem(QString moduleName);
        bool threadsDone();
        void startThreads();
        void oneItemStoppedOrCompleted(QString module, QString source, QString message);
};

#endif
