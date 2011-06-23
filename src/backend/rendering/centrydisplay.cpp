/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/rendering/centrydisplay.h"

#include <QApplication>
#include <QRegExp>

#include "backend/drivers/cswordbookmoduleinfo.h"
#include "backend/keys/cswordkey.h"
#include "backend/keys/cswordversekey.h"
#include "backend/managers/cdisplaytemplatemgr.h"
#include "backend/managers/referencemanager.h"
#include "backend/rendering/cdisplayrendering.h"
#include "backend/rendering/ctextrendering.h"


using namespace Rendering;

const QString CEntryDisplay::text(
        const QList<const CSwordModuleInfo*> &modules,
        const QString &keyName,
        const DisplayOptions &displayOptions,
        const FilterOptions &filterOptions)
{
    CDisplayRendering render(displayOptions, filterOptions);

    //no highlighted key and no extra key link in the text
    CTextRendering::KeyTreeItem::Settings normal_settings(false, CTextRendering::KeyTreeItem::Settings::CompleteShort);
    const CSwordModuleInfo *module = modules.first();

    Rendering::CTextRendering::KeyTree tree;

    //in Bibles and Commentaries we need to check if 0:0 and X:0 contain something
    if (module->type() == CSwordModuleInfo::Bible || module->type() == CSwordModuleInfo::Commentary) {
        ((sword::VerseKey*)(module->module()->getKey()))->Headings(1); //HACK: enable headings for VerseKeys

        CSwordVerseKey k1(module);
        k1.Headings(1);
        k1.setKey(keyName);

        // don't print the key
        CTextRendering::KeyTreeItem::Settings preverse_settings(false, CTextRendering::KeyTreeItem::Settings::NoKey);

        if (k1.getVerse() == 1) { // X:1, prepend X:0
            if (k1.getChapter() == 1) { // 1:1, also prepend 0:0 before that
                k1.setChapter(0);
                k1.setVerse(0);
                if ( k1.rawText().length() > 0 ) {
                    tree.append( new Rendering::CTextRendering::KeyTreeItem(k1.key(), modules, preverse_settings) );
                }
                k1.setChapter(1);
            }
            k1.setVerse(0);
            if ( k1.rawText().length() > 0 ) {
                tree.append( new Rendering::CTextRendering::KeyTreeItem(k1.key(), modules, preverse_settings) );
            }
        }
    }
    tree.append( new Rendering::CTextRendering::KeyTreeItem(keyName, modules, normal_settings) );
    QString result(render.renderKeyTree(tree));
    qDeleteAll(tree);
    return result;
}
