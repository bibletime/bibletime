/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef RENDERINGCBOOKDISPLAY_H
#define RENDERINGCBOOKDISPLAY_H

#include "backend/rendering/centrydisplay.h"

#include "backend/rendering/ctextrendering.h"


class CSwordTreeKey;

namespace Rendering {

/**
* A CEntryDisplay implementation which works on tree-based GenBook modules
* of Sword.
* @short CEntryDisplay implementation for GenBook modules,
* @author The BibleTime team
*/

class CBookDisplay : public CEntryDisplay {
    public: // Public methods
        virtual ~CBookDisplay() {}

        /**
        * Returns the rendered text using the modules in the list and using the key parameter.
        * The displayoptions and filter options are used, too.
        */
        virtual const QString text( const QList<CSwordModuleInfo*>& modules, const QString& key, const CSwordBackend::DisplayOptions displayOptions, const CSwordBackend::FilterOptions filterOptions);

    protected:
        void setupRenderTree(CSwordTreeKey* swordTree, CTextRendering::KeyTree* renderTree, const QString& highlightKey);
};

}

#endif
