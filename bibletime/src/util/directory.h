/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef UTIL_FILESDIRECTORYUTIL_H
#define UTIL_FILESDIRECTORYUTIL_H

#include <QDir>
#include <QIcon>
#include <QString>


namespace util {

/**
 * Tools for working with directories.
 * @author The BibleTime team <info@bibletime.info>
*/
namespace directory {

/**
  \brief Initializes the directory cache and returns whether it was successful.
  \note When this function returns unsuccessfully, the program should exit.
*/
bool initDirectoryCache();

/** Removes the given dir with all it's files and subdirs.
 *
 * \todo Check if it's suitable for huge dir trees, as it holds a QDir object
 * for each of it at the same time in the deepest recursion.
 * For really deep dir tree this may lead to a stack overflow.
 */
void removeRecursive(const QString &dir);

/** Returns the size of the directory including the size of all its files
 * and its subdirs.
 *
 * \todo Check if it's suitable for huge dir trees, as it holds a QDir object
 * for each of it at the same time in the deepest recursion.
 * For really deep dir tree this may lead to a stack overflow.
 *
 * @return The size of the dir in bytes
 */
unsigned long getDirSizeRecursive(const QString &dir);

/** Recursively copies one directory into another.  This WILL OVERWRITE
 * any existing files of the same name, and WILL NOT handle symlinks.
 *
 * This probably won't need to be used for large directory structures.
 */
void copyRecursive(const QString &src, const QString &dest);

/** Convert directory path separators to those for each platform
 *  Windows = "\", Others = "/"
 */
QString convertDirSeparators(const QString& path);

#ifdef Q_WS_WIN  // Windows only directories

/** Return the path to the sword dir., Windows only
    C:\Program Files\BibleTime\share\sword
 */
QDir getApplicationSwordDir();

/** Return the path to the %ALLUSERSPROFILE%\Sword directory */
QDir getSharedSwordDir();

#endif

/** Return the path to the icons. */
QDir getIconDir();

/** Return the path to the javascript. */
QDir getJavascriptDir();

/** Return the path to the license. */
QDir getLicenseDir();

/** Returns an icon with the given name */
QIcon getIcon(const QString &name);

/** Return the path to the pictures. */
QDir getPicsDir();

/** Return the path to the translation files. */
QDir getLocaleDir();

/** Return the path to the handbook files, either of the current locale or en as fallback. */
QDir getHandbookDir();

/** Return the path to the bible study howto files, either of the current locale or en as fallback. */
QDir getHowtoDir();

/** Return the path to the default display template files. */
QDir getDisplayTemplatesDir();

/** Return the path to the user's home directory.
    %APPDATA% on Windows
	$HOME on linux */
QDir getUserHomeDir();

/** Return the path to the user's home .sword (or Sword) directory.
    %APPDATA%\Sword on Windows
	$HOME\.sword on linux */
QDir getUserHomeSwordDir();

/** Return the path to the user's home .sword (or Sword) mods.d directory.
    %APPDATA%\Sword\mods.d on Windows
	$HOME\.sword\mods.d on linux */
QDir getUserHomeSwordModsDir();

/** Return the path to the user's settings directory.*/
QDir getUserBaseDir();

/** Return the path to the user's sessions directory.*/
QDir getUserSessionsDir();

/** Return the path to the user's cache directory.*/
QDir getUserCacheDir();

/** Return the path to the user's indices directory.*/
QDir getUserIndexDir();

/** Return the path to the user's custom display templates directory.*/
QDir getUserDisplayTemplatesDir();

} // namespace directory
} // namespace util

#endif
