/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btsourcesthread.h"

#include <cstddef>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <memory>
#include <utility>
#include "../util/btassert.h"
#include "btinstallbackend.h"
#include "btinstallmgr.h"

// Sword includes:
#include <installmgr.h>


void BtSourcesThread::run() {
    Q_EMIT percentComplete(0);
    Q_EMIT showMessage(tr("Getting Library List"));
    if (BtInstallMgr().refreshRemoteSourceConfiguration())
        qWarning("InstallMgr: getting remote list returned an error.");
    Q_EMIT percentComplete(10);

    if (shouldStop()) {
        Q_EMIT showMessage(tr("Updating stopped"));
        return;
    }

    QStringList const sourceNames = BtInstallBackend::sourceNameList();
    auto const sourceCount = sourceNames.count();
    BT_ASSERT(sourceCount >= 0);
    auto const failedSources(std::make_unique<int[]>(sourceCount));
    std::size_t numFailedSources = 0u;
    BtInstallMgr iMgr;
    for (int i = 0; i < sourceCount; ++i) {
        if (shouldStop()) {
            Q_EMIT showMessage(tr("Updating stopped"));
            return;
        }
        QString const & sourceName = sourceNames[i];
        Q_EMIT showMessage(
                    tr("Updating remote library \"%1\"").arg(sourceName));
        {
            sword::InstallSource source = BtInstallBackend::source(sourceName);
            if (iMgr.refreshRemoteSource(&source)) {
                failedSources[numFailedSources] = i;
                ++numFailedSources;
            }
        }
        Q_EMIT percentComplete(
                    static_cast<int>(10 + 90 * ((i + 1.0) / sourceCount)));
    }
    Q_EMIT percentComplete(100);
    if (numFailedSources <= 0) {
        Q_EMIT showMessage(tr("Remote libraries have been updated."));
        m_finishedSuccessfully.store(true, std::memory_order_release);
    } else {
        QString msg = tr("The following remote libraries failed to update: ");
        for (std::size_t i = 0;;) {
            msg += sourceNames[failedSources[i]];
            if (++i >= numFailedSources)
                break;
            static auto const separator = QStringLiteral(", ");
            msg.append(separator);
        }
        Q_EMIT showMessage(std::move(msg));
        m_finishedSuccessfully.store(true, std::memory_order_release);
    }
}
