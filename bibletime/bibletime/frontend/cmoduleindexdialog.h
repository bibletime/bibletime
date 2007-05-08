//
// C++ Interface: cmoduleindexdialog
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CMODULEINDEXDIALOG_H
#define CMODULEINDEXDIALOG_H

#include "backend/cswordmoduleinfo.h"

//Qt includes
#include <qobject.h>

//forward declaration
class KProgressDialog;

/**
 * This dialog is used to index a list of modules and to show progress for that.\
 * While the indexing is in progress it creates a blocking, top level dialog which shows the progress
 * while the indexing is done.
 *
 * @author The BibleTime team <info@bibletime.info>
*/
class CModuleIndexDialog : public QObject {
	Q_OBJECT
public:
	/** Get the singleton instance.
	 *
	 */
	static CModuleIndexDialog* getInstance();

	/** Starts the actual indexing. It shows the dialog with progress information.
	 */
	void indexAllModules( const ListCSwordModuleInfo& modules );
	
	/** Indexes all modules in the list which don't have an index yet.
	 */
	void indexUnindexedModules( const ListCSwordModuleInfo& modules );

private:
	KProgressDialog* progress;
	int m_currentModuleIndex;
	
protected slots:
    void slotModuleProgress( int percentage );
	void slotFinished();
};

#endif
