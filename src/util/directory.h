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

#pragma once

#include <cstddef>
#include <QtGlobal>
#include <QString>


class QDir;

namespace util {

/**
 * Tools for working with directories.
*/
namespace directory {

/**
  \brief Initializes the directory cache and returns whether it was successful.
  \note When this function returns unsuccessfully, the program should exit.
*/
bool initDirectoryCache();

/** Returns the size of the directory including the size of all its files
 * and its subdirs.
 *
 * \todo Check if it's suitable for huge dir trees, as it holds a QDir object
 * for each of it at the same time in the deepest recursion.
 * For really deep dir tree this may lead to a stack overflow.
 *
 * @return The size of the dir in bytes
 */
::qint64 getDirSizeRecursive(QString const & dir);

#if defined(Q_OS_WIN) // Windows only directory
/** Return the path to the sword dir., Windows only
    C:\Program Files\BibleTime\share\sword
 */
const QDir &getApplicationSwordDir();

/** Return the path to the shared Sword directory */
const QDir &getSharedSwordDir();

#elif defined(Q_OS_MAC) // Mac only directories
/**
 * Path to the Sword locales
 */
const QDir &getSwordLocalesDir();
#endif

/** Return the path to the icons. */
const QDir &getIconDir();

/** Return the path to the license. */
QString const & getLicensePath();

/** Return the path to the pictures. */
const QDir &getPicsDir();

/** Return the path to the translation files. */
const QDir &getLocaleDir();

/** \returns an absolute path to a handbook or QString() if not found. */
QString getHandbook();

/** \returns an absolute path to a howto or QString() if not found. */
QString getHowto();

/** Return the path to the default display template files. */
const QDir &getDisplayTemplatesDir();

/** Return the path to the user's home directory.
    %APPDATA% on Windows
    $HOME on linux */
const QDir &getUserHomeDir();

/** Return the path to the user's home .sword (or Sword) directory.
    %APPDATA%\Sword on Windows
    $HOME\.sword on linux */
const QDir &getUserHomeSwordDir();

/** Return the path to the user's settings directory.*/
const QDir &getUserBaseDir();

/** Return the path to the user's cache directory.*/
const QDir &getUserCacheDir();

/** Return the path to the user's indices directory.*/
const QDir &getUserIndexDir();

/** Return the path to the user's custom display templates directory.*/
const QDir &getUserDisplayTemplatesDir();

} // namespace directory
} // namespace util
