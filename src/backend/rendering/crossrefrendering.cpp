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

#include "crossrefrendering.h"

#include <QtGlobal>
#include "../drivers/cswordmoduleinfo.h"
#include "../keys/cswordversekey.h"
#include "../managers/referencemanager.h"


namespace Rendering {

CrossRefRendering::CrossRefRendering(const DisplayOptions &displayOptions,
                                     const FilterOptions &filterOptions)
        : CTextRendering(true, displayOptions, filterOptions)
{
    // Intentionally empty
}

QString CrossRefRendering::finishText(QString const & text,
                                      KeyTree const & tree) const
{
    Q_UNUSED(tree)
    return text;
}

QString CrossRefRendering::entryLink(KeyTreeItem const & item,
                                     CSwordModuleInfo const & module) const
{

    bool const isBible = (module.type() == CSwordModuleInfo::Bible);
    CSwordVerseKey vk(&module); // only valid for bible modules, i.e. isBible == true
    if (isBible)
        vk.setKey(item.key());

    QString linkText;
    switch (item.settings().keyRenderingFace) {
        case KeyTreeItem::Settings::NoKey:
            return {}; //no key is valid for all modules
        case KeyTreeItem::Settings::CompleteShort:
            if (isBible) {
                linkText = vk.shortText();
                break;
            }
            //fall through for non-Bible modules
            [[fallthrough]];
        case KeyTreeItem::Settings::CompleteLong:
            if (isBible) {
                linkText = vk.key();
                break;
            }
            //fall through for non-Bible modules
            [[fallthrough]];
        case KeyTreeItem::Settings::SimpleKey:
            if (isBible) {
                linkText = QString::number(vk.verse());
                break;
            }
            //fall through for non-Bible modules
            [[fallthrough]];
        default: { //default behaviour to return the passed key
            linkText = item.key();
            break;
        }
    }

    if (linkText.isEmpty()) // if we have an invalid link text
        return {};

    return QStringLiteral("<a href=\"%1\">%2</a>")
           .arg(ReferenceManager::encodeHyperlink(module, item.key()),
                linkText);
}

} // namespace Rendering
