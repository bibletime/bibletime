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

#include "cswordmoduleinfo.h"

#include <QObject>
#include <QString>


class CSwordBackend;
namespace sword {
class SWModule;
class TreeKeyIdx;
}

/** \brief Class for generic book support. */
class CSwordBookModuleInfo final: public CSwordModuleInfo {

    Q_OBJECT

public: // Methods:

    /**
      \param module The module which belongs to this object.
      \param backend The parent backend for this book module.
    */
    CSwordBookModuleInfo(sword::SWModule & module, CSwordBackend & usedBackend);

    /** \returns the maximal depth of sections and subsections. */
    int depth() const noexcept { return m_depth; }

    /**
      \returns A treekey filled with the structure of this module. Don't delete
               the returned key because it's casted from the module object.
    */
    sword::TreeKeyIdx * tree() const;

    CSwordKey * createKey() const final override;

private: // Fields:

    int const m_depth;

};
