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
#include "../config/btconfig.h"
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

        CPlainTextExportRendering(
            bool addText,
            const DisplayOptions &displayOptions = btConfig().getDisplayOptions(),
            const FilterOptions &filterOptions = btConfig().getFilterOptions());

    protected: // methods:

        QString renderEntry(const KeyTreeItem &item, CSwordKey * key = nullptr) override;
        QString finishText(const QString &text, const KeyTree &tree) override;

}; /* class CPlainTextExportRendering */

} /* namespace Rendering */
