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

#include <QString>
#include "ctextrendering.h"


class CSwordKey;
struct DisplayOptions;
struct FilterOptions;

namespace Rendering {

/**
 * This implementation can be used to export content as plain text.
 * @short Text rendering as plain text.
 * @author The BibleTime team
 */
class CPlainTextExportRendering: public CTextRendering {

    public: // methods:

        using CTextRendering::CTextRendering;

    protected: // methods:

        QString renderEntry(KeyTreeItem const & item, CSwordKey * key = nullptr)
                const override;
        QString finishText(QString const & text, KeyTree const & tree)
                const override;

}; /* class CPlainTextExportRendering */

} /* namespace Rendering */
