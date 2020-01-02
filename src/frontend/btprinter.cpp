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

#include "btprinter.h"

#include <QPrintDialog>
#include <QPrinter>
#include <QTextEdit>
#include "../backend/config/btconfig.h"
#include "../backend/keys/cswordversekey.h"
#include "../backend/managers/cdisplaytemplatemgr.h"
#include "../util/btassert.h"
#include "bibletime.h"


namespace {

inline FilterOptions mangleFilterOptions(FilterOptions fo) {
    fo.footnotes = false;
    fo.scriptureReferences = false;
    fo.strongNumbers = false;
    fo.morphTags = false;
    fo.headings = false;
    return fo;
}

} // anonymous namespace

BtPrinter::BtPrinter(DisplayOptions const & displayOptions,
                     FilterOptions const & filterOptions,
                     QObject * const parent)
        : QObject{parent}
        , CDisplayRendering{displayOptions, mangleFilterOptions(filterOptions)}
{}

void BtPrinter::printKeyTree(KeyTree const & tree) {
    QTextEdit htmlPage;
    htmlPage.setHtml(renderKeyTree(tree));

    QPrinter printer;
    QPrintDialog printDialog(&printer);
    if (printDialog.exec() == QDialog::Accepted)
        htmlPage.print(&printer);
}

QString BtPrinter::entryLink(KeyTreeItem const & item,
                             CSwordModuleInfo const * module)
{
    BT_ASSERT(module);
    if (module->type() != CSwordModuleInfo::Bible)
        return item.key();

    CSwordVerseKey vk(module);
    vk.setKey(item.key());
    switch (item.settings().keyRenderingFace) {
        case KeyTreeItem::Settings::CompleteShort:
            return QString::fromUtf8(vk.getShortText());

        case KeyTreeItem::Settings::CompleteLong:
            return vk.key();

        case KeyTreeItem::Settings::NoKey:
            return QString();

        case KeyTreeItem::Settings::SimpleKey: // fall through:
        default:
            return QString::number(vk.getVerse());
    }
}

QString BtPrinter::renderEntry(KeyTreeItem const & i, CSwordKey * key) {
    Q_UNUSED(key);
    BT_ASSERT(dynamic_cast<BtPrinter::KeyTreeItem const *>(&i));
    BtPrinter::KeyTreeItem const * const printItem =
            static_cast<BtPrinter::KeyTreeItem const *>(&i);

    if (printItem->hasAlternativeContent()) {
        QString ret =
                QString::fromLatin1("<div class=\"entry\"><div class=\""
                                    "rangeheading\">%1</div>").arg(
                                            printItem->getAlternativeContent());
        if (!i.childList()->isEmpty())
            Q_FOREACH (const KeyTreeItem * const c, *i.childList())
                ret.append(CDisplayRendering::renderEntry(*c));
        ret.append("</div>");
        return ret;
    }
    return CDisplayRendering::renderEntry(i);
}

QString BtPrinter::finishText(QString const & text, KeyTree const & tree) {
    BtConstModuleList const modules = collectModules(tree);
    BT_ASSERT(!modules.empty());

    CLanguageMgr::Language const * const lang = modules.first()->language();
    BT_ASSERT(lang);

    CDisplayTemplateMgr::Settings settings;
    //settings.modules = modules;
    settings.pageCSS_ID = "printer";
    if (modules.count() == 1 && lang->isValid())
        settings.langAbbrev = lang->abbrev();

    if (modules.count() == 1)
        settings.textDirection = modules.first()->textDirection();

    return CDisplayTemplateMgr::instance()->fillTemplate(
                CDisplayTemplateMgr::activeTemplateName(),
                text,
                settings);
}
