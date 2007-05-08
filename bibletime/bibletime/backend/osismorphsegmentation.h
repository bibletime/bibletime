/******************************************************************************
 *
 * $Id: osismorphsegmentation.h,v 1.4 2006/08/08 19:32:48 joachim Exp $
 *
 * Copyright 1998 CrossWire Bible Society (http://www.crosswire.org)
 * CrossWire Bible Society
 * P. O. Box 2528
 * Tempe, AZ  85280-2528
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

#ifndef OSISMORPHSEGMENTATION_H
#define OSISMORPHSEGMENTATION_H

#include <swoptfilter.h>

using namespace sword;

namespace Filters {

/** This Filter shows/hides headings in a OSIS text.
 * @author Martin Gruner
 */
class OSISMorphSegmentation : public SWOptionFilter {
	static const char oName[];
	static const char oTip[];
	static const SWBuf choices[3];
	static const StringList oValues;

public:
	OSISMorphSegmentation();
	virtual ~OSISMorphSegmentation();

	virtual char processText(SWBuf &text, const SWKey *key = 0, const SWModule *module = 0);
};

}

#endif
