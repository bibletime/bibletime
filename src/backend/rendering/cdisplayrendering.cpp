/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/rendering/cdisplayrendering.h"

#include <QString>
#include <QRegExp>
#include <QDebug>
#include "backend/keys/cswordkey.h"
#include "backend/keys/cswordversekey.h"
#include "backend/managers/cdisplaytemplatemgr.h"
#include "backend/managers/referencemanager.h"
#include "backend/config/btconfig.h"


namespace Rendering {

CDisplayRendering::CDisplayRendering(const DisplayOptions &displayOptions,
                                     const FilterOptions &filterOptions)
        : CHTMLExportRendering(CHTMLExportRendering::Settings(true),
                               displayOptions, filterOptions)
{
    // Intentionally empty
}

const QString CDisplayRendering::entryLink(const KeyTreeItem &item,
                                           const CSwordModuleInfo *module)
{
    QString linkText;

    const bool isBible = module && (module->type() == CSwordModuleInfo::Bible);
    CSwordVerseKey vk(module); //only valid for bible modules, i.e. isBible == true
    vk.Headings(true);

    if (isBible) {
        vk.setKey(item.key());
    }

    if (isBible && (vk.Verse() == 0)) {
        return QString::null; //Warning: return already here
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


    if (linkText.isEmpty()) {
        return QString("<a name=\"").append(keyToHTMLAnchor(item.key())).append("\"></a>");
    }
    else {
        return QString("<a name=\"").append(keyToHTMLAnchor(item.key())).append("\" ")
               .append("href=\"")
               .append(ReferenceManager::encodeHyperlink(
                           module->name(), item.key(), ReferenceManager::typeFromModule(module->type()))
                      )
               .append("\">").append(linkText).append("</a>\n");
    }

    return QString::null;
}

const QString CDisplayRendering::keyToHTMLAnchor(const QString& key) {
    QString ret = key;
    // Be careful not to remove non-ASCII characters, this causes problems
    // with many languages.
    ret = ret.trimmed().remove(QRegExp("\\s")).replace(QString(":"), QString("_"));

    return ret;
}

const QString CDisplayRendering::finishText( const QString& oldText, KeyTree& tree ) {
    QList<const CSwordModuleInfo*> modules = collectModules(&tree);
    qDebug() << "CDisplayRendering::finishText";

    //marking words is very slow, we have to find a better solution

    /*
     //mark all words by spans

     QString text = oldText;

     QRegExp re("(\\b)(?=\\w)"); //word begin marker
     int pos = text.find(re, 0);

     while (pos != -1) { //word begin found
      //qWarning("found word at %i in %i", pos, text.length());
      int endPos = pos + 1;
      if (!util::tool::inHTMLTag(pos+1, text)) { //the re has a positive look ahead which matches one char before the word start
       //qWarning("matched %s", text.mid(pos+1, 4).latin1());

       //find end of word and put a marker around it
       endPos = text.find(QRegExp("\\b|[,.:]"), pos+1);
       if ((endPos != -1) && !util::tool::inHTMLTag(endPos, text) && (endPos - pos >= 3)) { //reuire wordslonger than 3 chars
        text.insert(endPos, "</span>");
        text.insert(pos, "<span class=\"word\">");

        endPos += 26;
       }
      }
      pos = text.find(re, endPos);
     }
    */
    const CLanguageMgr::Language* const lang =
        (modules.count() >= 1)
        ? modules.first()->language()
        : CLanguageMgr::instance()->defaultLanguage();

    CDisplayTemplateMgr *tMgr = CDisplayTemplateMgr::instance();

    //Q_ASSERT(modules.count() >= 1);

    CDisplayTemplateMgr::Settings settings;
    settings.modules = modules;
    settings.langAbbrev = ((modules.count() == 1) && lang->isValid()) ? lang->abbrev() : QString::null;

    if (modules.count() == 1)
        settings.pageDirection = (modules.first()->textDirection() == CSwordModuleInfo::LeftToRight) ? "ltr"  : "rtl";
    else
        settings.pageDirection = QString::null;

    return tMgr->fillTemplate(getBtConfig().getValue<QString>("gui/displayStyle"), oldText, settings);
}
}
