/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include "centrydisplay.h"

#include <QString>
#include "../drivers/btmodulelist.h"


struct DisplayOptions;
struct FilterOptions;

namespace Rendering {

/**
* \brief CEntryDisplay implementation for GenBook modules,

  A CEntryDisplay implementation which works on tree-based GenBook modules of
  Sword.
*/
class CBookDisplay final: public CEntryDisplay {

public: // methods:

    QString text(BtConstModuleList const & modules,
                 QString const & key,
                 DisplayOptions const & displayOptions,
                 FilterOptions const & filterOptions) final override;

}; /* class CBookDisplay */

} /* namespace Rendering */
