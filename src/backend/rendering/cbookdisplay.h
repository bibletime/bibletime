/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
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
* \brief CEntryDisplay implementation for GenBook modules,

  A CEntryDisplay implementation which works on tree-based GenBook modules of
  Sword.
*/
class CBookDisplay: public CEntryDisplay {

    public: /* Methods: */

        virtual inline ~CBookDisplay() {}

        virtual const QString text(const QList<const CSwordModuleInfo*> &modules,
                                   const QString &key,
                                   const DisplayOptions &displayOptions,
                                   const FilterOptions &filterOptions);

    protected: /* Methods: */

        void setupRenderTree(CSwordTreeKey *swordTree,
                             CTextRendering::KeyTree *renderTree,
                             const QString &highlightKey);

}; /* class CBookDisplay */

} /* namespace Rendering */

#endif
