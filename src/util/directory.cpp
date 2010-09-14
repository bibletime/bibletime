/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "util/directory.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QLocale>


namespace util {
namespace directory {

namespace {

QDir cachedIconDir;
QDir cachedJavascriptDir;
QDir cachedLicenseDir;
QDir cachedPicsDir;
QDir cachedLocaleDir;
QDir cachedHandbookDir;
QDir cachedHowtoDir;
QDir cachedDisplayTemplatesDir;
QDir cachedUserDisplayTemplatesDir;
QDir cachedUserBaseDir;
QDir cachedUserHomeDir;
QDir cachedUserHomeSwordDir;
QDir cachedUserHomeSwordModsDir;
QDir cachedUserSessionsDir;
QDir cachedUserCacheDir;
QDir cachedUserIndexDir;
#ifdef Q_WS_WIN
QDir cachedApplicationSwordDir; // Only Windows installs the sword directory which contains locales.d
QDir cachedSharedSwordDir;
#endif

#ifdef Q_WS_WIN
static const char* BIBLETIME = "Bibletime";
static const char* SWORD_DIR = "Sword";
#else
static const char* BIBLETIME =".bibletime";
static const char* SWORD_DIR = ".sword";
#endif
} // anonymous namespace

bool initDirectoryCache() {
    QDir wDir(QCoreApplication::applicationDirPath());
    wDir.makeAbsolute();

    if (!wDir.cdUp()) { // Installation prefix
        qWarning() << "Cannot cd up from directory " << QCoreApplication::applicationDirPath();
        return false;
    }

#ifdef Q_WS_WIN

    cachedApplicationSwordDir = wDir; // application sword dir for Windows only
    if (!cachedApplicationSwordDir.cd("share/sword") || !cachedApplicationSwordDir.isReadable()) {
        qWarning() << "Cannot find sword directory relative to" << QCoreApplication::applicationDirPath();
        return false;
    }

	cachedSharedSwordDir = QDir(getenv("ALLUSERSPROFILE")); // sword dir for Windows only
    if (!cachedSharedSwordDir.cd("Application Data")) {
        qWarning() << "Cannot find ALLUSERSPROFILE\\Application Data";
        return false;
	}
    if (!cachedSharedSwordDir.cd(SWORD_DIR)) {
        if (!cachedSharedSwordDir.mkdir(SWORD_DIR) || !cachedSharedSwordDir.cd(SWORD_DIR)) {
			qWarning() << "Cannot find ALLUSERSPROFILE\\Application Data\\Sword";
			return false;
		}
    }
#endif

    cachedIconDir = wDir; // Icon dir
    if (!cachedIconDir.cd("share/bibletime/icons") || !cachedIconDir.isReadable()) {
        qWarning() << "Cannot find icon directory relative to" << QCoreApplication::applicationDirPath();
        return false;
    }

    cachedJavascriptDir = wDir;
    if (!cachedJavascriptDir.cd("share/bibletime/javascript") || !cachedJavascriptDir.isReadable()) {
        qWarning() << "Cannot find javascript directory relative to" << QCoreApplication::applicationDirPath();
        return false;
    }

    cachedLicenseDir = wDir;
    if (!cachedLicenseDir.cd("share/bibletime/license") || !cachedLicenseDir.isReadable()) {
        qWarning() << "Cannot find license directory relative to" << QCoreApplication::applicationDirPath();
        return false;
    }

    cachedPicsDir = wDir; //icon dir
    if (!cachedPicsDir.cd("share/bibletime/pics") || !cachedPicsDir.isReadable()) {
        qWarning() << "Cannot find icon directory relative to" << QCoreApplication::applicationDirPath();
        return false;
    }

    cachedLocaleDir = wDir;
    if (!cachedLocaleDir.cd("share/bibletime/locale")) {
        qWarning() << "Cannot find locale directory relative to" << QCoreApplication::applicationDirPath();
        return false;
    }

    QString localeName(QLocale::system().name());
    QString langCode(localeName.section('_', 0, 0));

    cachedHandbookDir = wDir;
    if (!cachedHandbookDir.cd("share/bibletime/docs/handbook/" + localeName)) {
        if (!cachedHandbookDir.cd("share/bibletime/docs/handbook/" + langCode)) {
            if (!cachedHandbookDir.cd("share/bibletime/docs/handbook/en/")) {
                qWarning() << "Cannot find handbook directory relative to" << QCoreApplication::applicationDirPath();
                return false;
            }
        }
    }

    cachedHowtoDir = wDir;
    if (!cachedHowtoDir.cd("share/bibletime/docs/howto/" + localeName)) {
        if (!cachedHowtoDir.cd("share/bibletime/docs/howto/" + langCode)) {
            if (!cachedHowtoDir.cd("share/bibletime/docs/howto/en/")) {
                qWarning() << "Cannot find handbook directory relative to" << QCoreApplication::applicationDirPath();
                return false;
            }
        }
    }

    cachedDisplayTemplatesDir = wDir; //display templates dir
    if (!cachedDisplayTemplatesDir.cd("share/bibletime/display-templates/")) {
        qWarning() << "Cannot find display template directory relative to" << QCoreApplication::applicationDirPath();
        return false;
    }

    cachedUserHomeDir = QDir(getenv("HOME"));

    cachedUserBaseDir = cachedUserHomeDir;
    if (!cachedUserBaseDir.cd(BIBLETIME)) {
        if (!cachedUserBaseDir.mkdir(BIBLETIME) || !cachedUserBaseDir.cd(BIBLETIME)) {
            qWarning() << "Could not create user setting directory.";
            return false;
        }
    }

    cachedUserHomeSwordDir = cachedUserHomeDir;
    if (!cachedUserHomeSwordDir.cd(SWORD_DIR)) {
        if (!cachedUserHomeSwordDir.mkdir(SWORD_DIR) || !cachedUserHomeSwordDir.cd(SWORD_DIR)) {
            qWarning() << "Could not create user home " << SWORD_DIR << " directory.";
            return false;
        }
    }

    cachedUserHomeSwordModsDir = cachedUserHomeSwordDir;
    if (!cachedUserHomeSwordModsDir.cd("mods.d")) {
        if (!cachedUserHomeSwordModsDir.mkdir("mods.d") || !cachedUserHomeSwordModsDir.cd("mods.d")) {
            qWarning() << "Could not create user home " << SWORD_DIR << " mods.d directory.";
            return false;
        }
    }

    cachedUserSessionsDir = cachedUserBaseDir;
    if (!cachedUserSessionsDir.cd("sessions")) {
        if (!cachedUserSessionsDir.mkdir("sessions") || !cachedUserSessionsDir.cd("sessions")) {
            qWarning() << "Could not create user sessions directory.";
            return false;
        }
    }

    cachedUserCacheDir = cachedUserBaseDir;
    if (!cachedUserCacheDir.cd("cache")) {
        if (!cachedUserCacheDir.mkdir("cache") || !cachedUserCacheDir.cd("cache")) {
            qWarning() << "Could not create user cache directory.";
            return false;
        }
    }

    cachedUserIndexDir = cachedUserBaseDir;
    if (!cachedUserIndexDir.cd("indices")) {
        if (!cachedUserIndexDir.mkdir("indices") || !cachedUserIndexDir.cd("indices")) {
            qWarning() << "Could not create user indices directory.";
        }
    }

    cachedUserDisplayTemplatesDir = cachedUserBaseDir;
    if (!cachedUserDisplayTemplatesDir.cd("display-templates")) {
        if (!cachedUserDisplayTemplatesDir.mkdir("display-templates") || !cachedUserDisplayTemplatesDir.cd("display-templates")) {
            qWarning() << "Could not create user display templates directory.";
        }
    }

    return true;
} // bool initDirectoryCache();

void removeRecursive(const QString &dir) {
    //Check for validity of argument
    if (dir.isEmpty()) return;
    QDir d(dir);
    if (!d.exists()) return;

    //remove all files in this dir
    d.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
    const QFileInfoList fileList = d.entryInfoList();
    for (QFileInfoList::const_iterator it_file = fileList.begin(); it_file != fileList.end(); it_file++) {
        d.remove( it_file->fileName() );
    }

    //remove all subdirs recursively
    d.setFilter( QDir::Dirs | QDir::NoSymLinks );
    const QFileInfoList dirList = d.entryInfoList();
    for (QFileInfoList::const_iterator it_dir = dirList.begin(); it_dir != dirList.end(); it_dir++) {
        if ( !it_dir->isDir() || it_dir->fileName() == "." || it_dir->fileName() == ".." ) {
            continue;
        }
        removeRecursive( it_dir->absoluteFilePath() );
    }
    d.rmdir(dir);
}

/** Returns the size of the directory including the size of all it's files and it's subdirs.
 */
unsigned long getDirSizeRecursive(const QString &dir) {
    //Check for validity of argument
    QDir d(dir);
    if (!d.exists()) return 0;

    unsigned long size = 0;

    //First get the size of all files int this folder
    d.setFilter(QDir::Files);
    const QFileInfoList infoList = d.entryInfoList();
    for (QFileInfoList::const_iterator it = infoList.begin(); it != infoList.end(); it++) {
        size += it->size();
    }

    //Then add the sizes of all subdirectories
    d.setFilter(QDir::Dirs);
    const QFileInfoList dirInfoList = d.entryInfoList();
    for (QFileInfoList::const_iterator it_dir = dirInfoList.begin(); it_dir != dirInfoList.end(); it_dir++) {
        if ( !it_dir->isDir() || it_dir->fileName() == "." || it_dir->fileName() == ".." ) {
            continue;
        }
        size += getDirSizeRecursive( it_dir->absoluteFilePath() );
    }
    return size;
}

/**Recursively copies a directory, overwriting existing files*/
void copyRecursive(const QString &src, const QString &dest) {
    QDir srcDir(src);
    QDir destDir(dest);
    //Copy files
    QStringList files = srcDir.entryList(QDir::Files);
    for (QStringList::iterator it = files.begin(); it != files.end(); ++it) {
        QFile currFile(src + "/" + *it);
        QString newFileLoc = dest + "/" + *it;
        QFile newFile(newFileLoc);
        newFile.remove();
        currFile.copy(newFileLoc);
    }
    QStringList dirs = srcDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (QStringList::iterator it = dirs.begin(); it != dirs.end(); ++it) {
        QString temp = *it;
        if (!destDir.cd(*it)) {
            destDir.mkdir(*it);
        }
        copyRecursive(src + "/" + *it, dest + "/" + *it);
    }
}

QString convertDirSeparators(const QString& path)
{
	QString result = path;
#ifdef Q_WS_WIN
	result.replace("/", "\\");
#else
	result.replace("\\", "/");
#endif
	return result;
}

#ifdef Q_WS_WIN
QDir getApplicationSwordDir() {
    return cachedApplicationSwordDir;
}

QDir getSharedSwordDir() {
    return cachedSharedSwordDir;
}

#endif

QDir getIconDir() {
    return cachedIconDir;
}

QDir getJavascriptDir() {
    return cachedJavascriptDir;
}

QDir getLicenseDir() {
    return cachedLicenseDir;
}

QIcon getIcon(const QString &name) {
    static QMap<QString, QIcon> iconCache;
    //error if trying to use name directly...
    QString name2(name);
    QString plainName = name2.remove(".svg", Qt::CaseInsensitive);
    if (iconCache.contains(plainName)) {
        return iconCache.value(plainName);
    }

    QString iconDir = getIconDir().canonicalPath();
    QString iconFileName = iconDir + "/" + plainName + ".svg";
    if (QFile(iconFileName).exists()) {
        QIcon ic = QIcon(iconFileName);
        iconCache.insert(plainName, ic);
        return ic;
    }
    else {
        iconFileName = iconDir + "/" + plainName + ".png";
        if (QFile(iconFileName).exists()) {
            QIcon ic = QIcon(iconFileName);
            iconCache.insert(plainName, ic);
            return ic;
        }
        else {
            qWarning() << "Cannot find icon file" << iconFileName << ", using default icon.";
            iconFileName = iconDir + "/" + "/default.svg";
            if (QFile(iconFileName).exists()) {
                return QIcon(iconDir + "/default.svg");
            }
            else {
                return QIcon(iconDir + "default.png");
            }
        }
    }
}

QDir getPicsDir() {
    return cachedPicsDir;
}

QDir getLocaleDir() {
    return cachedLocaleDir;
}

QDir getHandbookDir() {
    return cachedHandbookDir;
}

QDir getHowtoDir() {
    return cachedHowtoDir;
}

QDir getDisplayTemplatesDir() {
    return cachedDisplayTemplatesDir;
}

QDir getUserBaseDir() {
    return cachedUserBaseDir;
}

QDir getUserHomeDir() {
    return cachedUserHomeDir;
}

QDir getUserHomeSwordDir() {
    return cachedUserHomeSwordDir;
}

QDir getUserHomeSwordModsDir() {
    return cachedUserHomeSwordModsDir;
}

QDir getUserSessionsDir() {
    return cachedUserSessionsDir;
}

QDir getUserCacheDir() {
    return cachedUserCacheDir;
}

QDir getUserIndexDir() {
    return cachedUserIndexDir;
}

QDir getUserDisplayTemplatesDir() {
    return cachedUserDisplayTemplatesDir;
}

} // namespace directory
} // namespace util
