/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "frontend/cexportmanager.h"

#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QList>
#include <QProgressDialog>
#include <QTextStream>
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordkey.h"
#include "backend/keys/cswordversekey.h"
#include "backend/managers/referencemanager.h"
#include "backend/managers/cdisplaytemplatemgr.h"
#include "backend/rendering/centrydisplay.h"
#include "backend/rendering/chtmlexportrendering.h"
#include "backend/rendering/cplaintextexportrendering.h"
#include "frontend/btprinter.h"
#include "util/btassert.h"
#include "util/tool.h"

// Sword includes:
#include <swkey.h>
#include <listkey.h>


using namespace Rendering;

using KTI = CTextRendering::KeyTreeItem;


namespace {

QTextCodec * getCodec(CExportManager::Format const format) {
    if (format == CExportManager::HTML)
        return QTextCodec::codecForName("UTF-8");
    return QTextCodec::codecForLocale();
}

} // anonymous namespace

CExportManager::CExportManager(bool const showProgress,
                               QString const & progressLabel,
                               FilterOptions const & filterOptions,
                               DisplayOptions const & displayOptions)
{
    m_filterOptions = filterOptions;
    m_displayOptions = displayOptions;

    if (showProgress) {
        m_progressDialog = new QProgressDialog{nullptr, Qt::Dialog};
        m_progressDialog->setWindowTitle("BibleTime");
        m_progressDialog->setLabelText(progressLabel);
    } else {
        m_progressDialog = nullptr;
    }
}

CExportManager::~CExportManager() {
    delete m_progressDialog;
}

bool CExportManager::saveKey(CSwordKey const * const key,
                             Format const format,
                             bool const addText)
{
    if (!key || !key->module())
        return false;
    QString const filename = getSaveFileName(format);
    if (filename.isEmpty())
        return false;

    QString text;
    {
        BtConstModuleList modules;
        modules.append(key->module());
        CSwordVerseKey const * const vk =
                dynamic_cast<CSwordVerseKey const *>(key);
        auto const render = newRenderer(format, addText);
        if (vk && vk->isBoundSet()) {
            text = render->renderKeyRange(
                        QString::fromUtf8(vk->getLowerBound()),
                        QString::fromUtf8(vk->getUpperBound()),
                        modules);
            QString chapterTitle  = QString(vk->getBookName()) + " " + QString::number(vk->getChapter());
            text.replace("#CHAPTERTITLE#", chapterTitle);
        } else { // no range supported
            text = render->renderSingleKey(key->key(), modules);
        }
    }
    util::tool::savePlainFile(filename, text, false, getCodec(format));
    return true;
}

bool CExportManager::saveKeyList(sword::ListKey const & l,
                                 CSwordModuleInfo const * module,
                                 Format const format,
                                 bool const addText)
{
    if (!l.getCount())
        return false;

    QString const filename = getSaveFileName(format);
    if (filename.isEmpty())
        return false;

    CTextRendering::KeyTree tree; /// \todo Verify that items in tree are properly freed.

    setProgressRange(l.getCount());
    KTI::Settings itemSettings;
    itemSettings.highlight = false;

    sword::ListKey list(l);
    list.setPosition(sword::TOP);
    while (!list.popError()) {
        if (progressWasCancelled())
            return false;
        tree.append(new KTI(QString::fromLocal8Bit(list.getText()),
                            module,
                            itemSettings));
        incProgress();
        list.increment();
    }

    QString const text = newRenderer(format, addText)->renderKeyTree(tree);
    util::tool::savePlainFile(filename, text, false, getCodec(format));
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
        tree.append(new KTI(k->key(), k->module(), itemSettings));
        incProgress();
    };

    QString const text = newRenderer(format, addText)->renderKeyTree(tree);
    util::tool::savePlainFile(filename, text, false, getCodec(format));
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
            text = render->renderKeyRange(
                       QString::fromUtf8(vk->getLowerBound()),
                       QString::fromUtf8(vk->getUpperBound()),
                       modules
                   );
        } else { // no range supported
            text = render->renderSingleKey(key->key(), modules);
        }
    }

    copyToClipboard(text);
    return true;
}

