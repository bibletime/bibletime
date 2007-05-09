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
#include <QDir>
#include <QFileInfoList>

namespace util {

namespace filesystem {

void DirectoryUtil::removeRecursive(const QString& dir) {
	//qWarning("removeRecursive(%s)", dir.latin1());
	if (dir == QString::null)  {
		return;
	}
	
	QDir d(dir);
	if (!d.exists()) {
		return;
	}

	QFileInfo fi;
	
	//remove all files in this dir
	d.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );

	const QFileInfoList fileList = d.entryInfoList();
	QFileInfoList::const_iterator it_file;// = fileList.iterator()();

	for (it_file = fileList.begin(); it_file != fileList.end(); it_file++)
	{
		fi = (*it_file);
		d.remove( fi.fileName() );
	}
	
	//remove all subdirs recursively
	d.setFilter( QDir::Dirs | QDir::NoSymLinks );
	const QFileInfoList dirList = d.entryInfoList();
	QFileInfoList::const_iterator it_dir;
	
	for (it_dir = dirList.begin(); it_dir != dirList.end(); it_dir++)
	{
		fi = (*it_dir);
		if ( !fi.isDir() || fi.fileName() == "." || fi.fileName() == ".." ) {
			continue;
		}
		//qDebug("Removing dir %s", fi->absFilePath().latin1() );
		removeRecursive( fi.absoluteFilePath() );
	}

	d.rmdir(dir);
}

/** Returns the size of the directory including the size of all it's files and it's subdirs.
 */
unsigned long DirectoryUtil::getDirSizeRecursive(const QString& dir) {
	//qWarning("Getting size for %s", dir.latin1());

	QDir d(dir);
	if (!d.exists()) return 0;	

	d.setFilter(QDir::Files);
	
	unsigned long size = 0;
	
	const QFileInfoList infoList = d.entryInfoList();
	QFileInfoList::const_iterator it;
	QFileInfo info;
	
	for (it = infoList.begin(); it != infoList.end(); it++)
	{
		info = (*it);
		size += info.size();
	}

	d.setFilter(QDir::Dirs);
	const QFileInfoList dirInfoList = d.entryInfoList();
	QFileInfoList::const_iterator it_dir;
	for (it_dir = dirInfoList.begin(); it_dir != dirInfoList.end(); it_dir++)
	{
		info = (*it_dir);
		if ( !info.isDir() || info.fileName() == "." || info.fileName() == ".." ) {
			continue;
		}
		size += getDirSizeRecursive( info.absoluteFilePath() );
	}
	return size;
}


} //end of namespace util::filesystem

} //end of namespace util

