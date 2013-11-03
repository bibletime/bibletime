/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

/*
 * Check if Sword's current version is equal or larger than the version given to the program (first commandline argument)
 */

#include <iostream>
#include <swversion.h>

int main() {
    std::cout << (SWORD_VERSION_STR) << std::endl;
    return 0;
}
