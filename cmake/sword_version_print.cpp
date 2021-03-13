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

/*
 * Check if Sword's current version is equal or larger than the version given to the program (first commandline argument)
 */

#include <iostream>
#include <swversion.h>

#ifndef SWORD_VERSION_STR
#error SWORD_VERSION_STR not defined!
#endif

int main() {
    std::cout << (SWORD_VERSION_STR);
    return 0;
}
