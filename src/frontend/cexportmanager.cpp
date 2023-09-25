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

#include "cexportmanager.h"

#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QList>
#include <QProgressDialog>
#include <QTextStream>
#include "../backend/drivers/cswordmoduleinfo.h"
#include "../backend/keys/cswordkey.h"
#include "../backend/keys/cswordversekey.h"
#include "../backend/managers/referencemanager.h"
#include "../backend/rendering/cplaintextexportrendering.h"
#include "../backend/rendering/ctextrendering.h"
#include "../util/btassert.h"
#include "../util/tool.h"
#include "btprinter.h"

// Sword includes:
#include <swkey.h>
#include <listkey.h>


using namespace Rendering;

using KTI = CTextRendering::KeyTreeItem;

CExportManager::CExportManager(bool const showProgress,
                               QString const & progressLabel,
                               FilterOptions const & filterOptions,
                               DisplayOptions const & displayOptions)
    : m_filterOptions(filterOptions)
    , m_displayOptions(displayOptions)
    , m_progressDialog(
        showProgress
        ? [&progressLabel]{
            auto dialog =
                    std::make_unique<QProgressDialog>(nullptr, Qt::Dialog);
            dialog->setWindowTitle(QStringLiteral("BibleTime"));
            dialog->setLabelText(progressLabel);
            return dialog;
        }()
        : nullptr)
{}

CExportManager::~CExportManager() = default;

bool CExportManager::saveKey(CSwordKey const * const key,
                             Format const format,
                             bool const addText,
                             const BtConstModuleList& modules)
{
    if (!key || !key->module())
        return false;
    QString const filename = getSaveFileName(format);
    if (filename.isEmpty())
        return false;

    QString text;
    {
        CSwordVerseKey const * const vk =
                dynamic_cast<CSwordVerseKey const *>(key);
        auto const render = newRenderer(format, addText);
        if (vk && vk->isBoundSet()) {
            text = render->renderKeyRange(vk->lowerBound(),
                                          vk->upperBound(),
                                          modules);
            text.replace(QStringLiteral("#CHAPTERTITLE#"),
                         QStringLiteral("%1 %2")
                         .arg(vk->bookName())
                         .arg(QString::number(vk->chapter())));
            text.replace(QStringLiteral("#TEXT_ALIGN#"),
                         QStringLiteral("right"));
        } else { // no range supported
            text = render->renderSingleKey(key->key(), modules);
        }
    }
    util::tool::savePlainFile(filename, text);
    return true;
}

bool CExportManager::saveKeyList(CSwordModuleSearch::ModuleResultList const & l,
                                 CSwordModuleInfo const * module,
                                 Format const format,
                                 bool const addText)
{
    if (l.empty())
        return false;

    QString const filename = getSaveFileName(format);
    if (filename.isEmpty())
        return false;

    CTextRendering::KeyTree tree; /// \todo Verify that items in tree are properly freed.

    setProgressRange(l.size()); /// \todo check size
    KTI::Settings itemSettings;
    itemSettings.highlight = false;

    for (auto const & keyPtr : l) {
        if (progressWasCancelled())
            return false;
        tree.emplace_back(QString::fromLocal8Bit(keyPtr->getText()),
                          module,
                          itemSettings);
        incProgress();
    }

    QString const text = newRenderer(format, addText)->renderKeyTree(tree);
    util::tool::savePlainFile(filename, text);
    closeProgressDialog();
    return true;
}

bool CExportManager::saveKeyList(QList<CSwordKey *> const & list,
                                 Format const format,
                                 bool const addText)
{
    if (list.empty())
        return false;

    const QString filename = getSaveFileName(format);
    if (filename.isEmpty())
        return false;

    CTextRendering::KeyTree tree; /// \todo Verify that items in tree are properly freed.

    KTI::Settings itemSettings;
    itemSettings.highlight = false;

    setProgressRange(list.count());
    for (CSwordKey const * const k : list) {
        if (progressWasCancelled())
            return false;
        tree.emplace_back(k->key(), k->module(), itemSettings);
        incProgress();
    }

    QString const text = newRenderer(format, addText)->renderKeyTree(tree);
    util::tool::savePlainFile(filename, text);
    closeProgressDialog();
    return true;
}

namespace {

template <typename Arg> inline void copyToClipboard(Arg && arg)
{ QApplication::clipboard()->setText(std::forward<Arg>(arg)); }

} // anonymous namespace

