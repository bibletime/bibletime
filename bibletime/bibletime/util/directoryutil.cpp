//
// C++ Implementation: directoryutil
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "directoryutil.h"

//Qt includes
#include <qdir.h>

namespace util {

namespace filesystem {

void DirectoryUtil::removeRecursive(const QString& dir) {
	qWarning("removeRecursive(%s)", dir.latin1());
	if (dir == QString::null)  {
		return;
	}
	
	QDir d(dir);
	if (!d.exists()) {
		return;
	}

	QFileInfo *fi = 0;
	
	//remove all files in this dir
	d.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );

	const QFileInfoList *fileList = d.entryInfoList();
	QFileInfoListIterator it_file( *fileList );
	while ( (fi = it_file.current()) != 0 ) {
		++it_file;

		qDebug("Removing %s", fi->absFilePath().latin1() );
		d.remove( fi->fileName() ) ;
	}

	//remove all subdirs recursively
	d.setFilter( QDir::Dirs | QDir::NoSymLinks );
	const QFileInfoList *dirList = d.entryInfoList();
	QFileInfoListIterator it_dir( *dirList );
	
	while ( (fi = it_dir.current()) != 0 ) {
		++it_dir;

		if ( !fi->isDir() || fi->fileName() == "." || fi->fileName() == ".." ) {
			continue;
		}
		
		qDebug("Removing dir %s", fi->absFilePath().latin1() );
		//d.remove( fi->fileName() ) ;

		removeRecursive( fi->absFilePath() );
	}

	d.rmdir(dir);
}

/** Returns the size of the directory including the size of all it's files and it's subdirs.
 */
unsigned long DirectoryUtil::getDirSizeRecursive(const QString& dir) {
	qWarning("Getting size for %s", dir.latin1());

	QDir d(dir);
	if (!d.exists()) {
		return 0;
	}

	d.setFilter(QDir::Files);
	
	unsigned long size = 0;
	
	const QFileInfoList* infoList = d.entryInfoList();
	QFileInfoListIterator it(*infoList);
	QFileInfo* info = 0;
	while ((info = it.current()) != 0) {
		++it;

		size += info->size();
	}

	d.setFilter(QDir::Dirs);
	const QFileInfoList* dirInfoList = d.entryInfoList();
	QFileInfoListIterator it_dir(*dirInfoList);
	while ((info = it_dir.current()) != 0) {
		++it_dir;
		
		if ( !info->isDir() || info->fileName() == "." || info->fileName() == ".." ) {
			continue;
		}

		size += getDirSizeRecursive( info->absFilePath() );
	}
	
	return size;
}


} //end of namespace util::filesystem

} //end of namespace util

