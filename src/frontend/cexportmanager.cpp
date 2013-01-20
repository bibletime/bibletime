/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
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
#include "frontend/cprinter.h"
#include "util/tool.h"

// Sword includes:
#include <swkey.h>
#include <listkey.h>


using namespace Rendering;
using namespace Printing;

CExportManager::CExportManager(const bool showProgress,
                               const QString &progressLabel,
                               const FilterOptions &filterOptions,
                               const DisplayOptions &displayOptions)
{
    m_filterOptions = filterOptions;
    m_displayOptions = displayOptions;

    if (showProgress) {
        m_progressDialog = new QProgressDialog(0, Qt::Dialog);
        m_progressDialog->setWindowTitle("BibleTime");
        m_progressDialog->setLabelText(progressLabel);
    } else {
        m_progressDialog = 0;
    }
}

CExportManager::~CExportManager() {
    delete m_progressDialog;
}

bool CExportManager::saveKey(CSwordKey* key, const Format format, const bool addText) {
    if (!key) {
        return false;
    }
    if (!key->module()) {
        return false;
    }
    const QString filename = getSaveFileName(format);
    if (filename.isEmpty()) {
        return false;
    }

    CTextRendering * render = newRenderer(format, addText);

    QString text;

    QList<const CSwordModuleInfo*> modules;
    modules.append(key->module());

    CSwordVerseKey *vk = dynamic_cast<CSwordVerseKey*>(key);
    if (vk && vk->isBoundSet()) {
        text = render->renderKeyRange( QString::fromUtf8(vk->getLowerBound()), QString::fromUtf8(vk->getUpperBound()), modules );
    }
    else { //no range supported
        text = render->renderSingleKey(key->key(), modules);
    }
    delete render;

    util::tool::savePlainFile(filename, text, false,
                              (format == HTML)
                              ? QTextCodec::codecForName("UTF-8")
                              : QTextCodec::codecForLocale());
    return true;
}

bool CExportManager::saveKeyList(const sword::ListKey &l,
                                 const CSwordModuleInfo *module,
                                 Format format,
                                 bool addText)
{
    /// \warning This is a workaround for Sword constness
    sword::ListKey list = l;
    if (!list.Count())
        return false;

    const QString filename = getSaveFileName(format);
    if (filename.isEmpty()) {
        return false;
    }

    CTextRendering::KeyTree tree; /// \todo Verify that items in tree are properly freed.

    setProgressRange(list.Count());
    CTextRendering::KeyTreeItem::Settings itemSettings;
    itemSettings.highlight = false;

    list.setPosition(sword::TOP);
    while (!list.popError() && !progressWasCancelled()) {
        tree.append( new CTextRendering::KeyTreeItem(QString::fromLocal8Bit((const char*)list) , module, itemSettings) );
        incProgress();

        list.increment();
    }

    CTextRendering * render = newRenderer(format, addText);
    const QString text = render->renderKeyTree(tree);
    delete render;

    if (!progressWasCancelled()) {
        util::tool::savePlainFile(filename, text, false, (format == HTML) ? QTextCodec::codecForName("UTF-8") : QTextCodec::codecForLocale() );
        closeProgressDialog();
        return true;
    }
    return false;
}

bool CExportManager::saveKeyList(const QList<CSwordKey*> &list,
                                 Format format,
                                 bool addText)
{
    if (!list.count())
        return false;

    const QString filename = getSaveFileName(format);
    if (filename.isEmpty()) {
        return false;
    }

    CTextRendering::KeyTree tree; /// \todo Verify that items in tree are properly freed.

    setProgressRange(list.count());
    CTextRendering::KeyTreeItem::Settings itemSettings;
    itemSettings.highlight = false;

    QListIterator<CSwordKey*> it(list);
    while (it.hasNext() && !progressWasCancelled()) {
        CSwordKey* k = it.next();
        tree.append( new CTextRendering::KeyTreeItem(k->key(), k->module(), itemSettings) );
        incProgress();
    };

    CTextRendering * render = newRenderer(format, addText);
    const QString text = render->renderKeyTree(tree);
    delete render;

    if (!progressWasCancelled()) {
        util::tool::savePlainFile(filename, text, false, (format == HTML) ? QTextCodec::codecForName("UTF-8") : QTextCodec::codecForLocale() );
        closeProgressDialog();
        return true;
    }
    return false;
}

bool CExportManager::copyKey(CSwordKey* key, const Format format, const bool addText) {
    if (!key || !key->module())
        return false;

    QString text;
    QList<const CSwordModuleInfo*> modules;
    modules.append(key->module());

    CTextRendering * render = newRenderer(format, addText);
    CSwordVerseKey * vk = dynamic_cast<CSwordVerseKey*>(key);
    if (vk && vk->isBoundSet()) {
        text = render->renderKeyRange(
                   QString::fromUtf8(vk->getLowerBound()),
                   QString::fromUtf8(vk->getUpperBound()),
                   modules
               );
    }
    else { //no range supported
        text = render->renderSingleKey(key->key(), modules);
    }

    delete render;

    QApplication::clipboard()->setText(text);
    return true;
}

