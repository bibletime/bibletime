/******************************************************************************
 *
 * $Id: bt_thmlplain.h,v 1.1 2007/03/14 21:32:47 joachim Exp $
 *
 * Copyright 1998 CrossWire Bible Society (http://www.crosswire.org)
 *	CrossWire Bible Society
 *	P. O. Box 2528
 *	Tempe, AZ  85280-2528
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */

#ifndef BT_THMLPLAIN_H
#define BT_THMLPLAIN_H

#include <swfilter.h>

using namespace sword;

namespace Filters {

/** this filter converts ThML text to plain text
 */
class BT_ThMLPlain : public SWFilter {
protected:
	virtual char processText(SWBuf &text, const SWKey *key = 0, const SWModule *module = 0);
public:
	BT_ThMLPlain();
};

}
#endif
