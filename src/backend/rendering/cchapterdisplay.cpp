/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/rendering/cchapterdisplay.h"

#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/keys/cswordversekey.h"
#include "backend/rendering/cdisplayrendering.h"


const QString Rendering::CChapterDisplay::text(
        const QList<const CSwordModuleInfo*> &modules,
        const QString &keyName,
        const DisplayOptions &displayOptions,
        const FilterOptions &filterOptions)
{
    typedef CSwordBibleModuleInfo CSBMI;

    Q_ASSERT( modules.count() >= 1 );
    Q_ASSERT( !keyName.isEmpty() );

    const CSwordModuleInfo *module = modules.first();

    if (modules.count() == 1) module->module()->setSkipConsecutiveLinks( true ); //skip empty, linked verses

    CTextRendering::KeyTreeItem::Settings settings;
    settings.keyRenderingFace =
        displayOptions.verseNumbers
        ? CTextRendering::KeyTreeItem::Settings::SimpleKey
        : CTextRendering::KeyTreeItem::Settings::NoKey;

    QString startKey = keyName;
    QString endKey = startKey;

    //check whether there's an intro we have to include
    Q_ASSERT((module->type() == CSwordModuleInfo::Bible));

    if (module->type() == CSwordModuleInfo::Bible) {
        ((sword::VerseKey*)(module->module()->getKey()))->Headings(1); //HACK: enable headings for VerseKeys

        Q_ASSERT(dynamic_cast<const CSBMI*>(module) != 0);
        const CSBMI *bible = static_cast<const CSBMI*>(module);

        CSwordVerseKey k1(module);
        k1.Headings(1);
        k1.setKey(keyName);

        if (k1.Chapter() == 1)    k1.Chapter(0); //Chapter 1, start with 0:0, otherwise X:0

        k1.Verse(0);

        startKey = k1.key();

        if (k1.Chapter() == 0) k1.Chapter(1);
        k1.Verse(bible->verseCount(k1.book(), k1.Chapter()));
        endKey = k1.key();
    }

    CDisplayRendering render(displayOptions, filterOptions);
    return render.renderKeyRange( startKey, endKey, modules, keyName, settings );
}
