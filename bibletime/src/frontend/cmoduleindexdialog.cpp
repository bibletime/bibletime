//
// C++ Implementation: cmoduleindexdialog
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2006-2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cmoduleindexdialog.h"
#include "cmoduleindexdialog.moc"

#include "util/scoped_resource.h"

//Qt includes
#include <QString>
#include <QProgressDialog>

//KDE includes
#include <kapplication.h>
//#include <kprogressdialog.h>
#include <klocale.h>


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
	qDebug("indexAllModules");
	static bool indexing = false;
	if (!indexing) {
		indexing = true;
		qDebug() << &modules << modules << modules.count();
		//qDebug() << modules[0]->name();
		if (modules.count() < 1) {
			return;
		}
		
		m_currentModuleIndex = 0;
		m_progress = new QProgressDialog(QString(""), 0, 0, modules.count()*100);
		m_progress->setWindowModality(Qt::WindowModal); // not useful actually, should have parent for this
		m_progress->show();
		m_progress->raise();
		
		foreach (CSwordModuleInfo* info, modules) {
			info->connectIndexingFinished(this, SLOT(slotFinished()));
			info->connectIndexingProgress(this, SLOT(slotModuleProgress(int)) );
			QString modname(info->name());
			const QString labelText = QString(i18n("Creating index for work %1", modname  ));
			m_progress->setLabelText(labelText);
			info->buildIndex(); //waits until this module is finished
	
			m_currentModuleIndex++;
			info->disconnectIndexingSignals(this);
		}
	
		delete m_progress;
		m_progress = 0;
		indexing = false;
	}
}

void CModuleIndexDialog::indexUnindexedModules( const ListCSwordModuleInfo& modules ) {
	qDebug("indexUnindexedModules");
	//qDebug() << modules << modules.count();
	ListCSwordModuleInfo unindexedMods;
	
	ListCSwordModuleInfo::const_iterator end_it = modules.end();
	for( ListCSwordModuleInfo::const_iterator it = modules.begin(); it != end_it; ++it) {
		if ((*it)->hasIndex()) {
			continue;
		}

		unindexedMods << (*it);
	}
	//qDebug() << unindexedMods << unindexedMods.count();
	indexAllModules(unindexedMods);
}


/*!
    \fn CModuleIndexDialog::slotModuleProgress( int percentage )
 */
void CModuleIndexDialog::slotModuleProgress( int percentage ) {
	qDebug() << "progress:" << percentage;
	
	//progress->progressBar()->setValue( m_currentModuleIndex * 100 + percentage );
	m_progress->setValue(m_currentModuleIndex * 100 + percentage);
	//KApplication::kApplication()->processEvents(); //10 ms only; TODO: how about in qt4?
	qApp->processEvents();
}

void CModuleIndexDialog::slotFinished( ) {
	qDebug("indexing finished()");
	
	//progress->progressBar()->setValue( progress->progressBar()->maximum() - progress->progressBar()->minimum() );
	m_progress->setValue(m_progress->maximum());
	//KApplication::kApplication()->processEvents(); //1 ms only; TODO: how about in qt4?
	qApp->processEvents();
}
