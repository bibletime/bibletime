/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
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
#include <QLocale>

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

/**Recursively copies a directory, overwriting existing files*/
void DirectoryUtil::copyRecursive(QString src, QString dest){
	QDir srcDir(src);
	QDir destDir(dest);
	//Copy files
	QStringList files = srcDir.entryList(QDir::Files);
	for (QStringList::iterator it = files.begin(); it != files.end(); ++it){
		QFile currFile(src + "/" + *it);
		QString newFileLoc = dest + "/" + *it;
		QFile newFile(newFileLoc);
		newFile.remove();
		currFile.copy(newFileLoc);
	}
	QStringList dirs = srcDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (QStringList::iterator it = dirs.begin(); it != dirs.end(); ++it){
		QString temp = *it;
		if (!destDir.cd(*it)){
			destDir.mkdir(*it);
		}
		copyRecursive(src + "/" + *it, dest + "/" + *it);
	}
}

static QDir cachedIconDir;
static QDir cachedPicsDir;
static QDir cachedLocaleDir;
static QDir cachedHandbookDir;
static QDir cachedHowtoDir;
static QDir cachedDisplayTemplatesDir;
static QDir cachedUserDisplayTemplatesDir;
static QDir cachedUserBaseDir;
static QDir cachedUserHomeDir;
static QDir cachedUserSessionsDir;
static QDir cachedUserCacheDir;
static QDir cachedUserIndexDir;

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

	cachedLocaleDir = wDir;
	if (!cachedLocaleDir.cd("share/bibletime/locale")) {
		qWarning() << "Cannot find locale directory relative to" << QCoreApplication::applicationDirPath();
		throw;
	}
	
	QString localeName = QLocale::system().name();
	QString langCode = localeName.section('_', 0, 0);
	
	cachedHandbookDir = wDir;
	if (!cachedHandbookDir.cd(QString("share/bibletime/docs/handbook/") + localeName)) {
		if (!cachedHandbookDir.cd(QString("share/bibletime/docs/handbook/") + langCode)) {
			if (!cachedHandbookDir.cd("share/bibletime/docs/handbook/en/")) {
				qWarning() << "Cannot find handbook directory relative to" << QCoreApplication::applicationDirPath();
				throw;
			}
		}
	}

	cachedHowtoDir = wDir;
	if (!cachedHowtoDir.cd(QString("share/bibletime/docs/howto/") + localeName)) {
		if (!cachedHowtoDir.cd(QString("share/bibletime/docs/howto/") + langCode)) {
			if (!cachedHowtoDir.cd("share/bibletime/docs/howto/en/")) {
				qWarning() << "Cannot find handbook directory relative to" << QCoreApplication::applicationDirPath();
				throw;
			}
		}
	}

	cachedDisplayTemplatesDir = wDir; //display templates dir
	if (!cachedDisplayTemplatesDir.cd("share/bibletime/display-templates/")) {
		qWarning() << "Cannot find display template directory relative to" << QCoreApplication::applicationDirPath();
		throw;
	}

	cachedUserHomeDir = QDir::home();

	cachedUserBaseDir = QDir::home();
	if (!cachedUserBaseDir.cd(".bibletime")){
		bool success = cachedUserBaseDir.mkdir(".bibletime") && cachedUserBaseDir.cd(".bibletime");
		if (!success){
			qWarning() << "Could not create user setting directory.";
			throw;
		}
	}

	cachedUserSessionsDir = cachedUserBaseDir;
	if (!cachedUserSessionsDir.cd("sessions")){
		bool success = cachedUserSessionsDir.mkdir("sessions") && cachedUserSessionsDir.cd("sessions");
		if (!success){
			qWarning() << "Could not create user sessions directory.";
			throw;
		}
	}

	cachedUserCacheDir = cachedUserBaseDir;
	if (!cachedUserCacheDir.cd("cache")){
		bool success = cachedUserCacheDir.mkdir("cache") && cachedUserCacheDir.cd("cache");
		if (!success){
			qWarning() << "Could not create user cache directory.";
			throw;
		}
	}

	cachedUserIndexDir = cachedUserBaseDir;
	if (!cachedUserIndexDir.cd("indices")){
		bool success = cachedUserIndexDir.mkdir("indices") && cachedUserIndexDir.cd("indices");
		if (!success){
			qWarning() << "Could not create user indices directory.";
		}
	}
	
	cachedUserDisplayTemplatesDir = cachedUserBaseDir;
	if (!cachedUserDisplayTemplatesDir.cd("display-templates")){
		bool success = cachedUserDisplayTemplatesDir.mkdir("display-templates") && cachedUserDisplayTemplatesDir.cd("display-templates");
		if (!success){
			qWarning() << "Could not create user display templates directory.";
		}
	}

	dirCacheInitialized = true;
}

QDir DirectoryUtil::getIconDir(void)
{ 
	if (!dirCacheInitialized) initDirectoryCache();
	return cachedIconDir; 
}

QIcon DirectoryUtil::getIcon(const QString& name)
{
	static QMap<QString, QIcon> iconCache;

	if (iconCache.contains(name)) {
		return iconCache.value(name);
	}

	QString iconDir = getIconDir().canonicalPath();
	QString iconFileName = iconDir + "/" + name;
	if (QFile(iconFileName).exists())
	{
		QIcon ic = QIcon(iconFileName);
		iconCache.insert(name, ic);
		return ic;
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

QDir DirectoryUtil::getLocaleDir(void)
{
	if (!dirCacheInitialized) initDirectoryCache();
	return cachedLocaleDir;
}

QDir DirectoryUtil::getHandbookDir(void)
{
 	if (!dirCacheInitialized) initDirectoryCache();
 	return cachedHandbookDir;
}

QDir DirectoryUtil::getHowtoDir(void)
{
 	if (!dirCacheInitialized) initDirectoryCache();
 	return cachedHowtoDir;
}

QDir DirectoryUtil::getDisplayTemplatesDir(void)
{
	if (!dirCacheInitialized) initDirectoryCache();
	return cachedDisplayTemplatesDir;
}

QDir DirectoryUtil::getUserBaseDir(void)
{
	if (!dirCacheInitialized) initDirectoryCache();
	return cachedUserBaseDir;
}

QDir DirectoryUtil::getUserHomeDir(void)
{
	if (!dirCacheInitialized) initDirectoryCache();
	return cachedUserHomeDir;
}

QDir DirectoryUtil::getUserSessionsDir(void)
{
	if (!dirCacheInitialized) initDirectoryCache();
	return cachedUserSessionsDir;
}

QDir DirectoryUtil::getUserCacheDir(void)
{
	if (!dirCacheInitialized) initDirectoryCache();
	return cachedUserCacheDir;
}

QDir DirectoryUtil::getUserIndexDir(void)
{
	if (!dirCacheInitialized) initDirectoryCache();
	return cachedUserIndexDir;
}

QDir DirectoryUtil::getUserDisplayTemplatesDir(void)
{
	if (!dirCacheInitialized) initDirectoryCache();
	return cachedUserDisplayTemplatesDir;
}


} //end of namespace util::filesystem

} //end of namespace util

