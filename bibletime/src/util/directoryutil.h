/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef UTIL_FILESDIRECTORYUTIL_H
#define UTIL_FILESDIRECTORYUTIL_H

#include <QString>
#include <QDir>
#include <QIcon>

namespace util {

namespace filesystem {

/**
 * Tools for working with directories.
 * @author The BibleTime team <info@bibletime.info>
*/
class DirectoryUtil {
private:
	DirectoryUtil() {};
	~DirectoryUtil() {};

public:
	/** Removes the given dir with all it's files and subdirs.
	 *
	 * TODO: Check if it's suitable for huge dir trees, as it holds a QDir object
	 * for each of it at the same time in the deepest recursion.
	 * For really deep dir tree this may lead to a stack overflow.
	 */
	static void removeRecursive(const QString dir);

	/** Returns the size of the directory including the size of all its files
	 * and its subdirs.
	 * 
	 * TODO: Check if it's suitable for huge dir trees, as it holds a QDir object
	 * for each of it at the same time in the deepest recursion.
	 * For really deep dir tree this may lead to a stack overflow.
	 *
	 * @return The size of the dir in bytes
	 */
	static unsigned long getDirSizeRecursive(const QString dir);

	/** Recursively copies one directory into another.  This WILL OVERWRITE
	 * any existing files of the same name, and WILL NOT handle symlinks.
	 *
	 * This probably won't need to be used for large directory structures.
	 */
	static void copyRecursive(const QString src, const QString dest);

	/** Return the path to the icons. */
	static QDir getIconDir(void);
	
	/** Returns an icon with the given name */
	static QIcon getIcon(const QString& name);
	
	/** Return the path to the pictures. */
	static QDir getPicsDir(void);

	/** Return the path to the translation files. */
	static QDir getLocaleDir(void);

	/** Return the path to the handbook files, either of the current locale or en as fallback. */
	static QDir getHandbookDir(void);

	/** Return the path to the bible study howto files, either of the current locale or en as fallback. */
	static QDir getHowtoDir(void);

	/** Return the path to the default display template files. */
	static QDir getDisplayTemplatesDir(void);

	/** Return the path to the user's home directory.*/
	static QDir getUserHomeDir(void);

	/** Return the path to the user's settings directory.*/
	static QDir getUserBaseDir(void);

	/** Return the path to the user's sessions directory.*/
	static QDir getUserSessionsDir(void);

	/** Return the path to the user's cache directory.*/
	static QDir getUserCacheDir(void);

	/** Return the path to the user's indices directory.*/
	static QDir getUserIndexDir(void);
	
	/** Return the path to the user's custom display templates directory.*/
	static QDir getUserDisplayTemplatesDir(void);
	
private:
	/** Will cache a few directories like icon directory */
	static void initDirectoryCache(void);

};
	
} //namespace filesystem

} //namespace directoryutil

#endif
