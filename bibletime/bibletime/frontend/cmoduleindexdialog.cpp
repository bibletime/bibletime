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

#include "util/scoped_resource.h"

//Qt includes
#include <QString>

//KDE includes
#include <kapplication.h>
#include <kprogressdialog.h>
#include <klocale.h>

CModuleIndexDialog* CModuleIndexDialog::getInstance() {
	static CModuleIndexDialog* instance = 0;
	if (instance == 0) {
		instance = new CModuleIndexDialog();
	}

	return instance;
}

void CModuleIndexDialog::indexAllModules( const ListCSwordModuleInfo& modules ) {
	qDebug("indexAllModules");
	if (modules.count() == 0) {
		return;
	}
	
	m_currentModuleIndex = 0;
	progress = new KProgressDialog(0, i18n("Preparing instant search"), QString::null, Qt::Dialog);
	progress->setAllowCancel(false);
	progress->progressBar()->setRange(0, modules.count() * 100 );
	progress->setMinimumDuration(0);
// 	progress->show();
// 	progress->raise();

	ListCSwordModuleInfo::const_iterator end_it = modules.end();
	for( ListCSwordModuleInfo::const_iterator it = modules.begin(); it != end_it; ++it) {
		(*it)->connectIndexingFinished(this, SLOT(slotFinished()));
		(*it)->connectIndexingProgress(this, SLOT(slotModuleProgress(int)));

		progress->setLabelText(i18n("Creating index for work %1").arg((*it)->name()));
		qDebug("Building index for work %s", (*it)->name().toLatin1());
		
		(*it)->buildIndex();

		m_currentModuleIndex++;
		(*it)->disconnectIndexingSignals(this);
	}

	delete progress;
	progress = 0;
}

void CModuleIndexDialog::indexUnindexedModules( const ListCSwordModuleInfo& modules ) {
	qDebug("indexUnindexedModules");
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


/*!
    \fn CModuleIndexDialog::slotModuleProgress( int percentage )
 */
void CModuleIndexDialog::slotModuleProgress( int percentage ) {
//	qDebug("progress %d", percentage);
	
	progress->progressBar()->setValue( m_currentModuleIndex * 100 + percentage );
	KApplication::kApplication()->processEvents(); //10 ms only; TODO: how about in qt4?
}

void CModuleIndexDialog::slotFinished( ) {
	qDebug("indexing finished()");
	
	progress->progressBar()->setValue( progress->progressBar()->maximum() - progress->progressBar()->minimum() );
	KApplication::kApplication()->processEvents(); //1 ms only; TODO: how about in qt4?
}
