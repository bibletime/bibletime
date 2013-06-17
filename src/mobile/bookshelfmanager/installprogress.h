#ifndef INSTALL_PROGRESS_H
#define INSTALL_PROGRESS_H

#include <QObject>
#include <QMultiMap>
#include <QMap>

class QQuickItem;
class CSwordModuleInfo;
class BtInstallThread;

namespace btm {

class InstallProgress: public QObject {
    Q_OBJECT

public:
    InstallProgress(QObject* parent = 0);

    void openProgress(const QList<CSwordModuleInfo*>& modules);

private slots:
    void cancel();
    void close();
    void slotOneItemCompleted(QString module, QString source, int status);
    void slotOneItemStopped(QString module, QString source);
    void slotStopInstall();
    void slotStatusUpdated(QString module, int status);
    void slotDownloadStarted(QString module);
    void slotInstallStarted(QString module, QString);
    bool threadsDone();

private:
    void findProgressObject();
    QString getSourcePath();
    void oneItemStoppedOrCompleted(QString module, QString source, QString message);
    void setProperties();
    void startThreads();

    QQuickItem* m_progressObject;
    QMultiMap<QString, BtInstallThread*> m_waitingThreads;
    QMultiMap<QString, BtInstallThread*> m_runningThreads;
    QMap<QString, BtInstallThread*> m_threadsByModule;
};


}

#endif
