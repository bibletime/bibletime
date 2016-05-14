/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef INSTALL_SOURCES_MANAGER_H
#define INSTALL_SOURCES_MANAGER_H

#include <QObject>
#include <QQuickItem>

namespace btm {

class InstallSources;

class InstallSourcesManager :public QObject {
    Q_OBJECT

public:
    InstallSourcesManager(QObject* parent = nullptr);
    ~InstallSourcesManager();

    void refreshSources();

signals:
    void sourcesUpdated();

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
