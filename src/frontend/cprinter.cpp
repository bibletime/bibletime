/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/cprinter.h"

#include <QPrintDialog>
#include <QPrinter>
#include <QWebFrame>
#include <QWebPage>
#include "backend/keys/cswordversekey.h"
#include "backend/managers/cdisplaytemplatemgr.h"
#include "backend/config/btconfig.h"


namespace Printing {

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

CPrinter::CPrinter(DisplayOptions const & displayOptions,
                   FilterOptions const & filterOptions,
                   QObject * const parent)
        : QObject{parent}
        , CDisplayRendering{displayOptions, mangleFilterOptions(filterOptions)}
{}

void CPrinter::printKeyTree(KeyTree const & tree) {
    QWebPage htmlPage;
    htmlPage.mainFrame()->setHtml(renderKeyTree(tree));

    QPrinter printer;
    QPrintDialog printDialog(&printer);
    if (printDialog.exec() == QDialog::Accepted)
        htmlPage.mainFrame()->print(&printer);
}

QString CPrinter::entryLink(KeyTreeItem const & item,
                            CSwordModuleInfo const * module)
{
    Q_ASSERT(module);
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
            return QString::null;

        case KeyTreeItem::Settings::SimpleKey: // fall through:
        default:
            return QString::number(vk.getVerse());
    }
}

QString CPrinter::renderEntry(KeyTreeItem const & i, CSwordKey * key) {
    Q_UNUSED(key);
    Q_ASSERT(dynamic_cast<CPrinter::KeyTreeItem const *>(&i));
    CPrinter::KeyTreeItem const * const printItem =
            static_cast<CPrinter::KeyTreeItem const *>(&i);

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

QString CPrinter::finishText(QString const & text, KeyTree const & tree) {
    BtConstModuleList const modules = collectModules(tree);
    Q_ASSERT(!modules.empty());

    CLanguageMgr::Language const * const lang = modules.first()->language();
    Q_ASSERT(lang);

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

} //end of namespace