bool CExportManager::copyKey(CSwordKey const * const key,
                             Format const format,
                             bool const addText)
{
    if (!key || !key->module())
        return false;

    QString text;
    BtConstModuleList modules;
    modules.append(key->module());

    {
        auto const render = newRenderer(format, addText);
        CSwordVerseKey const * const vk =
                dynamic_cast<CSwordVerseKey const *>(key);
        if (vk && vk->isBoundSet()) {
            text = render->renderKeyRange(vk->lowerBound(),
                                          vk->upperBound(),
                                          modules);
        } else { // no range supported
            text = render->renderSingleKey(key->key(), modules);
        }
    }

    copyToClipboard(text);
    return true;
}

bool CExportManager::copyKeyList(CSwordModuleSearch::ModuleResultList const & l,
                                 CSwordModuleInfo const * const module,
                                 Format const format,
                                 bool const addText)
{
    if (l.empty())
        return false;

    CTextRendering::KeyTree tree; /// \todo Verify that items in tree are properly freed.
    KTI::Settings itemSettings;
    itemSettings.highlight = false;

    for (auto const & keyPtr : l) {
        if (progressWasCancelled())
            return false;
        tree.emplace_back(QString::fromLocal8Bit(keyPtr->getText()),
                          module,
                          itemSettings);
    }

    copyToClipboard(newRenderer(format, addText)->renderKeyTree(tree));
    closeProgressDialog();
    return true;
}


bool CExportManager::copyKeyList(QList<CSwordKey *> const & list,
                                 Format const format,
                                 bool const addText)
{
    if (list.empty())
        return false;

    CTextRendering::KeyTree tree; /// \todo Verify that items in tree are properly freed.
    KTI::Settings itemSettings;
    itemSettings.highlight = false;

    setProgressRange(list.count());
    for (CSwordKey const * const k : list) {
        if (progressWasCancelled())
            return false;
        tree.emplace_back(k->key(), k->module(), itemSettings);
        incProgress();
    }

    copyToClipboard(newRenderer(format, addText)->renderKeyTree(tree));
    closeProgressDialog();
    return true;
}

namespace {

struct PrintSettings: BtPrinter::KeyTreeItem::Settings {

    PrintSettings(DisplayOptions const & displayOptions)
        : BtPrinter::KeyTreeItem::Settings{
            false,
            displayOptions.verseNumbers ? Settings::SimpleKey : Settings::NoKey}
    {}

};

} // anonymous namespace

bool CExportManager::printKey(CSwordKey const * const key,
                              DisplayOptions const & displayOptions,
                              FilterOptions const & filterOptions)
{
    PrintSettings settings{displayOptions};
    BtPrinter::KeyTree tree; /// \todo Verify that items in tree are properly freed.
    tree.emplace_back(key->key(), key->module(), settings);
    BtPrinter{displayOptions, filterOptions}.printKeyTree(tree);
    return true;
}

bool CExportManager::printKey(CSwordModuleInfo const * const module,
                              QString const & startKey,
                              QString const & stopKey,
                              DisplayOptions const & displayOptions,
                              FilterOptions const & filterOptions)
{
    PrintSettings settings{displayOptions};
    BtPrinter::KeyTree tree;
    if (startKey != stopKey) {
        tree.emplace_back(startKey, stopKey, module, settings);
    } else {
        tree.emplace_back(startKey, module, settings);
    }
    BtPrinter{displayOptions, filterOptions}.printKeyTree(tree);
    return true;
}

bool CExportManager::printByHyperlink(QString const & hyperlink,
                                      DisplayOptions const & displayOptions,
                                      FilterOptions const & filterOptions)
{
    auto const decodedLink(ReferenceManager::decodeHyperlink(hyperlink));
    if (!decodedLink && !decodedLink->module)
        return false;
    auto const * const module = decodedLink->module;
    auto const & keyName = decodedLink->key;

    BtPrinter::KeyTree tree; /// \todo Verify that items in tree are properly freed.
    PrintSettings settings{displayOptions};
    //check if we have a range of entries or a single one
    if ((module->type() == CSwordModuleInfo::Bible)
        || (module->type() == CSwordModuleInfo::Commentary))
    {
        sword::ListKey const verses =
                sword::VerseKey().parseVerseList(
                        keyName.toUtf8().constData(),
                        "Genesis 1:1",
                        true);

        for (int i = 0; i < verses.getCount(); i++) {
            if (sword::VerseKey const * const element =
                    dynamic_cast<sword::VerseKey const *>(verses.getElement(i)))
            {
                tree.emplace_back(
                            QString::fromUtf8(
                                element->getLowerBound().getText()),
                            QString::fromUtf8(
                                element->getUpperBound().getText()),
                            module,
                            settings);
            } else if (verses.getElement(i)) {
                tree.emplace_back(
                            QString::fromUtf8(verses.getElement(i)->getText()),
                            module,
                            settings);
            }
        }
    } else {
        tree.emplace_back(keyName, module, settings);
    }
    BtPrinter{displayOptions, filterOptions}.printKeyTree(tree);
    return true;
}

