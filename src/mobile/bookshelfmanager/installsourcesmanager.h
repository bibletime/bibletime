#ifndef INSTALL_SOURCES_MANAGER_H
#define INSTALL_SOURCES_MANAGER_H

#include <QObject>
#include <QQuickItem>

namespace btm {

class InstallSources;

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

    QQuickItem* m_progressObject;
    InstallSources* m_worker;
};

} // end namespace
#endif
