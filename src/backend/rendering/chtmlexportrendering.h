/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef RENDERINGCHTMLEXPORTRENDERING_H
#define RENDERINGCHTMLEXPORTRENDERING_H

#include "ctextrendering.h"

#include "../btglobal.h"
#include "../config/btconfig.h"
#include "../managers/cswordbackend.h"


namespace Rendering {

/**
  \brief Provides HTML rendering for export.
  This CTextRenerding implementation creates HTML specially made for export as
  HTML files.
*/
class CHTMLExportRendering: public CTextRendering {

public: /* Methods: */

    CHTMLExportRendering(
        bool addText,
        DisplayOptions const & displayOptions =
                btConfig().getDisplayOptions(),
        FilterOptions const & filterOptions =
                btConfig().getFilterOptions());

protected: /* Methods: */

    QString renderEntry(KeyTreeItem const & item,
                        CSwordKey * key = nullptr) override;
    QString finishText(QString const & text, KeyTree const & tree) override;
    virtual QString entryLink(KeyTreeItem const & item,
                              CSwordModuleInfo const * module);
    void initRendering() override;

protected: /* Fields: */

    DisplayOptions const m_displayOptions;
    FilterOptions const m_filterOptions;
    bool const m_addText;

}; /* class CHTMLExportRendering */

} /* namespace Rendering */

#endif
