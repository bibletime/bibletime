/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef RENDERINGCCHAPTERDISPLAY_H
#define RENDERINGCCHAPTERDISPLAY_H

#include "backend/rendering/centrydisplay.h"


namespace Rendering {

/** Chapter rendering.
* A CEntryDisplay implementation mde for Bibles to display whole chapters
* at once.
* @author The BibleTime team
*/

class CChapterDisplay : public CEntryDisplay {

    public: // Public methods
        virtual ~CChapterDisplay() {}

        /**
        * Returns the rendered text using the modules in the list and using the key parameter.
        * The displayoptions and filter options are used, too.
        */
        virtual const QString text( const QList<CSwordModuleInfo*>& modules, const QString& key, const CSwordBackend::DisplayOptions displayOptions, const CSwordBackend::FilterOptions filterOptions);
};

}

#endif
