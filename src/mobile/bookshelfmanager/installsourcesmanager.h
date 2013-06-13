#ifndef INSTALL_SOURCES_MANAGER_H
#define INSTALL_SOURCES_MANAGER_H

#include <QObject>
#include <QQuickItem>

namespace btm {

class InstallSourcesThread;

class InstallSourcesManager :public QObject {
    Q_OBJECT

public:
    InstallSourcesManager(QObject* parent = 0);
    ~InstallSourcesManager();

    void refreshSources();

private slots:
    void cancel();
    void percentComplete(int percent, const QString& title);

private:
    void findProgressObject();
    void runThread();

    QQuickItem* progressObject_;
    InstallSourcesThread* installSourcesThread_;
};

} // end namespace
#endif