bool CExportManager::copyKeyList(sword::ListKey const & l,
                                 CSwordModuleInfo const * const module,
                                 Format const format,
                                 bool const addText)
{
    sword::ListKey list = l;
    if (!list.getCount())
        return false;

    CTextRendering::KeyTree tree; /// \todo Verify that items in tree are properly freed.
    KTI::Settings itemSettings;
    itemSettings.highlight = false;

    list.setPosition(sword::TOP);
    while (!list.popError()) {
        if (progressWasCancelled())
            return false;
        tree.append(new KTI(QString::fromLocal8Bit(list.getText()),
                            module,
                            itemSettings));
        list.increment();
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
        tree.append(new KTI(k->key(), k->module(), itemSettings));
        incProgress();
    };

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
    tree.append(new BtPrinter::KeyTreeItem(key->key(), key->module(), settings));
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
    BtPrinter::KeyTree tree; /// \todo Verify that items in tree are properly freed.
    if (startKey != stopKey) {
        tree.append(new BtPrinter::KeyTreeItem(startKey, stopKey, module, settings));
    } else {
        tree.append(new BtPrinter::KeyTreeItem(startKey, module, settings));
    }
    BtPrinter{displayOptions, filterOptions}.printKeyTree(tree);
    return true;
}

bool CExportManager::printByHyperlink(QString const & hyperlink,
                                      DisplayOptions const & displayOptions,
                                      FilterOptions const & filterOptions)
{
    QString moduleName;
    QString keyName;
    ReferenceManager::Type type;
    ReferenceManager::decodeHyperlink(hyperlink, moduleName, keyName, type);
    if (moduleName.isEmpty())
        moduleName = ReferenceManager::preferredModule(type);

    BtPrinter::KeyTree tree; /// \todo Verify that items in tree are properly freed.
    PrintSettings settings{displayOptions};
    CSwordModuleInfo const * module =
            CSwordBackend::instance()->findModuleByName(moduleName);
    BT_ASSERT(module);
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
                tree.append(
                        new BtPrinter::KeyTreeItem(
                                QString::fromUtf8(
                                        element->getLowerBound().getText()),
                                QString::fromUtf8(
                                        element->getUpperBound().getText()),
                                module,
                                settings) );
            } else if (verses.getElement(i)) {
                tree.append(
                        new BtPrinter::KeyTreeItem(
                            QString::fromUtf8(verses.getElement(i)->getText()),
                            module,
                            settings) );
            }
        }
    } else {
        tree.append(new BtPrinter::KeyTreeItem(keyName, module, settings));
    }
    BtPrinter{displayOptions, filterOptions}.printKeyTree(tree);
    return true;
}

bool CExportManager::printKeyList(sword::ListKey const & list,
                                  CSwordModuleInfo const * const module,
                                  DisplayOptions const & displayOptions,
                                  FilterOptions const & filterOptions)
{
    if (!list.getCount())
        return false;
    PrintSettings settings{displayOptions};
    BtPrinter::KeyTree tree; /// \todo Verify that items in tree are properly freed.

    setProgressRange(list.getCount());
    for (int i = 0; i < list.getCount(); i++) {
        if (progressWasCancelled())
            return false;
        sword::SWKey const * const swKey = list.getElement(i);
        if (sword::VerseKey const * const vKey =
                    dynamic_cast<const sword::VerseKey*>(swKey))
        {
            QString const startKey = vKey->getText();
            tree.append(new KTI(startKey, startKey, module, settings));
        } else {
            QString const key = swKey->getText();
            tree.append(new KTI(key, key, module, settings));
        }
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
        tree.append(new BtPrinter::KeyTreeItem(key, module, settings));
        incProgress();
    }
    BtPrinter{displayOptions, filterOptions}.printKeyTree(tree);
    closeProgressDialog();
    return true;
}

/** Returns the string for the filedialogs to show the correct files. */
const QString CExportManager::filterString( const Format format ) {
    QString const allFiles = QObject::tr("All files") + QString(" (*.*)");
    switch (format) {
        case HTML:
            return allFiles + QObject::tr("HTML files")
                            + QString(" (*.html *.htm);;");
        case Text:
            return allFiles + QObject::tr("Text files") + QString(" (*.txt);;");
        default:
            return allFiles;
    }
}

/** Returns a filename to save a file. */
const QString CExportManager::getSaveFileName(const Format format) {
    return QFileDialog::getSaveFileName(nullptr,
                                        QObject::tr("Save file"),
                                        "",
                                        filterString(format),
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
        return R{new CHTMLExportRendering(addText,
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
