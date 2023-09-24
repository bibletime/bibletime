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

#include <QRegularExpression>
#include <QString>
#include <QtGlobal>
#include "../../util/btassert.h"
#include "../config/btconfig.h"
#include "../drivers/cswordmoduleinfo.h"
#include "../keys/cswordkey.h"
#include "../keys/cswordversekey.h"
#include "../managers/cdisplaytemplatemgr.h"
#include "../managers/referencemanager.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsuggest-destructor-override"
#endif
#include <swmodule.h>
#include <listkey.h>
#include <versekey.h> // For search scope configuration
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#pragma GCC diagnostic pop

namespace Rendering {

CDisplayRendering::CDisplayRendering()
    : CDisplayRendering(btConfig().getDisplayOptions(),
                        btConfig().getFilterOptions())
{};

CDisplayRendering::CDisplayRendering(DisplayOptions const & displayOptions,
                                     FilterOptions const & filterOptions)
    : CTextRendering(true, displayOptions, filterOptions)
{}

QString CDisplayRendering::renderDisplayEntry(
        BtConstModuleList const & modules,
        QString const & keyName,
        CTextRendering::KeyTreeItem::Settings::KeyRenderingFace keyRendering)
        const
{
    BT_ASSERT(!keyName.isEmpty());

    //no highlighted key and no extra key link in the text
    const CSwordModuleInfo *module = modules.first();

    Rendering::CTextRendering::KeyTree tree;

    //in Bibles and Commentaries we need to check if 0:0 and X:0 contain something
    if (module->type() == CSwordModuleInfo::Bible
        || module->type() == CSwordModuleInfo::Commentary)
    {
        // HACK: enable headings for VerseKeys
        static_cast<sword::VerseKey *>(module->swordModule().getKey())
                ->setIntros(true);

        CSwordVerseKey k1(module);
        k1.setIntros(true);
        k1.setKey(keyName);

        // don't print the key
        CTextRendering::KeyTreeItem::Settings preverse_settings{
            false,
            CTextRendering::KeyTreeItem::Settings::NoKey};

        if (k1.verse() == 1) { // X:1, prepend X:0
            if (k1.chapter() == 1) { // 1:1, also prepend 0:0 before that
                k1.setChapter(0);
                k1.setVerse(0);
                if (k1.rawText().length() > 0)
                    tree.emplace_back(k1.key(), modules, preverse_settings);
                k1.setChapter(1);
            }
            k1.setVerse(0);
            if (k1.rawText().length() > 0)
                tree.emplace_back(k1.key(), modules, preverse_settings);
        }
    }
    using Settings = CTextRendering::KeyTreeItem::Settings;
    tree.emplace_back(keyName, modules, Settings{false, keyRendering});
    return renderKeyTree(tree);
}

QString CDisplayRendering::entryLink(KeyTreeItem const & item,
                                     CSwordModuleInfo const & module) const
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
        [[fallthrough]];

    case KeyTreeItem::Settings::CompleteShort:
        if (isBible) {
            linkText = vk.shortText();
            break;
        }
        [[fallthrough]];

    case KeyTreeItem::Settings::ExpandedLong:
        if (isBible) {
            linkText = QStringLiteral("%1 (%2)").arg(vk.key(), module.name());
            break;
        }
        [[fallthrough]];

    case KeyTreeItem::Settings::CompleteLong:
        if (isBible) {
            linkText = vk.key();
            break;
        }
        [[fallthrough]];

    case KeyTreeItem::Settings::SimpleKey:
        if (isBible) {
            if(item.mappedKey() != nullptr) {
                CSwordVerseKey baseKey(*item.modules().begin());
                baseKey.setKey(item.key());

                if (vk.bookName() != baseKey.bookName()) {
                    linkText = vk.shortText();
                } else if (vk.chapter() != baseKey.chapter()) {
                    linkText =
                            QStringLiteral("%1:%2")
                            .arg(vk.chapter())
                            .arg(vk.verse());
                } else {
                    linkText = QString::number(vk.verse());
                }

                if(vk.isBoundSet()) {
                    linkText += '-';
                    auto const upper = vk.upperBound();
                    auto const lower = vk.lowerBound();
                    if (upper.book() != lower.book()) {
                        linkText += upper.shortText();
                    } else if(upper.chapter() != lower.chapter()) {
                        linkText += QStringLiteral("%1:%2").arg(upper.chapter())
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
        [[fallthrough]];

    default: //default behaviour to return the passed key
        linkText = item.key();
        break;

    }


    if (linkText.isEmpty()) {
        return QStringLiteral("<a name=\"%1\"></a>").arg(
                    keyToHTMLAnchor(item.key()));
    }
    else {
        return QStringLiteral("<a name=\"%1\" href=\"%2\">%3</a>\n")
                .arg(keyToHTMLAnchor(item.key()),
                     ReferenceManager::encodeHyperlink(module, item.key()),
                     linkText);
    }
}

QString CDisplayRendering::keyToHTMLAnchor(QString const & key) {
    // Be careful not to remove non-ASCII characters, this causes problems
    // with many languages.
    static QRegularExpression const re(QStringLiteral(R"PCRE(\s)PCRE"));
    return key.trimmed().remove(re).replace(':', '_');
}

QString
CDisplayRendering::finishText(QString const & text, KeyTree const & tree) const
{
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

    return tMgr->fillTemplate(m_displayTemplateName.isEmpty()
                              ? CDisplayTemplateMgr::activeTemplateName()
                              : m_displayTemplateName,
                              text,
                              settings);
}
}
