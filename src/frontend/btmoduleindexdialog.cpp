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

#include "btmoduleindexdialog.h"

#include <array>
#include <QApplication>
#include <utility>
#include "../backend/drivers/cswordmoduleinfo.h"
#include "../util/btassert.h"
#include "../util/btconnect.h"
#include "../util/btdebugonly.h"
#include "messagedialog.h"


bool BtModuleIndexDialog::indexAllModules(const QList<CSwordModuleInfo *> &modules)
{
    if (modules.empty()) return true;

    BtModuleIndexDialog d(modules.size());
    d.show();
    d.raise();
    return d.indexAllModulesPrivate(modules);
}

BtModuleIndexDialog::BtModuleIndexDialog(int numModules)
    : QProgressDialog(tr("Preparing to index modules..."), tr("Cancel"), 0,
                      numModules * 100, nullptr),
      m_currentModuleIndex(0)
{
    setWindowTitle(tr("Creating indices"));
    setModal(true);
}

bool BtModuleIndexDialog::indexAllModulesPrivate(const QList<CSwordModuleInfo*> &modules)
{
    bool success = true;

    QList <CSwordModuleInfo*> indexedModules;
    for (auto * const m : modules) {
        BT_ASSERT(!m->hasIndex());

        /*
          Keep track of created indices to delete them on failure or
          cancellation:
        */
        indexedModules.append(m);

        std::array<QMetaObject::Connection, 3u> connections{
                BT_CONNECT(this, &BtModuleIndexDialog::canceled,
                           m /* needed */,  [m]{ m->cancelIndexing(); }),
                BT_CONNECT(m, &CSwordModuleInfo::indexingFinished,
                           this, // needed
                           [this]{
                               setValue(m_currentModuleIndex * 100 + 100);
                               qApp->processEvents();
                           }),
                BT_CONNECT(m, &CSwordModuleInfo::indexingProgress,
                           this, // needed
                           [this](int percentage) {
                               setValue(m_currentModuleIndex * 100 + percentage);
                               qApp->processEvents();
                           })};

        // Single module indexing blocks until finished:
        setLabelText(tr("Creating index for work: %1").arg(m->name()));

        try {
            m->buildIndex();
        } catch (...) {
            QString msg;
            try {
                throw;
            } catch (std::exception const & e) {
                msg = e.what();
            } catch (...) {
                msg = tr("<UNKNOWN EXCEPTION>");
            }

            message::showWarning(this,
                                 tr("Indexing aborted"),
                                 tr("An internal error occurred while building "
                                    "the index:<br/><br/>%1").arg(msg));
            success = false;
        }

        m_currentModuleIndex++;

        for (auto & connection : connections) {
            BT_DEBUG_ONLY(auto const r =) disconnect(std::move(connection));
            BT_ASSERT(r);
        }

        if (wasCanceled()) success = false;

        if (!success) break;
    }

    if (!success) {
        // Delete already created indices:
        for (auto * const m : indexedModules)
            if (m->hasIndex())
                m->deleteIndex();
    }
    return success;
}
