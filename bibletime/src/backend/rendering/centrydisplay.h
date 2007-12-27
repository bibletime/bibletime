/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CENTRYDISPLAY_H
#define CENTRYDISPLAY_H

//BibleTime
//#include "ctextrendering.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/cswordbackend.h"

#include "util/cpointers.h"

//Sword
#include <swdisp.h>

//Qt
#include <QString>

class CSwordModuleInfo;

namespace Rendering {

/**
* The reimplementation of SWDisplay to fit our needs.
* @short Display implementation
* @author The BibleTime team
*/

class CEntryDisplay : public sword::SWDisplay, public CPointers {

public:
	virtual ~CEntryDisplay() {}

	/**
	* Returns the rendered text using the modules in the list and using the key parameter.
	* The displayoptions and filter options are used, too.
	*/
	virtual const QString text( const ListCSwordModuleInfo& modules, const QString& key, const CSwordBackend::DisplayOptions displayOptions, const CSwordBackend::FilterOptions filterOptions);
};


}

#endif
