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

#include "crossrefrendering.h"

#include <QtGlobal>
#include "../backend/drivers/cswordmoduleinfo.h"
#include "../backend/keys/cswordversekey.h"
#include "../backend/managers/referencemanager.h"
#include "../util/btassert.h"


namespace InfoDisplay {

CrossRefRendering::CrossRefRendering(const DisplayOptions &displayOptions,
                                     const FilterOptions &filterOptions)
        : CHTMLExportRendering(true, displayOptions, filterOptions)
{
    // Intentionally empty
}

QString CrossRefRendering::finishText(const QString &text, const KeyTree &tree) {
    Q_UNUSED(tree);
    return text;
}

QString CrossRefRendering::entryLink(const KeyTreeItem &item,
                                     const CSwordModuleInfo *module)
{
    BT_ASSERT(module);
    QString linkText;

    const bool isBible = (module->type() == CSwordModuleInfo::Bible);
    CSwordVerseKey vk(module); //only valid for bible modules, i.e. isBible == true
    if (isBible) {
        vk.setKey(item.key());
    }

    switch (item.settings().keyRenderingFace) {
        case KeyTreeItem::Settings::NoKey:
            linkText = QString();
            break; //no key is valid for all modules
        case KeyTreeItem::Settings::CompleteShort:
            if (isBible) {
                linkText = QString::fromUtf8(vk.getShortText());
                break;
            }
            //fall through for non-Bible modules
            Q_FALLTHROUGH();
        case KeyTreeItem::Settings::CompleteLong:
            if (isBible) {
                linkText = vk.key();
                break;
            }
            //fall through for non-Bible modules
            Q_FALLTHROUGH();
        case KeyTreeItem::Settings::SimpleKey:
            if (isBible) {
                linkText = QString::number(vk.getVerse());
                break;
            }
            //fall through for non-Bible modules
            Q_FALLTHROUGH();
        default: { //default behaviour to return the passed key
            linkText = item.key();
            break;
        }
    }

    if (!linkText.isEmpty()) { //if we have a valid link text
        //     qWarning("rendering");
        return QString("<a href=\"%1\">%2</a>")
               .arg(
                   ReferenceManager::encodeHyperlink(
                       module->name(),
                       item.key(),
                       ReferenceManager::typeFromModule(module->type())
                   )
               )
               .arg(linkText);
    }

    return QString();
}

}
