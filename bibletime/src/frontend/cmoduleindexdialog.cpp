/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "cmoduleindexdialog.h"
#include "cmoduleindexdialog.moc"

#include "util/cpointers.h"
#include "backend/managers/cswordbackend.h"

#include <boost/scoped_ptr.hpp>

//Qt includes
#include <QString>
#include <QProgressDialog>
#include <QDebug>
#include <QApplication>
#include <QMessageBox>

CModuleIndexDialog* CModuleIndexDialog::getInstance() {
	qDebug("CModuleIndexDialog::getInstance");
	static CModuleIndexDialog* instance = 0;
	if (instance == 0) {
		instance = new CModuleIndexDialog();
	}
	qDebug("CModuleIndexDialog::getInstance end");
	return instance;
}

void CModuleIndexDialog::indexAllModules( const ListCSwordModuleInfo& modules )
{
	static bool indexing = false;
	if (!indexing) {
		indexing = true;
		if (modules.count() < 1) return;
		
		m_currentModuleIndex = 0;
		m_progress = new QProgressDialog(QString(""), tr("Cancel"), 0, modules.count()*100);
		m_progress->setWindowModality(Qt::WindowModal); // not useful actually, should have parent for this
		m_progress->setWindowTitle(tr("Creating indices"));
		m_progress->show();
		m_progress->raise();
		
		foreach (CSwordModuleInfo* info, modules) {
			//TODO: how to cancel
			//QObject::connect(CPointers::backend(), SIGNAL(sigSwordSetupChanged()), this, SLOT(swordSetupChanged()));
			connect(this, SIGNAL(sigCancel()), info, SLOT(cancelIndexing()) );
			connect(m_progress, SIGNAL(canceled()), info, SLOT(cancelIndexing()));
			connect(info, SIGNAL(indexingFinished()), this, SLOT(slotFinished()));
			connect(info, SIGNAL(indexingProgress(int)), this, SLOT(slotModuleProgress(int)) );
			QString modname(info->name());
			const QString labelText = tr("Creating index for work: ").append(modname);
			m_progress->setLabelText(labelText);
			//todo: if we want to cancel indexing from
			info->buildIndex(); //waits until this module is finished
	
			m_currentModuleIndex++;
			disconnect(m_progress, SIGNAL(canceled()), info, SLOT(cancelIndexing()));
			disconnect(info, SIGNAL(indexingFinished()), this, SLOT(slotFinished()));
			disconnect(info, SIGNAL(indexingProgress(int)), this, SLOT(slotModuleProgress(int)) );
			if (m_progress->wasCanceled()) break;
		}
	
		delete m_progress;
		m_progress = 0;
		indexing = false;
	}
}

void CModuleIndexDialog::indexUnindexedModules( const ListCSwordModuleInfo& modules ) {
	ListCSwordModuleInfo unindexedMods;
	
	ListCSwordModuleInfo::const_iterator end_it = modules.end();
	for( ListCSwordModuleInfo::const_iterator it = modules.begin(); it != end_it; ++it) {
		if ((*it)->hasIndex()) {
			continue;
		}

		unindexedMods << (*it);
	}
	indexAllModules(unindexedMods);
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
void CModuleIndexDialog::slotSwordSetupChanged()
{
	qDebug("CModuleIndexDialog::slotSwordSetupChanged, TODO: cancel if modules are removed");
	QMessageBox::information(0, tr("Indexing Is Cancelled"), tr("Indexing is cancelled because modules are removed."));
	emit sigCancel();
}
