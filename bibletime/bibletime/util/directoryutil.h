//
// C++ Interface: directoryutil
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef UTIL_FILESDIRECTORYUTIL_H
#define UTIL_FILESDIRECTORYUTIL_H

#include <qstring.h>

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
    static void removeRecursive(const QString& dir);

	/** Returns the size of the directory including the size of all it's files
	 * and it's subdirs.
	 * 
	 * TODO: Check if it's suitable for huge dir trees, as it holds a QDir object
	 * for each of it at the same time in the deepest recursion.
	 * For really deep dir tree this may lead to a stack overflow.
	 *
	 * @return The size of the dir in bytes
	 */
	static unsigned long getDirSizeRecursive(const QString& dir);
};

}

}

#endif
