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

#include <optional>
#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QFlags>
#include <QLocale>
#include <QStringList>
#include <QtGlobal>
#include "btassert.h"


namespace util {
namespace directory {

namespace {

std::optional<QDir> cachedPrefix;
std::optional<QDir> cachedPicsDir;
std::optional<QDir> cachedLocaleDir;
std::optional<QDir> cachedDisplayTemplatesDir;
std::optional<QDir> cachedUserDisplayTemplatesDir;
std::optional<QDir> cachedUserBaseDir;
std::optional<QDir> cachedUserHomeDir;
std::optional<QDir> cachedUserHomeSwordDir;
std::optional<QDir> cachedUserCacheDir;
std::optional<QDir> cachedUserIndexDir;
#ifdef Q_OS_WIN
// Only Windows installs the sword directory which contains locales.d:
std::optional<QDir> cachedApplicationSwordDir;
std::optional<QDir> cachedSharedSwordDir;
#endif

#ifdef Q_OS_MACOS
std::optional<QDir> cachedSwordLocalesDir;
#endif

#if defined Q_OS_WIN
QString const BIBLETIME = QStringLiteral("Bibletime");
QString const SWORD_DIR = QStringLiteral("Sword");
#elif defined(Q_OS_MAC)
QString const BIBLETIME =
        QStringLiteral("Library/Application Support/BibleTime");
QString const SWORD_DIR = QStringLiteral("Library/Application Support/Sword");
#else
QString const BIBLETIME = QStringLiteral(".bibletime");
QString const SWORD_DIR = QStringLiteral(".sword");
#endif
} // anonymous namespace

bool initDirectoryCache() {
    QDir wDir(QCoreApplication::applicationDirPath());
    wDir.makeAbsolute();
    if (!wDir.cdUp()) { // Installation prefix
        qWarning() << "Unable to use prefix one level up from " << wDir.path();
        return false;
    }
    cachedPrefix.emplace(wDir);

#ifdef Q_OS_WIN
    // application sword dir for Windows only:
    cachedApplicationSwordDir.emplace(wDir);
    if (!cachedApplicationSwordDir->cd(QStringLiteral("share/sword"))) {
        qWarning() << "Cannot find sword directory relative to"
                   << QCoreApplication::applicationDirPath();
        return false;
    }
    // Sword dir for Windows only:
    cachedSharedSwordDir.emplace(qEnvironmentVariable("ProgramData"));
    if (!cachedSharedSwordDir->cd(SWORD_DIR)) {
        if (!cachedSharedSwordDir->mkdir(SWORD_DIR)
            || !cachedSharedSwordDir->cd(SWORD_DIR))
        {
            qWarning() << "Cannot find " << programDataDir << " \\Sword";
            return false;
        }
    }
#endif

#ifdef Q_OS_MACOS
    // application sword dir for Windows only:
    cachedSwordLocalesDir.emplace(wDir);
    if (!cachedSwordLocalesDir->cd(QStringLiteral("share/sword/locales.d"))) {
        qWarning() << "Cannot find sword locales directory relative to"
                   << QCoreApplication::applicationDirPath();
        return false;
    }
#endif

    // We unset the SWORD_PATH so libsword finds paths correctly:
    ::qunsetenv("SWORD_PATH");

    cachedPicsDir.emplace(wDir);
    if (!cachedPicsDir->cd(QStringLiteral("share/bibletime/pics"))) {
        qWarning() << "Cannot find pics directory relative to"
                   << wDir.absolutePath();
        return false;
    }

    cachedLocaleDir.emplace(wDir);
    if (!cachedLocaleDir->cd(QStringLiteral("share/bibletime/locale"))) {
        qWarning() << "Cannot find locale directory relative to"
                   << wDir.absolutePath();
        return false;
    }

    cachedDisplayTemplatesDir.emplace(wDir); //display templates dir
    if (!cachedDisplayTemplatesDir->cd(
            QStringLiteral("share/bibletime/display-templates/")))
    {
        qWarning() << "Cannot find display template directory relative to"
                   << wDir.absolutePath();
        return false;
    }

#ifdef Q_OS_WINRT
    cachedUserHomeDir.emplace();
#elif defined (Q_OS_WIN) && !defined(Q_OS_WIN32)
    cachedUserHomeDir.emplace(QCoreApplication::applicationDirPath());
#elif defined Q_OS_WIN32
    cachedUserHomeDir.emplace(qEnvironmentVariable("APPDATA"));
#else
    cachedUserHomeDir.emplace(qgetenv("HOME"));
#endif

    cachedUserBaseDir.emplace(*cachedUserHomeDir);
    if (!cachedUserBaseDir->cd(BIBLETIME)) {
        if (!cachedUserBaseDir->mkpath(BIBLETIME)
            || !cachedUserBaseDir->cd(BIBLETIME))
        {
            qWarning() << "Could not create user settings directory relative to"
                       << cachedUserHomeDir->absolutePath();
            return false;
        }
    }

    cachedUserHomeSwordDir.emplace(*cachedUserHomeDir);
#if !defined(Q_OS_WIN) || defined(Q_OS_WIN32)
    if (!cachedUserHomeSwordDir->cd(SWORD_DIR)) {
        if (!cachedUserHomeSwordDir->mkpath(SWORD_DIR)
            || !cachedUserHomeSwordDir->cd(SWORD_DIR))
        {
            qWarning() << "Could not create user home " << SWORD_DIR
                       << " directory.";
            return false;
        }
    }
#endif

    { /// \todo Check and comment whether this is needed:
        auto userHomeSwordModsDir = *cachedUserHomeSwordDir;
        static auto const modsDir = QStringLiteral("mods.d");
        if (!userHomeSwordModsDir.cd(modsDir)) {
            if (!userHomeSwordModsDir.mkdir(modsDir)
                || !userHomeSwordModsDir.cd(modsDir))
            {
                qWarning() << "Could not create user home " << SWORD_DIR
                           << " mods.d directory.";
                return false;
            }
        }
    }

    cachedUserCacheDir.emplace(*cachedUserBaseDir);
    {
        static auto const cacheDir = QStringLiteral("cache");
        if (!cachedUserCacheDir->cd(cacheDir)) {
            if (!cachedUserCacheDir->mkdir(cacheDir)
                || !cachedUserCacheDir->cd(cacheDir))
            {
                qWarning() << "Could not create user cache directory.";
                return false;
            }
        }
    }

    cachedUserIndexDir.emplace(*cachedUserBaseDir);
    {
        static auto const indicesDir = QStringLiteral("indices");
        if (!cachedUserIndexDir->cd(indicesDir)) {
            if (!cachedUserIndexDir->mkdir(indicesDir)
                || !cachedUserIndexDir->cd(indicesDir))
            {
                qWarning() << "Could not create user indices directory.";
                return false;
            }
        }
    }

    cachedUserDisplayTemplatesDir.emplace(*cachedUserBaseDir);
    {
        static auto const displayTemplatesDir =
                QStringLiteral("display-templates");
        if (!cachedUserDisplayTemplatesDir->cd(displayTemplatesDir)) {
            if (!cachedUserDisplayTemplatesDir->mkdir(displayTemplatesDir)
                || !cachedUserDisplayTemplatesDir->cd(displayTemplatesDir))
            {
                qWarning() << "Could not create user display templates "
                              "directory.";
                return false;
            }
        }
    }

    return true;
} // bool initDirectoryCache();

/** \returns the size of the directory including the size of all it's files and
             it's subdirs. */
::qint64 getDirSizeRecursive(QString const & dir) {
    //Check for validity of argument
    QDir d(dir);
    if (!d.exists())
        return 0u;

    ::qint64 size = 0u;

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
            && dirInfo.fileName() != QStringLiteral(".")
            && dirInfo.fileName() != QStringLiteral(".."))
            size += getDirSizeRecursive(dirInfo.absoluteFilePath());
    return size;
}

#if defined(Q_OS_WIN)
const QDir &getApplicationSwordDir() {
    return *cachedApplicationSwordDir;
}

const QDir &getSharedSwordDir() {
    return *cachedSharedSwordDir;
}
#elif defined(Q_OS_MACOS)
const QDir &getSwordLocalesDir() {
    return *cachedSwordLocalesDir;
}
#endif

QDir const & getIconDir() {
    static const QDir cachedIconDir(
                cachedPrefix->filePath(
                    QStringLiteral("share/bibletime/icons")));
    return cachedIconDir;
}

QString const & getLicensePath() {
    static auto const cachedLicensePath(
                cachedPrefix->filePath(
                    QStringLiteral("share/bibletime/license/LICENSE")));
    return cachedLicensePath;
}

const QDir &getPicsDir() {
    return *cachedPicsDir;
}

const QDir &getLocaleDir() {
    return *cachedLocaleDir;
}

namespace {

QString getDocFile(QString docName) {
    static QDir const cachedDocDir(cachedPrefix->filePath(BT_RUNTIME_DOCDIR));
    QString r;
    if (auto dir = cachedDocDir; dir.cd(docName)) {
        QStringList tryLanguages;
        tryLanguages.append(QLocale().name());
        {
            auto const & localeName = tryLanguages.back();
            if (localeName.contains('_'))
                tryLanguages.append(localeName.section('_', 0, 0));
        }
        tryLanguages.append(QStringLiteral("en"));
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
        r = tryDoc(QStringLiteral("html"), QStringLiteral("index.html"));
        if (r.isEmpty())
            r = tryDoc(QStringLiteral("pdf"), docName + QStringLiteral(".pdf"));
    }
    return r;
}

} // anonymous namespace

QString getHandbook() { return getDocFile(QStringLiteral("handbook")); }

QString getHowto() { return getDocFile(QStringLiteral("howto")); }

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
