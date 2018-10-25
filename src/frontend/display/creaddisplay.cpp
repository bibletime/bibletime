/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "frontend/display/creaddisplay.h"

#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/drivers/cswordbookmoduleinfo.h"
#include "backend/drivers/cswordlexiconmoduleinfo.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordkey.h"
#include "backend/keys/cswordldkey.h"
#include "backend/keys/cswordtreekey.h"
#include "backend/keys/cswordversekey.h"
#include "frontend/cexportmanager.h"
#include "frontend/displaywindow/cdisplaywindow.h"
#include "frontend/displaywindow/creadwindow.h"


CReadDisplay::CReadDisplay(CReadWindow* readWindow) :
    CDisplay(readWindow),
    m_activeAnchor(QString::null),
    m_useMouseTracking(true) {}

/** Returns the current active anchor. */
const QString& CReadDisplay::activeAnchor() const {
    return m_activeAnchor;
}

/** Sets the current anchor to the parameter. */
void CReadDisplay::setActiveAnchor( const QString& anchor ) {
    m_activeAnchor = anchor;
}


/** Returns true if the display has an active anchor. */
bool CReadDisplay::hasActiveAnchor() const {
    return !activeAnchor().isEmpty();
}


void CReadDisplay::print(const CDisplay::TextPart type,
                         const DisplayOptions &displayOptions,
                         const FilterOptions &filterOptions)
{
    using CSBiMI = CSwordBibleModuleInfo;
    CDisplayWindow* window = parentWindow();
    CSwordKey* const key = window->key();
    const CSwordModuleInfo *module = key->module();

    const CDisplayWindow *displayWindow = parentWindow();
    CExportManager mgr(false, QString::null, displayWindow->filterOptions(), displayWindow->displayOptions());

    switch (type) {
    case Document: {
        if (module->type() == CSwordModuleInfo::Bible) {
            CSwordVerseKey* vk = dynamic_cast<CSwordVerseKey*>(key);

            CSwordVerseKey startKey(*vk);
            startKey.setVerse(1);

            CSwordVerseKey stopKey(*vk);

            const CSBiMI *bible = dynamic_cast<const CSBiMI*>(module);
            if (bible) {
                stopKey.setVerse(bible->verseCount(bible->bookNumber(startKey.book()), startKey.getChapter()));
            }

            mgr.printKey(module, startKey.key(), stopKey.key(), displayOptions, filterOptions);
        }
        else if (module->type() == CSwordModuleInfo::Lexicon || module->type() == CSwordModuleInfo::Commentary ) {
            mgr.printKey(module, key->key(), key->key(), displayOptions, filterOptions);
        }
        else if (module->type() == CSwordModuleInfo::GenericBook) {
            CSwordTreeKey* tree = dynamic_cast<CSwordTreeKey*>(key);

            CSwordTreeKey startKey(*tree);
            //        while (startKey.previousSibling()) { // go to first sibling on this level!
            //        }

            CSwordTreeKey stopKey(*tree);
            //    if (CSwordBookModuleInfo* book = dynamic_cast<CSwordBookModuleInfo*>(module)) {
            //          while ( stopKey.nextSibling() ) { //go to last displayed sibling!
            //          }
            //        }
            mgr.printKey(module, startKey.key(), stopKey.key(), displayOptions, filterOptions);
        }
    }

    case AnchorWithText: {
        if (hasActiveAnchor()) {
            mgr.printByHyperlink( activeAnchor(), displayOptions, filterOptions );
        }
    }

    default:
        break;
    }
}

