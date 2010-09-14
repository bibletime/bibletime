/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BT_PLAINHTML_H
#define BT_PLAINHTML_H

//Sword includes
#include <swfilter.h>
#include <swbuf.h>

class SWKey;
class SWModule;

namespace Filters {

	/** Plain to HTML filter,
	* This filter converts Plain Text into HTML
	*/
class BT_PLAINHTML : public sword::SWFilter{
protected:
		virtual char processText(sword::SWBuf& buf, const sword::SWKey*, const sword::SWModule * = 0);
public:
		BT_PLAINHTML();
	};
}

#endif
