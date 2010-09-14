/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/crossrefrendering.h"

#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordversekey.h"
#include "backend/managers/referencemanager.h"


namespace InfoDisplay {

CrossRefRendering::CrossRefRendering( CSwordBackend::DisplayOptions displayOptions,
                                      CSwordBackend::FilterOptions filterOptions
                                    )
        : CHTMLExportRendering(Settings(), displayOptions, filterOptions) {}

const QString CrossRefRendering::finishText( const QString& text, KeyTree& ) {
    //   qDebug() << "CrossRefRendering::finishText";
    return text;
}

const QString CrossRefRendering::entryLink( const KeyTreeItem& item, CSwordModuleInfo*  module ) {
    QString linkText;

    const bool isBible = module && (module->type() == CSwordModuleInfo::Bible);
    CSwordVerseKey vk(module); //only valid for bible modules, i.e. isBible == true
    if (isBible) {
        vk.key(item.key());
    }

    switch (item.settings().keyRenderingFace) {
        case KeyTreeItem::Settings::NoKey: {
            linkText = QString::null;
            break; //no key is valid for all modules
        }
        case KeyTreeItem::Settings::CompleteShort: {
            if (isBible) {
                linkText = QString::fromUtf8(vk.getShortText());
                break;
            }
            //fall through for non-Bible modules
        }
        case KeyTreeItem::Settings::CompleteLong: {
            if (isBible) {
                linkText = vk.key();
                break;
            }
            //fall through for non-Bible modules
        }
        case KeyTreeItem::Settings::SimpleKey: {
            if (isBible) {
                linkText = QString::number(vk.Verse());
                break;
            }
            //fall through for non-Bible modules
        }
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

    return QString::null;
}

}
