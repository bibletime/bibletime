/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "installsources.h"

#include "backend/btinstallbackend.h"
#include "backend/btinstallmgr.h"
#include <QDebug>

namespace btm {

InstallSources::InstallSources(QObject *parent)
    : QObject(parent),
      done(false),
      m_cancelled(false) {
    m_iMgr = new BtInstallMgr();
}


InstallSources::~InstallSources() {
    delete m_iMgr;
}

void InstallSources::process() {

    m_canceled = false;
    refreshSourceList();
    if (m_canceled)
            return;
    QStringList sourceNames = BtInstallBackend::sourceNameList();
    refreshWorks(sourceNames);
    emit finished();
}

void InstallSources::refreshSourceList() {
    int ret = m_iMgr->refreshRemoteSourceConfiguration();
    if (ret ) {
        qWarning("InstallMgr: getting remote list returned an error.");
    }
}

void InstallSources::refreshWorks(const QStringList& sourceNames) {
    int sourceCount = sourceNames.count();
    for (int i=0; i<sourceCount; ++i) {
        if (m_canceled)
            break;
        QString sourceName = sourceNames.at(i);
        int percent = 10 + 90 *((double)i/sourceCount);
        QString title = tr("Refreshing") + " " + sourceName;
        emit percentComplete(percent, title);
        sword::InstallSource source = BtInstallBackend::source(sourceName);
        bool result = (m_iMgr->refreshRemoteSource(&source) == 0);
        if (result) {
            ;
        } else {
            QString error = QString(tr("Failed to refresh source %1")).arg(sourceName);
            qDebug() << error;
        }
    }
    emit percentComplete(100, "Done");
}

void InstallSources::cancel() {
    m_canceled = true;
}

}
