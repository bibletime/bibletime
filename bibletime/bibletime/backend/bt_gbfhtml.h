/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



/* $Header: /cvsroot/bibletime/bibletime/bibletime/backend/bt_gbfhtml.h,v 1.24 2006/02/25 11:38:15 joachim Exp $ */
/* $Revision: 1.24 $ */

#ifndef BT_GBFHTML_H
#define BT_GBFHTML_H

//Sword includes
#include <gbfhtml.h>

namespace Filters {

	/** GBF to HTML filter,
	* This filter converts GBF Text into HTML
	*/

class BT_GBFHTML : public sword::GBFHTML/*, protected CFilterTool */
	{

protected:

class BT_UserData : public sword::GBFHTML::MyUserData {

public:
BT_UserData(const sword::SWModule *module, const sword::SWKey *key) : sword::GBFHTML::MyUserData(module, key) {
				swordFootnote = 1;
				hasFootnotePreTag = false;
			}

			short unsigned int swordFootnote;
		};

		virtual sword::BasicFilterUserData *createUserData(const sword::SWModule* module, const sword::SWKey* key) {
			return new BT_UserData(module, key);
		}

public:
		BT_GBFHTML();
		virtual bool handleToken(sword::SWBuf &buf, const char *token, sword::BasicFilterUserData *userData);
		virtual char processText(sword::SWBuf& buf, const sword::SWKey*, const sword::SWModule * = 0);
	};

}

#endif