bool CExportManager::copyKeyList(const sword::ListKey &l,
                                 const CSwordModuleInfo *module,
                                 Format format,
                                 bool addText)
{
    sword::ListKey list = l;
    if (!list.Count())
        return false;

    CTextRendering::KeyTree tree; /// \todo Verify that items in tree are properly freed.
    CTextRendering::KeyTreeItem::Settings itemSettings;
    itemSettings.highlight = false;

    list.setPosition(sword::TOP);
    while (!list.popError() && !progressWasCancelled()) {
        tree.append( new CTextRendering::KeyTreeItem(QString::fromLocal8Bit((const char*)list) , module, itemSettings) );

        list.increment();
    }

    CTextRendering * render = newRenderer(format, addText);
    const QString text = render->renderKeyTree(tree);
    delete render;
    QApplication::clipboard()->setText(text);
    return true;
}


bool CExportManager::copyKeyList(const QList<CSwordKey*> &list,
                                 Format format,
                                 bool addText)
{
    if (!list.count())
        return false;


    CTextRendering::KeyTree tree; /// \todo Verify that items in tree are properly freed.

    CTextRendering::KeyTreeItem::Settings itemSettings;
    itemSettings.highlight = false;

    QListIterator<CSwordKey*> it(list);
    while (it.hasNext() && !progressWasCancelled()) {
        CSwordKey* k = it.next();
        tree.append( new CTextRendering::KeyTreeItem(k->key(), k->module(), itemSettings) );
        incProgress();
    };

    CTextRendering * render = newRenderer(format, addText);
    const QString text = render->renderKeyTree(tree);
    delete render;

    QApplication::clipboard()->setText(text);
    if (!progressWasCancelled()) {
        closeProgressDialog();
    }
    return true;
}

bool CExportManager::printKeyList(const sword::ListKey &l,
                                  const CSwordModuleInfo *module,
                                  const DisplayOptions &displayOptions,
                                  const FilterOptions &filterOptions)
{
    /// \warning This is a workaround for Sword constness
    sword::ListKey list = l;
    CPrinter::KeyTreeItem::Settings settings;
    CPrinter::KeyTree tree; /// \todo Verify that items in tree are properly freed.

    setProgressRange(list.Count());
    for (int i=0; i< list.Count(); i++) {
        const sword::SWKey* swKey = list.getElement(i);
        const sword::VerseKey* vKey = dynamic_cast<const sword::VerseKey*>(swKey);
        if (vKey != 0) {
            QString startKey = vKey->getText();
            tree.append(new CTextRendering::KeyTreeItem(startKey,
                startKey,
                module,
                settings));
        }
        else {
            QString key = swKey->getText();
            tree.append(new CTextRendering::KeyTreeItem(key,
                key,
                module,
                settings));
        }
        incProgress();
        if (progressWasCancelled())
            break;
    }


    if (!progressWasCancelled()) {
        CPrinter * printer = new CPrinter(0, displayOptions, filterOptions);
        printer->printKeyTree(tree);
        delete printer;
        closeProgressDialog();
        return true;
    }

    return false;
}

bool CExportManager::printKey(const CSwordModuleInfo *module,
                              const QString &startKey,
                              const QString &stopKey,
                              const DisplayOptions &displayOptions,
                              const FilterOptions &filterOptions)
{
    CPrinter::KeyTreeItem::Settings settings;
    settings.keyRenderingFace =
        displayOptions.verseNumbers
        ? CPrinter::KeyTreeItem::Settings::SimpleKey
        : CPrinter::KeyTreeItem::Settings::NoKey;

    CPrinter::KeyTree tree; /// \todo Verify that items in tree are properly freed.
    if (startKey != stopKey) {
        tree.append( new CPrinter::KeyTreeItem(startKey, stopKey, module, settings) );
    }
    else {
        tree.append( new CPrinter::KeyTreeItem(startKey, module, settings) );
    }

    CPrinter * printer = new CPrinter(0, displayOptions, filterOptions);
    printer->printKeyTree(tree);
    delete printer;
    return true;
}

bool CExportManager::printKey(const CSwordKey *key,
                              const DisplayOptions &displayOptions,
                              const FilterOptions &filterOptions)
{
    CPrinter::KeyTreeItem::Settings settings;
    settings.keyRenderingFace =
        displayOptions.verseNumbers
        ? CPrinter::KeyTreeItem::Settings::SimpleKey
        : CPrinter::KeyTreeItem::Settings::NoKey;

    CPrinter::KeyTree tree; /// \todo Verify that items in tree are properly freed.
    tree.append( new CPrinter::KeyTreeItem(key->key(), key->module(), settings) );

    CPrinter * printer = new CPrinter(0, displayOptions, filterOptions);
    printer->printKeyTree(tree);
    delete printer;
    return true;
}

