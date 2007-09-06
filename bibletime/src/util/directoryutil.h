/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
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
	
	/** Return the path to the icons. */
	static QDir getIconDir(void);
	
	/** Returns an icon with the given name */
	static QIcon getIcon(const QString& name);
	
	/** Return the path to the pictures. */
	static QDir getPicsDir(void);

	/** Return the path to the xml files. */
	static QDir getXmlDir(void);
	
private:
	/** Will cache a few directories like icon directory */
	static void initDirectoryCache(void);

};
	
} //namespace filesystem

} //namespace directoryutil

#endif