bool CExportManager::printKeyList(
        CSwordModuleSearch::ModuleResultList const & list,
        CSwordModuleInfo const * const module,
        DisplayOptions const & displayOptions,
        FilterOptions const & filterOptions)
{
    if (list.empty())
        return false;
    PrintSettings settings{displayOptions};
    BtPrinter::KeyTree tree; /// \todo Verify that items in tree are properly freed.

    setProgressRange(list.size());
    for (auto const & keyPtr : list) {
        if (progressWasCancelled())
            return false;
        QString const key = keyPtr->getText();
        tree.emplace_back(key, key, module, settings);
        incProgress();
    }
    BtPrinter{displayOptions, filterOptions}.printKeyTree(tree);
    closeProgressDialog();
    return true;
}

bool CExportManager::printKeyList(QStringList const & list,
                                  CSwordModuleInfo const * const module,
                                  DisplayOptions const & displayOptions,
                                  FilterOptions const & filterOptions)
{
    if (list.empty())
        return false;

    PrintSettings settings{displayOptions};
    BtPrinter::KeyTree tree; /// \todo Verify that items in tree are properly freed.

    setProgressRange(list.count());
    for (QString const & key: list) {
        if (progressWasCancelled())
            return false;
        tree.emplace_back(key, module, settings);
        incProgress();
    }
    BtPrinter{displayOptions, filterOptions}.printKeyTree(tree);
    closeProgressDialog();
    return true;
}

/** Returns a filename to save a file. */
const QString CExportManager::getSaveFileName(const Format format) {
    QString filter;
    switch (format) {
        case HTML:
            filter = QObject::tr("HTML files") + " (*.html *.htm);;";
            break;
        case Text:
            filter = QObject::tr("Text files") + " (*.txt);;";
            break;
    }
    filter += QObject::tr("All files") + QStringLiteral(" (*)");

    return QFileDialog::getSaveFileName(nullptr,
                                        QObject::tr("Save file"),
                                        QString(),
                                        filter,
                                        nullptr);
}

std::unique_ptr<CTextRendering> CExportManager::newRenderer(Format const format,
                                                            bool const addText)
{
    FilterOptions filterOptions = m_filterOptions;
    filterOptions.footnotes = false;
    filterOptions.strongNumbers = false;
    filterOptions.morphTags = false;
    filterOptions.lemmas = false;
    filterOptions.scriptureReferences = false;
    filterOptions.textualVariants = false;

    using R = std::unique_ptr<CTextRendering>;
    BT_ASSERT((format == Text) || (format == HTML));
    if (format == HTML)
        return R{new CTextRendering(addText,
                                    m_displayOptions,
                                    filterOptions)};
    return R{new CPlainTextExportRendering(addText,
                                           m_displayOptions,
                                           filterOptions)};
}

void CExportManager::setProgressRange(int const items) {
    if (!m_progressDialog)
        return;

    m_progressDialog->setMaximum(items);
    m_progressDialog->setValue(0);
    m_progressDialog->setMinimumDuration(0);
    m_progressDialog->show();
    //     m_progressDialog->repaint();
    qApp->processEvents(); //do not lock the GUI!
}

/** Increments the progress by one item. */
void CExportManager::incProgress() {
    if (m_progressDialog)
        m_progressDialog->setValue(m_progressDialog->value() + 1);
}

bool CExportManager::progressWasCancelled() {
    return m_progressDialog ? m_progressDialog->wasCanceled() : false;
}

/** Closes the progress dialog immediatly. */
void CExportManager::closeProgressDialog() {
    if (m_progressDialog) {
        m_progressDialog->close();
        m_progressDialog->reset();
    }
    qApp->processEvents(); //do not lock the GUI!
}
