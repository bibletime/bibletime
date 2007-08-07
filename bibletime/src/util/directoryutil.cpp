/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "directoryutil.h"

//Qt includes
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>

#include <QDebug>

namespace util {

namespace filesystem {

void DirectoryUtil::removeRecursive(const QString dir) {
	//Check for validity of argument
	if (dir == QString::null) return;
	QDir d(dir);
	if (!d.exists()) return;

	//remove all files in this dir
	d.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
	const QFileInfoList fileList = d.entryInfoList();
	for (QFileInfoList::const_iterator it_file = fileList.begin(); it_file != fileList.end(); it_file++)
	{
		d.remove( it_file->fileName() );
	}
	
	//remove all subdirs recursively
	d.setFilter( QDir::Dirs | QDir::NoSymLinks );
	const QFileInfoList dirList = d.entryInfoList();
	for (QFileInfoList::const_iterator it_dir = dirList.begin(); it_dir != dirList.end(); it_dir++)
	{
		if ( !it_dir->isDir() || it_dir->fileName() == "." || it_dir->fileName() == ".." ) {
			continue;
		}
		removeRecursive( it_dir->absoluteFilePath() );
	}
	d.rmdir(dir);
}

/** Returns the size of the directory including the size of all it's files and it's subdirs.
 */
unsigned long DirectoryUtil::getDirSizeRecursive(const QString dir) {
	//Check for validity of argument
	QDir d(dir);
	if (!d.exists()) return 0;	

	unsigned long size = 0;
	
	//First get the size of all files int this folder
	d.setFilter(QDir::Files);
	const QFileInfoList infoList = d.entryInfoList();
	for (QFileInfoList::const_iterator it = infoList.begin(); it != infoList.end(); it++)
	{
		size += it->size();
	}

	//Then add the sizes of all subdirectories
	d.setFilter(QDir::Dirs);
	const QFileInfoList dirInfoList = d.entryInfoList();
	for (QFileInfoList::const_iterator it_dir = dirInfoList.begin(); it_dir != dirInfoList.end(); it_dir++)
	{
		if ( !it_dir->isDir() || it_dir->fileName() == "." || it_dir->fileName() == ".." ) {
			continue;
		}
		size += getDirSizeRecursive( it_dir->absoluteFilePath() );
	}
	return size;
}

static QDir cached_iconDir;

void DirectoryUtil::initDirectoryCache(QString executablePath)
{
	QDir wDir(QFileInfo(executablePath).dir());
	wDir.makeAbsolute();
	if (!wDir.cdUp() || !wDir.cd("share/bibletime/icons"))
	{
		qWarning() << "Cannot find icon directory relative to" << executablePath;
		throw; //icon dir
	}
	cached_iconDir = wDir;
}

QDir DirectoryUtil::getIconDir(void)
{ 
	return cached_iconDir; 
}

} //end of namespace util::filesystem

} //end of namespace util

