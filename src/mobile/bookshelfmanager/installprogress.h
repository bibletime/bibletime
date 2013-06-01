#ifndef INSTALL_PROGRESS_H
#define INSTALL_PROGRESS_H

#include <QObject>

class QQuickItem;
class CSwordModuleInfo;

namespace btm {

class InstallProgress: public QObject {
    Q_OBJECT

public:
    InstallProgress(QObject* parent = 0);

    void openProgress(const QList<CSwordModuleInfo*>& modules);

private slots:
    void cancel();

private:
    void findProgressObject();
    void makeConnections();
    void setProperties();

    QQuickItem* progressObject_;

};


}

#endif
