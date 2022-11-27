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

#include "btprinter.h"

#include <QPrintDialog>
#include <QPrinter>
#include <QTextEdit>
#include "../backend/keys/cswordversekey.h"
#include "../backend/managers/cdisplaytemplatemgr.h"
#include "../util/btassert.h"


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
                             CSwordModuleInfo const & module) const
{
    if (module.type() != CSwordModuleInfo::Bible)
        return item.key();

    CSwordVerseKey vk(&module);
    vk.setKey(item.key());
    switch (item.settings().keyRenderingFace) {
        case KeyTreeItem::Settings::CompleteShort:
            return vk.shortText();

        case KeyTreeItem::Settings::CompleteLong:
            return vk.key();

        case KeyTreeItem::Settings::NoKey:
            return QString();

        case KeyTreeItem::Settings::SimpleKey: // fall through:
        default:
            return QString::number(vk.verse());
    }
}

QString BtPrinter::renderEntry(KeyTreeItem const & i, CSwordKey * key) const {
    Q_UNUSED(key)
    BtPrinter::KeyTreeItem const * const printItem =
            static_cast<BtPrinter::KeyTreeItem const *>(&i);

    if (printItem->hasAlternativeContent()) {
        auto ret =
                QStringLiteral(
                    "<div class=\"entry\"><div class=\"rangeheading\">%1</div>")
                .arg(printItem->getAlternativeContent());
        if (!i.childList().empty())
            for (auto const & item : i.childList())
                ret.append(CDisplayRendering::renderEntry(item));
        ret.append(QStringLiteral("</div>"));
        return ret;
    }
    return CDisplayRendering::renderEntry(i);
}

QString BtPrinter::finishText(QString const & text, KeyTree const & tree) const
{
    BtConstModuleList const modules = collectModules(tree);
    BT_ASSERT(!modules.empty());

    CDisplayTemplateMgr::Settings settings;
    //settings.modules = modules;
    settings.pageCSS_ID = QStringLiteral("printer");
    if (modules.count() == 1)
        settings.langAbbrev = modules.first()->language()->abbrev();

    if (modules.count() == 1)
        settings.textDirection = modules.first()->textDirection();

    return CDisplayTemplateMgr::instance()->fillTemplate(
                CDisplayTemplateMgr::activeTemplateName(),
                text,
                settings);
}
