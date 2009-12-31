/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef UTIL_MIGRATIONUTIL_H
#define UTIL_MIGRATIONUTIL_H

namespace util {

/**
 * Tools for handling settings migration automatically.
 * @author The BibleTime team <info@bibletime.info>
 */
namespace migration {

/**
* Performs any and all applicable migration actions, if neccessary
*/
void checkMigration();

/*
 * Performs a migration from a KDE 3 version of BibleTime.  It supports all
 * KDE 3 versions of BibleTime, including versions older than 1.3.  Its
 * only alteration is to move files to the new location, and to rename the
 * sessions directory from pre-1.3 versions if necessary.  It does not
 * change any settings.
 */
void tryMigrationFromKDE3();

} // namespace migration
} // namespace util

#endif
