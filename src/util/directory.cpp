/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "directory.h"

#include <memory>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QLocale>
#include <QStandardPaths>
#include "btassert.h"
#ifdef Q_OS_WIN32
#include <windows.h>
#endif


namespace util {
namespace directory {

namespace {

std::unique_ptr<QDir> cachedIconDir;
std::unique_ptr<QDir> cachedLicenseDir;
std::unique_ptr<QDir> cachedPicsDir;
std::unique_ptr<QDir> cachedLocaleDir;
std::unique_ptr<QDir> cachedHandbookDir;
std::unique_ptr<QDir> cachedHowtoDir;
std::unique_ptr<QDir> cachedDisplayTemplatesDir;
std::unique_ptr<QDir> cachedUserDisplayTemplatesDir;
std::unique_ptr<QDir> cachedUserBaseDir;
std::unique_ptr<QDir> cachedUserHomeDir;
std::unique_ptr<QDir> cachedUserHomeSwordDir;
std::unique_ptr<QDir> cachedUserHomeSwordModsDir;
std::unique_ptr<QDir> cachedUserSessionsDir;
std::unique_ptr<QDir> cachedUserCacheDir;
std::unique_ptr<QDir> cachedUserIndexDir;
std::unique_ptr<QDir> cachedSwordPathDir;
#ifdef Q_OS_WIN
std::unique_ptr<QDir> cachedApplicationSwordDir; // Only Windows installs the sword directory which contains locales.d
std::unique_ptr<QDir> cachedSharedSwordDir;
#endif

#ifdef Q_OS_MACOS
std::unique_ptr<QDir> cachedSwordLocalesDir;
#endif

#ifdef Q_OS_ANDROID
std::unique_ptr<QDir> cachedSharedSwordDir;  // Directory that AndBible uses
static const char AND_BIBLE[] = "/sdcard/Android/data/net.bible.android.activity/files";
#endif

#if defined Q_OS_WIN || defined Q_OS_SYMBIAN
static const char BIBLETIME[] = "Bibletime";
static const char SWORD_DIR[] = "Sword";
#else
#ifdef Q_OS_MAC
static const char BIBLETIME[] = "Library/Application Support/BibleTime";
static const char SWORD_DIR[] = "Library/Application Support/Sword";
#else
static const char BIBLETIME[] = ".bibletime";
static const char SWORD_DIR[] = ".sword";
#endif
#endif
static const char SWORD_PATH[] = "SWORD_PATH";
} // anonymous namespace

bool initDirectoryCache() {
    QDir wDir(":/share/bibletime"); // check if resources would be read from qrc
    if(wDir.exists())
        wDir.setPath(":/");
    else {
        wDir.setPath(QCoreApplication::applicationDirPath());
        wDir.makeAbsolute();
        if (!wDir.cdUp()) { // Installation prefix
            qWarning() << "Cannot cd up from directory " << QCoreApplication::applicationDirPath();
            return false;
        }
    }

#ifdef Q_OS_ANDROID
    cachedSharedSwordDir.reset(new QDir());
    if (!cachedSharedSwordDir->cd(AND_BIBLE)) {
        cachedSharedSwordDir->mkpath(AND_BIBLE);
        cachedSharedSwordDir->cd(AND_BIBLE);
    }
#endif


#ifdef Q_OS_WIN
    cachedApplicationSwordDir.reset(new QDir(wDir)); // application sword dir for Windows only
    if (!cachedApplicationSwordDir->cd("share/sword") || !cachedApplicationSwordDir->isReadable()) {
        qWarning() << "Cannot find sword directory relative to" << QCoreApplication::applicationDirPath();
        return false;
    }
    QByteArray programDataDir = qgetenv("ProgramData");
    cachedSharedSwordDir.reset(new QDir(programDataDir)); // sword dir for Windows only
    if (!cachedSharedSwordDir->cd(SWORD_DIR)) {
        if (!cachedSharedSwordDir->mkdir(SWORD_DIR) || !cachedSharedSwordDir->cd(SWORD_DIR)) {
            qWarning() << "Cannot find " << programDataDir << " \\Sword";
            return false;
        }
    }
#endif

#ifdef Q_OS_MACOS
    cachedSwordLocalesDir.reset(new QDir(wDir)); // application sword dir for Windows only
    if (!cachedSwordLocalesDir->cd("share/sword/locales.d") || !cachedSwordLocalesDir->isReadable()) {
        qWarning() << "Cannot find sword locales directory relative to" << QCoreApplication::applicationDirPath();
        return false;
    }
#endif

#ifdef Q_OS_WINCE
    cachedSwordPathDir.reset(new QDir(wDir));
#elif defined (ANDROID)
#else
    cachedSwordPathDir.reset(new QDir());
    auto swordPath(qgetenv(SWORD_PATH));
    if (swordPath.data()) {
        cachedSwordPathDir.reset(new QDir(swordPath.data()));
        // We unset the SWORD_PATH so libsword finds paths correctly
        qputenv(SWORD_PATH, "");
    }
#endif

    cachedIconDir.reset(new QDir(wDir)); // Icon dir
    if (!cachedIconDir->cd("share/bibletime/icons") || !cachedIconDir->isReadable()) {
        qWarning() << "Cannot find icon directory relative to" << wDir.absolutePath();
        return false;
    }

    cachedLicenseDir.reset(new QDir(wDir));
    if (!cachedLicenseDir->cd("share/bibletime/license") || !cachedLicenseDir->isReadable()) {
        qWarning() << "Cannot find license directory relative to" << wDir.absolutePath();
        return false;
    }

    cachedPicsDir.reset(new QDir(wDir));
    if (!cachedPicsDir->cd("share/bibletime/pics") || !cachedPicsDir->isReadable()) {
        qWarning() << "Cannot find pics directory relative to" << wDir.absolutePath();
        return false;
    }

    cachedLocaleDir.reset(new QDir(wDir));
    if (!cachedLocaleDir->cd("share/bibletime/locale")) {
        qWarning() << "Cannot find locale directory relative to" << wDir.absolutePath();
        return false;
    }

    QString localeName(QLocale::system().name());
    QString langCode(localeName.section('_', 0, 0));

    cachedHandbookDir.reset(new QDir(wDir));
    if (!cachedHandbookDir->cd("share/doc/bibletime/handbook/" + localeName)) {
        if (!cachedHandbookDir->cd("share/doc/bibletime/handbook/" + langCode)) {
            if (!cachedHandbookDir->cd("share/doc/bibletime/handbook/en/")) {
                qWarning() << "Cannot find handbook directory relative to" << wDir.absolutePath();
                return false;
            }
        }
    }

    cachedHowtoDir.reset(new QDir(wDir));
    if (!cachedHowtoDir->cd("share/doc/bibletime/howto/" + localeName)) {
        if (!cachedHowtoDir->cd("share/doc/bibletime/howto/" + langCode)) {
            if (!cachedHowtoDir->cd("share/doc/bibletime/howto/en/")) {
                qWarning() << "Cannot find handbook directory relative to" << wDir.absolutePath();
                return false;
            }
        }
    }

    cachedDisplayTemplatesDir.reset(new QDir(wDir)); //display templates dir
    if (!cachedDisplayTemplatesDir->cd("share/bibletime/display-templates/")) {
        qWarning() << "Cannot find display template directory relative to" << wDir.absolutePath();
        return false;
    }

#ifdef Q_OS_WINRT
    cachedUserHomeDir.reset(new QDir(""));
#elif defined (Q_OS_WIN) && !defined(Q_OS_WIN32)
    cachedUserHomeDir.reset(new QDir(QCoreApplication::applicationDirPath()));
#elif defined(ANDROID)
    cachedUserHomeDir.reset(new QDir(qgetenv("EXTERNAL_STORAGE")));
    if(!cachedUserHomeDir->exists() || !cachedUserHomeDir->isReadable())
    {
        qWarning() << "No external storage found, use application home.";
        cachedUserHomeDir->setPath(QDir::homePath());
    }
#elif defined Q_OS_SYMBIAN
    cachedUserHomeDir.reset(new QDir(QCoreApplication::applicationDirPath()[0] + ":\\"));
    if (!cachedUserHomeDir->cd("data")) {
        if(!cachedUserHomeDir->mkpath("data") || !cachedUserHomeDir->cd("data")) {
            qWarning() << "Could not create user home directory" << cachedUserHomeDir->absolutePath();
            return false;
        }
    }
#elif defined Q_OS_WIN32
#define BUFSIZE 4096
    wchar_t homeDir[BUFSIZE];
    GetEnvironmentVariable(TEXT("APPDATA"), homeDir, BUFSIZE);
    QString qHomeDir = QString::fromWCharArray(homeDir);
    cachedUserHomeDir.reset(new QDir(qHomeDir));
#else
    cachedUserHomeDir.reset(new QDir(qgetenv("HOME")));
#endif

    cachedUserBaseDir.reset(new QDir(*cachedUserHomeDir));
    if (!cachedUserBaseDir->cd(BIBLETIME)) {
        if (!cachedUserBaseDir->mkpath(BIBLETIME) || !cachedUserBaseDir->cd(BIBLETIME)) {
            qWarning() << "Could not create user settings directory relative to" << cachedUserHomeDir->absolutePath();
            return false;
        }
    }

    cachedUserHomeSwordDir.reset(new QDir(*cachedUserHomeDir));
#if defined(Q_OS_WIN) && !defined(Q_OS_WIN32)
#else
    if (!cachedUserHomeSwordDir->cd(SWORD_DIR)) {
        if (!cachedUserHomeSwordDir->mkpath(SWORD_DIR) || !cachedUserHomeSwordDir->cd(SWORD_DIR)) {
            qWarning() << "Could not create user home " << SWORD_DIR << " directory.";
            return false;
        }
    }
#endif

#if defined Q_OS_ANDROID || defined Q_OS_SYMBIAN
    // help for SWMgr to find the right place
    qputenv(SWORD_PATH, cachedUserHomeSwordDir->absolutePath().toLocal8Bit());
#endif

    cachedUserHomeSwordModsDir.reset(new QDir(*cachedUserHomeSwordDir));
    if (!cachedUserHomeSwordModsDir->cd("mods.d")) {
        if (!cachedUserHomeSwordModsDir->mkdir("mods.d") || !cachedUserHomeSwordModsDir->cd("mods.d")) {
            qWarning() << "Could not create user home " << SWORD_DIR << " mods.d directory.";
            return false;
        }
    }

    cachedUserSessionsDir.reset(new QDir(*cachedUserBaseDir));
    if (!cachedUserSessionsDir->cd("sessions")) {
        if (!cachedUserSessionsDir->mkdir("sessions") || !cachedUserSessionsDir->cd("sessions")) {
            qWarning() << "Could not create user sessions directory.";
            return false;
        }
    }

    cachedUserCacheDir.reset(new QDir(*cachedUserBaseDir));
    if (!cachedUserCacheDir->cd("cache")) {
        if (!cachedUserCacheDir->mkdir("cache") || !cachedUserCacheDir->cd("cache")) {
            qWarning() << "Could not create user cache directory.";
            return false;
        }
    }

    cachedUserIndexDir.reset(new QDir(*cachedUserBaseDir));
    if (!cachedUserIndexDir->cd("indices")) {
        if (!cachedUserIndexDir->mkdir("indices") || !cachedUserIndexDir->cd("indices")) {
            qWarning() << "Could not create user indices directory.";
            return false;
        }
    }

    cachedUserDisplayTemplatesDir.reset(new QDir(*cachedUserBaseDir));
    if (!cachedUserDisplayTemplatesDir->cd("display-templates")) {
        if (!cachedUserDisplayTemplatesDir->mkdir("display-templates") || !cachedUserDisplayTemplatesDir->cd("display-templates")) {
            qWarning() << "Could not create user display templates directory.";
            return false;
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
    for (QFileInfoList::const_iterator it_file = fileList.begin(); it_file != fileList.end(); ++it_file) {
        d.remove( it_file->fileName() );
    }

    //remove all subdirs recursively
    d.setFilter( QDir::Dirs | QDir::NoSymLinks );
    const QFileInfoList dirList = d.entryInfoList();
    for (QFileInfoList::const_iterator it_dir = dirList.begin(); it_dir != dirList.end(); ++it_dir) {
        if ( !it_dir->isDir() || it_dir->fileName() == "." || it_dir->fileName() == ".." ) {
            continue;
        }
        removeRecursive( it_dir->absoluteFilePath() );
    }
    d.rmdir(dir);
}

/** Returns the size of the directory including the size of all it's files and it's subdirs.
 */
size_t getDirSizeRecursive(QString const & dir) {
    //Check for validity of argument
    QDir d(dir);
    if (!d.exists())
        return 0u;

    size_t size = 0u;

    //First get the size of all files int this folder
    d.setFilter(QDir::Files);
    const QFileInfoList infoList = d.entryInfoList();
    for (QFileInfoList::const_iterator it = infoList.begin(); it != infoList.end(); ++it) {
        BT_ASSERT(it->size() > 0);
        size += it->size();
    }

    //Then add the sizes of all subdirectories
    d.setFilter(QDir::Dirs);
    const QFileInfoList dirInfoList = d.entryInfoList();
    for (QFileInfoList::const_iterator it_dir = dirInfoList.begin(); it_dir != dirInfoList.end(); ++it_dir) {
        if ( !it_dir->isDir() || it_dir->fileName() == "." || it_dir->fileName() == ".." ) {
            continue;
        }
        size += getDirSizeRecursive( it_dir->absoluteFilePath() );
    }
    return size;
}

QString convertDirSeparators(const QString& path) {
    QString result = path;
#ifdef Q_OS_WIN
    result.replace("/", "\\");
#else
    result.replace("\\", "/");
#endif
    return result;
}

#ifdef Q_OS_WIN
const QDir &getApplicationSwordDir() {
    return *cachedApplicationSwordDir;
}
#endif

#if defined Q_OS_WIN || defined Q_OS_ANDROID
const QDir &getSharedSwordDir() {
    return *cachedSharedSwordDir;
}

#endif

#ifdef Q_OS_MACOS
const QDir &getSwordLocalesDir() {
    return *cachedSwordLocalesDir;
}
#endif

const QDir &getSwordPathDir() {
    return *cachedSwordPathDir;
}

const QDir &getIconDir() {
    return *cachedIconDir;
}

const QDir &getLicenseDir() {
    return *cachedLicenseDir;
}

const QDir &getPicsDir() {
    return *cachedPicsDir;
}

const QDir &getLocaleDir() {
    return *cachedLocaleDir;
}

const QDir &getHandbookDir() {
    return *cachedHandbookDir;
}

const QDir &getHowtoDir() {
    return *cachedHowtoDir;
}

const QDir &getDisplayTemplatesDir() {
    return *cachedDisplayTemplatesDir;
}

const QDir &getUserBaseDir() {
    return *cachedUserBaseDir;
}

const QDir &getUserHomeDir() {
    return *cachedUserHomeDir;
}

const QDir &getUserHomeSwordDir() {
    return *cachedUserHomeSwordDir;
}

const QDir &getUserHomeSwordModsDir() {
    return *cachedUserHomeSwordModsDir;
}

const QDir &getUserSessionsDir() {
    return *cachedUserSessionsDir;
}

const QDir &getUserCacheDir() {
    return *cachedUserCacheDir;
}

const QDir &getUserIndexDir() {
    return *cachedUserIndexDir;
}

const QDir &getUserDisplayTemplatesDir() {
    return *cachedUserDisplayTemplatesDir;
}

} // namespace directory
} // namespace util
