/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



/* $Header: /cvsroot/bibletime/bibletime/bibletime/backend/bt_plainhtml.h,v 1.2 2006/09/20 17:11:36 mgruner Exp $ */
/* $Revision: 1.2 $ */

#ifndef BT_PLAINHTML_H
#define BT_PLAINHTML_H

//Backend
#include "cfiltertool.h"

//Sword includes
#include <swkey.h>
#include <swmodule.h>
#include <swfilter.h>
//#include <plainhtml.h>

namespace Filters {

	/** Plain to HTML filter,
	* This filter converts Plain Text into HTML
	*/

class BT_PLAINHTML : public sword::SWFilter{

protected:

public:
		BT_PLAINHTML();
		virtual char processText(sword::SWBuf& buf, const sword::SWKey*, const sword::SWModule * = 0);
	};

}

#endif
