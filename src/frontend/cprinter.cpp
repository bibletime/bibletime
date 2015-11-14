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

inline FilterOptions mangleFilterOptions(FilterOptions const & fo) {
    FilterOptions r(fo);
    r.footnotes = false;
    r.scriptureReferences = false;
    r.strongNumbers = false;
    r.morphTags = false;
    r.headings = false;
    return r;
}

} // anonymous namespace

/// \todo WHY IS parent NOT USED!?
CPrinter::CPrinter(QObject *,
                   const DisplayOptions &displayOptions,
                   const FilterOptions &filterOptions)
        : QObject(nullptr),
          CDisplayRendering(displayOptions, mangleFilterOptions(filterOptions)),
          m_htmlPage(new QWebPage())
{ m_htmlPage->setParent(this); }

CPrinter::~CPrinter() {
    delete m_htmlPage;
    m_htmlPage = nullptr;
}

void CPrinter::printKeyTree( KeyTree& tree ) {
    m_htmlPage->mainFrame()->setHtml(renderKeyTree(tree));

    QPrinter printer;
    QPrintDialog printDialog(&printer);
    if (printDialog.exec() == QDialog::Accepted) {
        m_htmlPage->mainFrame()->print(&printer);
    }
}

QString CPrinter::entryLink(const KeyTreeItem &item,
                            const CSwordModuleInfo * module)
{
    Q_ASSERT(module);
    if (module->type() == CSwordModuleInfo::Bible) {
        CSwordVerseKey vk(module);
        vk.setKey(item.key());
        switch (item.settings().keyRenderingFace) {
            case KeyTreeItem::Settings::CompleteShort:
                return QString::fromUtf8(vk.getShortText());

            case KeyTreeItem::Settings::CompleteLong:
                return vk.key();

            case KeyTreeItem::Settings::NoKey:
                return QString::null;

            case KeyTreeItem::Settings::SimpleKey: //fall through
            default:
                return QString::number(vk.getVerse());
        }
    }
    return item.key();
}

QString CPrinter::renderEntry(const KeyTreeItem &i, CSwordKey * key) {
    Q_UNUSED(key);

    const CPrinter::KeyTreeItem* printItem = dynamic_cast<const CPrinter::KeyTreeItem*>(&i);
    Q_ASSERT(printItem);

    if (printItem && printItem->hasAlternativeContent()) {
        QString ret = QString::fromLatin1("<div class=\"entry\"><div class=\"rangeheading\">%1</div>").arg(printItem->getAlternativeContent());

        if (!i.childList()->isEmpty()) {
            KeyTree const * tree = i.childList();

            Q_FOREACH (const KeyTreeItem * const c, *tree)
                ret.append( CDisplayRendering::renderEntry( *c ) );
        }

        ret.append("</div>");
        return ret;
    }
    return CDisplayRendering::renderEntry(i);
}

QString CPrinter::finishText(const QString &text, const KeyTree &tree) {
    BtConstModuleList modules = collectModules(tree);
    Q_ASSERT(modules.count() > 0);

    const CLanguageMgr::Language* const lang = modules.first()->language();
    Q_ASSERT(lang);

    CDisplayTemplateMgr::Settings settings;
    //settings.modules = modules;
    settings.pageCSS_ID = "printer";
    if (modules.count() == 1 && lang->isValid())
        settings.langAbbrev = lang->abbrev();

    if (modules.count() == 1)
        settings.textDirection = modules.first()->textDirection();

    CDisplayTemplateMgr *tMgr = CDisplayTemplateMgr::instance();
    return tMgr->fillTemplate(CDisplayTemplateMgr::activeTemplateName(), text, settings);
}

} //end of namespace
