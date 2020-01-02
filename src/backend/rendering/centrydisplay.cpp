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

#include "centrydisplay.h"

#include <QApplication>
#include <QRegExp>

#include "../drivers/cswordbookmoduleinfo.h"
#include "../keys/cswordkey.h"
#include "../keys/cswordversekey.h"
#include "../managers/cdisplaytemplatemgr.h"
#include "../managers/referencemanager.h"
#include "cdisplayrendering.h"
#include "ctextrendering.h"


using namespace Rendering;

const QString CEntryDisplay::text(
        const BtConstModuleList &modules,
        const QString &keyName,
        const DisplayOptions &displayOptions,
        const FilterOptions &filterOptions) {
    return textKeyRendering(modules, keyName, displayOptions, filterOptions,
                     CTextRendering::KeyTreeItem::Settings::CompleteShort);
}

const QString CEntryDisplay::textKeyRendering(
        const BtConstModuleList &modules,
        const QString &keyName,
        const DisplayOptions &displayOptions,
        const FilterOptions &filterOptions,
        CTextRendering::KeyTreeItem::Settings::KeyRenderingFace keyRendering) {

    if (keyName.isEmpty())
        return QString("");

    CDisplayRendering render(displayOptions, filterOptions);

    //no highlighted key and no extra key link in the text
    CTextRendering::KeyTreeItem::Settings normal_settings{false, keyRendering};
    const CSwordModuleInfo *module = modules.first();

    Rendering::CTextRendering::KeyTree tree;

    //in Bibles and Commentaries we need to check if 0:0 and X:0 contain something
    if (module->type() == CSwordModuleInfo::Bible || module->type() == CSwordModuleInfo::Commentary) {
        // HACK: enable headings for VerseKeys
        static_cast<sword::VerseKey *>(module->module().getKey())
                ->setIntros(true);

        CSwordVerseKey k1(module);
        k1.setIntros(true);
        k1.setKey(keyName);

        // don't print the key
        CTextRendering::KeyTreeItem::Settings preverse_settings{
            false,
            CTextRendering::KeyTreeItem::Settings::NoKey};

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
    return render.renderKeyTree(tree);
}
