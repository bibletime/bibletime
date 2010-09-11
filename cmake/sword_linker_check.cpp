/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

/*
 * Use some important Sword classes to see if the linker flags work correctly
 */

#include <iostream>

#include <swmgr.h>
#include <installmgr.h>
#include <ftptrans.h>

int main(int argc, char* argv[])
{
    sword::SWMgr mgr;
    sword::InstallMgr imgr;
    sword::FTPTransport trans("example.com");

    return 0;
}
