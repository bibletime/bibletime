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
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDebug>
#include <QCoreApplication>

namespace util {

namespace filesystem {

void DirectoryUtil::removeRecursive(const QString dir) {
	//Check for validity of argument
	if (dir.isEmpty()) return;
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

static QDir cachedIconDir;
static QDir cachedPicsDir;
static QDir cachedXmlDir;
static QDir cachedLocaleDir;
static QDir cachedDocsDir;
static QDir cachedUserBaseDir;
static QDir cachedUserSessionsDir;

static bool dirCacheInitialized = false;

void DirectoryUtil::initDirectoryCache(void)
{
	QDir wDir( QCoreApplication::applicationDirPath() );
	wDir.makeAbsolute();
	
	if (!wDir.cdUp()) //installation prefix
	{
		qWarning() << "Cannot cd up from directory " << QCoreApplication::applicationDirPath();
		throw;
	}
	
	cachedIconDir = wDir; //icon dir
	if (!cachedIconDir.cd("share/bibletime/icons") || !cachedIconDir.isReadable())
	{
		qWarning() << "Cannot find icon directory relative to" << QCoreApplication::applicationDirPath();
		throw;
	}

	cachedPicsDir = wDir; //icon dir
	if (!cachedPicsDir.cd("share/bibletime/pics") || !cachedPicsDir.isReadable())
	{
		qWarning() << "Cannot find icon directory relative to" << QCoreApplication::applicationDirPath();
		throw;
	}

	cachedXmlDir = wDir; //xml dir
	if (!cachedXmlDir.cd("share/bibletime/xml")) {
		qWarning() << "Cannot find xml directory relative to" << QCoreApplication::applicationDirPath();
		throw;
	}
	
	cachedLocaleDir = wDir; //xml dir
	if (!cachedLocaleDir.cd("share/bibletime/locale")) {
		qWarning() << "Cannot find locale directory relative to" << QCoreApplication::applicationDirPath();
		throw;
	}

	cachedDocsDir = wDir; //xml dir
	if (!cachedDocsDir.cd("share/bibletime/docs/")) {
		qWarning() << "Cannot find documentation directory relative to" << QCoreApplication::applicationDirPath();
		throw;
	}

	wDir = QDir::home();
	if (!wDir.cd(".bibletime")){
		bool success = wDir.mkdir(".bibletime") && wDir.cd(".bibletime");
		if (!success){
			qWarning() << "Could not create user setting directory.";
			throw;
		}
	}

	cachedUserBaseDir = wDir;
	if (!wDir.cd("sessions")){
		bool success = wDir.mkdir("sessions") && wDir.cd("sessions");
		if (!success){
			qWarning() << "Could not create user sessions directory.";
			throw;
		}
	}

	cachedUserSessionsDir = wDir;

	dirCacheInitialized = true;
}

QDir DirectoryUtil::getIconDir(void)
{ 
	if (!dirCacheInitialized) initDirectoryCache();
	return cachedIconDir; 
}

QIcon DirectoryUtil::getIcon(const QString& name)
{
	QString iconDir = getIconDir().canonicalPath();
	QString iconFileName = iconDir + "/" + name;
	if (QFile(iconFileName).exists())
	{
		return QIcon(iconFileName);
	}
	else
	{
		qWarning() << "Cannot find icon file" << iconFileName << ", using default icon.";
		return QIcon(iconDir + "/default.svg");
	}
}

QDir DirectoryUtil::getPicsDir(void)
{ 
	if (!dirCacheInitialized) initDirectoryCache();
	return cachedPicsDir; 
}

QDir DirectoryUtil::getXmlDir(void)
{
	if (!dirCacheInitialized) initDirectoryCache();
	return cachedXmlDir;
}

QDir DirectoryUtil::getLocaleDir(void)
{
	if (!dirCacheInitialized) initDirectoryCache();
	return cachedLocaleDir;
}

QDir DirectoryUtil::getDocsDir(void)
{
	if (!dirCacheInitialized) initDirectoryCache();
	return cachedDocsDir;
}

QDir DirectoryUtil::getUserBaseDir(void)
{
	if (!dirCacheInitialized) initDirectoryCache();
	return cachedUserBaseDir;
}

QDir DirectoryUtil::getUserSessionsDir(void)
{
	if (!dirCacheInitialized) initDirectoryCache();
	return cachedUserSessionsDir;
}

} //end of namespace util::filesystem

} //end of namespace util

