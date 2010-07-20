/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/cmoduleindexdialog.h"

#include <QApplication>
#include <QDebug>
#include <QProgressDialog>
#include <QString>
#include "backend/managers/cswordbackend.h"
#include "util/dialogutil.h"


CModuleIndexDialog* CModuleIndexDialog::getInstance() {
    qDebug() << "CModuleIndexDialog::getInstance";
    static CModuleIndexDialog* instance = 0;
    if (instance == 0) {
        instance = new CModuleIndexDialog();
    }
    qDebug() << "CModuleIndexDialog::getInstance end";
    return instance;
}

bool CModuleIndexDialog::indexAllModules(
        const QList<const CSwordModuleInfo*> &modules)
{
    if (modules.size() < 0) return true;

    static bool indexing = false;

    bool success = true;
    if (!indexing) {
        indexing = true;

        m_currentModuleIndex = 0;
        m_progress = new QProgressDialog(QString(""), tr("Cancel"), 0, modules.count()*100);
        m_progress->setWindowModality(Qt::WindowModal); // not useful actually, should have parent for this
        m_progress->setWindowTitle(tr("Creating indices"));
        m_progress->show();
        m_progress->raise();

        QList<CSwordModuleInfo *> indexedModules;
        Q_FOREACH(const CSwordModuleInfo *cm, modules) {
            Q_ASSERT(!cm->hasIndex());

            /// \warning const_cast
            CSwordModuleInfo *m = const_cast<CSwordModuleInfo*>(cm);
            indexedModules.append(m);

            /// \todo how to cancel
            //QObject::connect(CSwordBackend::instance()(), SIGNAL(sigSwordSetupChanged()), this, SLOT(swordSetupChanged()));
            connect(this, SIGNAL(sigCancel()), m, SLOT(cancelIndexing()) );
            connect(m_progress, SIGNAL(canceled()), m, SLOT(cancelIndexing()));
            connect(m, SIGNAL(indexingFinished()), this, SLOT(slotFinished()));
            connect(m, SIGNAL(indexingProgress(int)), this, SLOT(slotModuleProgress(int)) );
            QString modname(m->name());
            const QString labelText = tr("Creating index for work: %1").arg(modname);
            m_progress->setLabelText(labelText);

            // Single module indexing blocks until finished:
            m->buildIndex();

            m_currentModuleIndex++;
            disconnect(m_progress, SIGNAL(canceled()), m, SLOT(cancelIndexing()));
            disconnect(m, SIGNAL(indexingFinished()), this, SLOT(slotFinished()));
            disconnect(m, SIGNAL(indexingProgress(int)), this, SLOT(slotModuleProgress(int)) );
            if (m_progress->wasCanceled()) {
                success = false;
                break;
            }
        }

        delete m_progress;
        m_progress = 0;

        if (!success) {
            // Delete already created indices:
            Q_FOREACH(CSwordModuleInfo *m, indexedModules) {
                if (m->hasIndex()) {
                    m->deleteIndex();
                }
            }
        }

        indexing = false;
    }
    return success;
}

bool CModuleIndexDialog::indexUnindexedModules(
        const QList<const CSwordModuleInfo*> &modules)
{
    QList<const CSwordModuleInfo*> unindexedMods;

    QList<const CSwordModuleInfo*>::const_iterator end_it = modules.end();
    for ( QList<const CSwordModuleInfo*>::const_iterator it = modules.begin(); it != end_it; ++it) {
        if ((*it)->hasIndex()) {
            continue;
        }

        unindexedMods << (*it);
    }
    return indexAllModules(unindexedMods);
}

void CModuleIndexDialog::slotModuleProgress( int percentage ) {
    m_progress->setValue(m_currentModuleIndex * 100 + percentage);
    qApp->processEvents();
}

void CModuleIndexDialog::slotFinished( ) {
    m_progress->setValue(m_currentModuleIndex * 100 + 100);
    qApp->processEvents();
}

// Modules may be removed
void CModuleIndexDialog::slotSwordSetupChanged() {
    qDebug() << "CModuleIndexDialog::slotSwordSetupChanged"; /// \todo cancel if modules are removed
    util::showInformation(0, tr("Indexing Is Cancelled"), tr("Indexing is cancelled because modules are removed."));
    emit sigCancel();
}
