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

void CModuleIndexDialog::indexAllModules( const ListCSwordModuleInfo& modules ) {
	qDebug("indexAllModules");
	if (modules.count() == 0) {
		return;
	}
	
	m_currentModuleIndex = 0;
	//progress = new KProgressDialog(0, i18n("Preparing instant search"), QString::null, Qt::Dialog);
	//progress->setAllowCancel(false);
	//progress->progressBar()->setRange(0, modules.count() * 100 );
	//progress->setMinimumDuration(0);
	m_progress = new QProgressDialog(QString(""), 0, 0, modules.count()*100);
	//m_progress->setCancelButtonText(0);
	//m_progress->setRange(0, );
	m_progress->setLabelText(QString("1"));
	m_progress->show();
	m_progress->setLabelText(QString("2"));
// 	progress->show();
// 	progress->raise();
	qDebug("indexAllModules 1");
	ListCSwordModuleInfo::const_iterator end_it = modules.end();
	for( ListCSwordModuleInfo::const_iterator it = modules.begin(); it != end_it; ++it) {
		m_progress->setLabelText(QString("3"));
		qDebug() << *it;
		(*it)->connectIndexingFinished(this, SLOT(slotFinished()));
		//(*it)->connectIndexingProgress(this, SLOT(slotModuleProgress(int)));
		(*it)->connectIndexingProgress(this, SLOT(slotModuleProgress(int)) );
		m_progress->setLabelText(QString("4"));
		QString modname((*it)->name());
		qDebug() << "modname:" << modname;
		const QString labelText = QString(i18n("Creating index for work %1", modname  )).simplified();
		QString tempStr(labelText.toUtf8());
		m_progress->setLabelText(tempStr);
		//qDebug() << "labelText:" << labelText;
		//m_progress->setLabelText(labelText);
		
		qDebug("Building index for work %s", (*it)->name().toLatin1());
		
		(*it)->buildIndex(); //waits until this module is finished

		m_currentModuleIndex++;
		(*it)->disconnectIndexingSignals(this);
	}

	delete m_progress;
	m_progress = 0;
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
	qDebug() << "progress:" << percentage;
	
	//progress->progressBar()->setValue( m_currentModuleIndex * 100 + percentage );
	m_progress->setValue(m_currentModuleIndex * 100 + percentage);
	KApplication::kApplication()->processEvents(); //10 ms only; TODO: how about in qt4?
}

void CModuleIndexDialog::slotFinished( ) {
	qDebug("indexing finished()");
	
	//progress->progressBar()->setValue( progress->progressBar()->maximum() - progress->progressBar()->minimum() );
	m_progress->setValue(m_progress->maximum());
	KApplication::kApplication()->processEvents(); //1 ms only; TODO: how about in qt4?
}
