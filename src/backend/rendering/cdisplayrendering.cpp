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

#include "cdisplayrendering.h"

#include <QRegExp>
#include <QString>
#include <QtGlobal>
#include "../config/btconfig.h"
#include "../keys/cswordkey.h"
#include "../keys/cswordversekey.h"
#include "../managers/cdisplaytemplatemgr.h"
#include "../managers/referencemanager.h"


namespace Rendering {

CDisplayRendering::CDisplayRendering(const DisplayOptions &displayOptions,
                                     const FilterOptions &filterOptions)
        : CTextRendering(true, displayOptions, filterOptions)
{
    // Intentionally empty
}

QString CDisplayRendering::entryLink(KeyTreeItem const & item,
                                     CSwordModuleInfo const & module)
{
    QString linkText;

    const bool isBible = module.type() == CSwordModuleInfo::Bible;
    CSwordVerseKey vk(&module); // only valid for bible modules, i.e. isBible == true
    vk.setIntros(true);

    if (isBible) {
        vk.setKey(item.mappedKey() ? item.mappedKey()->key() : item.key());
    }

    if (isBible && (vk.verse() == 0)) {
        return QString(); //Warning: return already here
    }

    switch (item.settings().keyRenderingFace) {

    case KeyTreeItem::Settings::NoKey:
        linkText = QString();
        break; //no key is valid for all modules

    case KeyTreeItem::Settings::ExpandedShort:
        if (isBible) {
            linkText = module.name() + ':' + vk.shortText();
            break;
        }
        Q_FALLTHROUGH();

    case KeyTreeItem::Settings::CompleteShort:
        if (isBible) {
            linkText = vk.shortText();
            break;
        }
        Q_FALLTHROUGH();

    case KeyTreeItem::Settings::ExpandedLong:
        if (isBible) {
            linkText = QString("%1 (%2)").arg(vk.key()).arg(module.name());
            break;
        }
        Q_FALLTHROUGH();

    case KeyTreeItem::Settings::CompleteLong:
        if (isBible) {
            linkText = vk.key();
            break;
        }
        Q_FALLTHROUGH();

    case KeyTreeItem::Settings::SimpleKey:
        if (isBible) {
            if(item.mappedKey() != nullptr) {
                CSwordVerseKey baseKey(*item.modules().begin());
                baseKey.setKey(item.key());

                if (vk.bookName() != baseKey.bookName()) {
                    linkText = vk.shortText();
                } else if (vk.chapter() != baseKey.chapter()) {
                    linkText = QString("%1:%2").arg(vk.chapter()).arg(vk.verse());
                } else {
                    linkText = QString::number(vk.verse());
                }

                if(vk.isBoundSet()) {
                    linkText += "-";
                    auto const upper = vk.upperBound();
                    auto const lower = vk.lowerBound();
                    if (upper.book() != lower.book()) {
                        linkText += upper.shortText();
                    } else if(upper.chapter() != lower.chapter()) {
                        linkText += QString("%1:%2").arg(upper.chapter())
                                                    .arg(lower.verse());
                    } else {
                        linkText += QString::number(upper.verse());
                    }
                }
            } else {
                linkText = QString::number(vk.verse());
            }
            break;
        } // else fall through for non-Bible modules
        Q_FALLTHROUGH();

    default: //default behaviour to return the passed key
        linkText = item.key();
        break;

    }


    if (linkText.isEmpty()) {
        return QString("<a name=\"").append(keyToHTMLAnchor(item.key())).append("\"></a>");
    }
    else {
        return QString("<a name=\"").append(keyToHTMLAnchor(item.key())).append("\" ")
               .append("href=\"")
               .append(ReferenceManager::encodeHyperlink(module, item.key()))
               .append("\">").append(linkText).append("</a>\n");
    }
}

QString CDisplayRendering::keyToHTMLAnchor(const QString& key) {
    // Be careful not to remove non-ASCII characters, this causes problems
    // with many languages.
    return key.trimmed().remove(QRegExp("\\s")).replace(QString(":"), QString("_"));
}

QString CDisplayRendering::finishText(const QString &text, const KeyTree &tree) {
    BtConstModuleList modules = collectModules(tree);

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

    CDisplayTemplateMgr *tMgr = CDisplayTemplateMgr::instance();

    //BT_ASSERT(modules.count() >= 1);

    CDisplayTemplateMgr::Settings settings;
    settings.modules = modules;
    if (modules.count() == 1)
        if (auto const lang = modules.first()->language())
            if (auto const & abbrev = lang->abbrev(); !abbrev.isEmpty())
                settings.langAbbrev = abbrev;

    if (modules.count() == 1)
        settings.textDirection = modules.first()->textDirection();

    return tMgr->fillTemplate(CDisplayTemplateMgr::activeTemplateName(), text, settings);
}
}
