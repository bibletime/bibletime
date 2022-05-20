/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "directory.h"

#include <memory>
#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QFlags>
#include <QList>
#include <QLocale>
#include <QStringList>
#include <QtGlobal>
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
std::unique_ptr<QDir> cachedDocDir;
std::unique_ptr<QDir> cachedDisplayTemplatesDir;
std::unique_ptr<QDir> cachedUserDisplayTemplatesDir;
std::unique_ptr<QDir> cachedUserBaseDir;
std::unique_ptr<QDir> cachedUserHomeDir;
std::unique_ptr<QDir> cachedUserHomeSwordDir;
std::unique_ptr<QDir> cachedUserCacheDir;
std::unique_ptr<QDir> cachedUserIndexDir;
#ifdef Q_OS_WIN
std::unique_ptr<QDir> cachedApplicationSwordDir; // Only Windows installs the sword directory which contains locales.d
std::unique_ptr<QDir> cachedSharedSwordDir;
#endif

#ifdef Q_OS_MACOS
std::unique_ptr<QDir> cachedSwordLocalesDir;
#endif

#if defined Q_OS_WIN
static const char BIBLETIME[] = "Bibletime";
static const char SWORD_DIR[] = "Sword";
#elif defined(Q_OS_MAC)
static const char BIBLETIME[] = "Library/Application Support/BibleTime";
static const char SWORD_DIR[] = "Library/Application Support/Sword";
#else
static const char BIBLETIME[] = ".bibletime";
static const char SWORD_DIR[] = ".sword";
#endif
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

#ifdef Q_OS_WIN
    cachedApplicationSwordDir.reset(new QDir(wDir)); // application sword dir for Windows only
    if (!cachedApplicationSwordDir->cd("share/sword") || !cachedApplicationSwordDir->isReadable()) {
        qWarning() << "Cannot find sword directory relative to" << QCoreApplication::applicationDirPath();
        return false;
    }
    // Sword dir for Windows only:
    cachedSharedSwordDir.reset(new QDir(qEnvironmentVariable("ProgramData")));
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

    // We unset the SWORD_PATH so libsword finds paths correctly:
    ::qunsetenv("SWORD_PATH");

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

    QString localeName(QLocale().name());
    QString langCode(localeName.section('_', 0, 0));


    cachedDocDir.reset(new QDir(wDir));
    if (!cachedDocDir->cd(BT_RUNTIME_DOCDIR)) {
        qWarning() << "Cannot find documentation directory relative to"
                   << wDir.absolutePath();
        return false;
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
#elif defined Q_OS_WIN32
    cachedUserHomeDir.reset(new QDir(qEnvironmentVariable("APPDATA")));
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
#if !defined(Q_OS_WIN) || defined(Q_OS_WIN32)
    if (!cachedUserHomeSwordDir->cd(SWORD_DIR)) {
        if (!cachedUserHomeSwordDir->mkpath(SWORD_DIR) || !cachedUserHomeSwordDir->cd(SWORD_DIR)) {
            qWarning() << "Could not create user home " << SWORD_DIR << " directory.";
            return false;
        }
    }
#endif

    { /// \todo Check and comment whether this is needed:
        auto userHomeSwordModsDir = *cachedUserHomeSwordDir;
        if (!userHomeSwordModsDir.cd("mods.d")) {
            if (!userHomeSwordModsDir.mkdir("mods.d")
                || !userHomeSwordModsDir.cd("mods.d"))
            {
                qWarning() << "Could not create user home " << SWORD_DIR << " mods.d directory.";
                return false;
            }
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
    for (auto const & fileInfo : d.entryInfoList())
        d.remove(fileInfo.fileName());

    //remove all subdirs recursively
    d.setFilter( QDir::Dirs | QDir::NoSymLinks );
    for (auto const & dirInfo : d.entryInfoList())
        if (dirInfo.isDir()
            && dirInfo.fileName() != "."
            && dirInfo.fileName() != "..")
            removeRecursive(dirInfo.absoluteFilePath());
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
    for (auto const & fileInfo : d.entryInfoList()) {
        BT_ASSERT(fileInfo.size() > 0);
        size += fileInfo.size();
    }

    //Then add the sizes of all subdirectories
    d.setFilter(QDir::Dirs);
    for (auto const & dirInfo : d.entryInfoList())
        if (dirInfo.isDir()
            && dirInfo.fileName() != "."
            && dirInfo.fileName() != "..")
            size += getDirSizeRecursive(dirInfo.absoluteFilePath());
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

#if defined Q_OS_WIN
const QDir &getSharedSwordDir() {
    return *cachedSharedSwordDir;
}
#endif

#ifdef Q_OS_MACOS
const QDir &getSwordLocalesDir() {
    return *cachedSwordLocalesDir;
}
#endif

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

namespace {

QString getDocFile(QString docName) {
    auto dir(*cachedDocDir);
    QString r;
    if (dir.cd(docName)) {
        QStringList tryLanguages;
        tryLanguages.append(QLocale().name());
        {
            auto const & localeName = tryLanguages.back();
            if (localeName.contains('_'))
                tryLanguages.append(localeName.section('_', 0, 0));
        }
        tryLanguages.append("en");
        auto const tryDoc =
                [&dir,&tryLanguages](QString const & type,
                                     QString const & filename) -> QString
                {
                    if (dir.cd(type)) {
                        for (auto const & tryLanguage : tryLanguages) {
                            if (dir.cd(tryLanguage)) {
                                if (dir.exists(filename))
                                    return dir.absoluteFilePath(filename);
                                dir.cdUp();
                            }
                        }
                        dir.cdUp();
                    }
                    return {};
                };
        r = tryDoc("html", "index.html");
        if (r.isEmpty())
            r = tryDoc("pdf", docName + ".pdf");
    }
    return r;
}

} // anonymous namespace

QString getHandbook() { return getDocFile("handbook"); }

QString getHowto() { return getDocFile("howto"); }

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
