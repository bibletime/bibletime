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

#include "indexthread.h"

#include <QDebug>
#include <QString>
#include <QThread>
#include "backend/btinstallbackend.h"
#include "backend/managers/cswordbackend.h"
#include "util/btconnect.h"

// Sword includes:
#include <filemgr.h>


IndexThread::IndexThread(const QList<CSwordModuleInfo*>& modules, QObject* const parent)
    : QThread(parent)
    , m_modules(modules),
    m_currentModuleIndex(0),
    m_stopRequested(false) {
}

void IndexThread::run() {
    m_stopRequestedMutex.lock();
    try {
        for (m_currentModuleIndex = 0;
             !m_stopRequested && (m_currentModuleIndex < m_modules.size());
             m_currentModuleIndex++)
        {
            m_stopRequestedMutex.unlock();
            indexModule();
            m_stopRequestedMutex.lock();
        }
        emit indexingFinished();
    } catch (...) {
        m_stopRequestedMutex.unlock();
        throw;
    }
    m_stopRequestedMutex.unlock();
}

void IndexThread::stopIndex() {
    const QMutexLocker lock(&m_stopRequestedMutex);
    m_stopRequested = true;
    CSwordModuleInfo* module = m_modules.at(m_currentModuleIndex);
    module->cancelIndexing();
}

void IndexThread::indexModule() {
    CSwordModuleInfo* module = m_modules.at(m_currentModuleIndex);
    QString moduleName = module->name();
    emit beginIndexingModule(moduleName);
    BT_CONNECT(module, SIGNAL(indexingProgress(int)),
               this,   SLOT(slotModuleProgress(int)));
    bool success = true;
    try {
        module->buildIndex();
    }
    catch (...) {
        success = false;
    }
    emit endIndexingModule(moduleName, success);
}

void IndexThread::slotModuleProgress(int percentComplete) {
    emit indexingProgress(percentComplete);
}