bool CExportManager::printByHyperlink(const QString &hyperlink,
                                      const DisplayOptions &displayOptions,
                                      const FilterOptions &filterOptions)
{
    QString moduleName;
    QString keyName;
    ReferenceManager::Type type;

    ReferenceManager::decodeHyperlink(hyperlink, moduleName, keyName, type);
    if (moduleName.isEmpty()) {
        moduleName = ReferenceManager::preferredModule(type);
    }

    CPrinter::KeyTree tree; /// \todo Verify that items in tree are properly freed.
    CPrinter::KeyTreeItem::Settings settings;
    settings.keyRenderingFace =
        displayOptions.verseNumbers
        ? CPrinter::KeyTreeItem::Settings::SimpleKey
        : CPrinter::KeyTreeItem::Settings::NoKey;

    CSwordModuleInfo* module = CSwordBackend::instance()->findModuleByName(moduleName);
    Q_ASSERT(module);

    if (module) {
        //check if we have a range of entries or a single one
        if ((module->type() == CSwordModuleInfo::Bible) || (module->type() == CSwordModuleInfo::Commentary)) {
            sword::ListKey verses = sword::VerseKey().parseVerseList((const char*)keyName.toUtf8(), "Genesis 1:1", true);

            for (int i = 0; i < verses.Count(); ++i) {
                sword::VerseKey* element = dynamic_cast<sword::VerseKey*>(verses.getElement(i));
                if (element) {
                    const QString startKey = QString::fromUtf8(element->getLowerBound().getText());
                    const QString stopKey =  QString::fromUtf8(element->getUpperBound().getText());

                    tree.append( new CPrinter::KeyTreeItem(startKey, stopKey, module, settings) );
                }
                else if (verses.getElement(i)) {
                    const QString key =  QString::fromUtf8(verses.getElement(i)->getText());

                    tree.append( new CPrinter::KeyTreeItem(key, module, settings) );
                }
            }
        }
        else {
            tree.append( new CPrinter::KeyTreeItem(keyName, module, settings) );
        }
    }

    CPrinter * printer = new CPrinter(0, displayOptions, filterOptions);
    printer->printKeyTree(tree);
    delete printer;
    return true;
}

bool CExportManager::printKeyList(const QStringList &list,
                                  const CSwordModuleInfo *module,
                                  const DisplayOptions &displayOptions,
                                  const FilterOptions &filterOptions)
{
    CPrinter::KeyTreeItem::Settings settings;
    settings.keyRenderingFace =
        displayOptions.verseNumbers
        ? CPrinter::KeyTreeItem::Settings::SimpleKey
        : CPrinter::KeyTreeItem::Settings::NoKey;

    CPrinter::KeyTree tree; /// \todo Verify that items in tree are properly freed.
    setProgressRange(list.count());

    const QStringList::const_iterator end = list.constEnd();
    for (QStringList::const_iterator it = list.constBegin(); (it != end) && !progressWasCancelled(); ++it) {
        tree.append( new CPrinter::KeyTreeItem(*it, module, settings) );
        incProgress();
    }


    if (!progressWasCancelled()) {
        CPrinter * printer = new CPrinter(0, displayOptions, filterOptions);
        printer->printKeyTree(tree);
        delete printer;
        closeProgressDialog();
        return true;
    }

    return false;
}

/** Returns the string for the filedialogs to show the correct files. */
const QString CExportManager::filterString( const Format format ) {
    switch (format) {
        case HTML:
            return QObject::tr("HTML files") + QString(" (*.html *.htm);;") + QObject::tr("All files") + QString(" (*.*)");
        case Text:
            return QObject::tr("Text files") + QString(" (*.txt);;") + QObject::tr("All files") + QString(" (*.*)");
        default:
            return QObject::tr("All files") + QString(" (*.*)");
    }
}

/** Returns a filename to save a file. */
const QString CExportManager::getSaveFileName(const Format format) {
    return QFileDialog::getSaveFileName(0, QObject::tr("Save file"), "", filterString(format), 0);
}

CTextRendering * CExportManager::newRenderer(const Format format, bool addText) {
    FilterOptions filterOptions = m_filterOptions;
    filterOptions.footnotes = false;
    filterOptions.strongNumbers = false;
    filterOptions.morphTags = false;
    filterOptions.lemmas = false;
    filterOptions.scriptureReferences = false;
    filterOptions.textualVariants = false;

    if (format == HTML) {
        return new CHTMLExportRendering(addText, m_displayOptions, filterOptions);
    } else {
        Q_ASSERT(format == Text);
        return new CPlainTextExportRendering(addText, m_displayOptions, filterOptions);
    }
}

void CExportManager::setProgressRange( const int items ) {
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
    if (m_progressDialog)
        return m_progressDialog->wasCanceled();

    return false;
}

/** Closes the progress dialog immediatly. */
void CExportManager::closeProgressDialog() {
    if (m_progressDialog) {
        m_progressDialog->close();
        m_progressDialog->reset();
    }

    qApp->processEvents(); //do not lock the GUI!
}
