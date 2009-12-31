/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/cexportmanager.h"

#include <boost/scoped_ptr.hpp>
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

CExportManager::CExportManager(const QString& caption, const bool showProgress, const QString& progressLabel, const CSwordBackend::FilterOptions filterOptions, const CSwordBackend::DisplayOptions displayOptions) {
    m_caption = !caption.isEmpty() ? caption : QString::fromLatin1("BibleTime");
    m_progressLabel = progressLabel;
    m_filterOptions = filterOptions;
    m_displayOptions = displayOptions;
    m_showProgress = showProgress;
    m_progressDialog = 0;
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

    CSwordBackend::FilterOptions filterOptions = m_filterOptions;
    filterOptions.footnotes = false;
    filterOptions.strongNumbers = false;
    filterOptions.morphTags = false;
    filterOptions.lemmas = false;
    filterOptions.scriptureReferences = false;
    filterOptions.textualVariants = false;

    CHTMLExportRendering::Settings settings(addText);
    boost::scoped_ptr<CTextRendering> render (
        (format == HTML)
        ? new CHTMLExportRendering(settings, m_displayOptions, filterOptions)
        : new CPlainTextExportRendering(settings, m_displayOptions, filterOptions)
    );

    QString text;
    QString startKey;
    QString stopKey;

    QList<CSwordModuleInfo*> modules;
    modules.append(key->module());

    CSwordVerseKey *vk = dynamic_cast<CSwordVerseKey*>(key);
    if (vk && vk->isBoundSet()) {
        text = render->renderKeyRange( QString::fromUtf8(vk->LowerBound()), QString::fromUtf8(vk->UpperBound()), modules );
    }
    else { //no range supported
        text = render->renderSingleKey(key->key(), modules);
    }

    if (!progressWasCancelled()) {
        util::tool::savePlainFile(filename, text, false, (format == HTML) ? QTextCodec::codecForName("UTF-8") : QTextCodec::codecForLocale() );
        closeProgressDialog();
        return true;
    }
    return false;
}

bool CExportManager::saveKeyList(sword::ListKey* list, CSwordModuleInfo* module, const Format format, const bool addText) {
    if (!list->Count())
        return false;

    const QString filename = getSaveFileName(format);
    if (filename.isEmpty()) {
        return false;
    }

    CSwordBackend::FilterOptions filterOptions = m_filterOptions;
    filterOptions.footnotes = false;
    filterOptions.strongNumbers = false;
    filterOptions.morphTags = false;
    filterOptions.lemmas = false;
    filterOptions.scriptureReferences = false;
    filterOptions.textualVariants = false;

    CHTMLExportRendering::Settings settings(addText);
    boost::scoped_ptr<CTextRendering> render (
        (format == HTML)
        ? new CHTMLExportRendering(settings, m_displayOptions, filterOptions)
        : new CPlainTextExportRendering(settings, m_displayOptions, filterOptions)
    );

    CTextRendering::KeyTree tree;

    setProgressRange(list->Count());
    CTextRendering::KeyTreeItem::Settings itemSettings;
    itemSettings.highlight = false;

    *list = sword::TOP;
    while (!list->Error() && !progressWasCancelled()) {
        tree.append( new CTextRendering::KeyTreeItem(QString::fromLocal8Bit((const char*)(*list)) , module, itemSettings) );
        incProgress();

        (*list)++;
    }

    const QString text = render->renderKeyTree(tree);

    if (!progressWasCancelled()) {
        util::tool::savePlainFile(filename, text, false, (format == HTML) ? QTextCodec::codecForName("UTF-8") : QTextCodec::codecForLocale() );
        closeProgressDialog();
        return true;
    }
    return false;
}

bool CExportManager::saveKeyList(QList<CSwordKey*>& list, const Format format, const bool addText ) {
    if (!list.count())
        return false;

    const QString filename = getSaveFileName(format);
    if (filename.isEmpty()) {
        return false;
    }

    CSwordBackend::FilterOptions filterOptions = m_filterOptions;
    filterOptions.footnotes = false;
    filterOptions.strongNumbers = false;
    filterOptions.morphTags = false;
    filterOptions.lemmas = false;
    filterOptions.scriptureReferences = false;
    filterOptions.textualVariants = false;

    CHTMLExportRendering::Settings settings(addText);
    boost::scoped_ptr<CTextRendering> render (
        (format == HTML)
        ? new CHTMLExportRendering(settings, m_displayOptions, filterOptions)
        : new CPlainTextExportRendering(settings, m_displayOptions, filterOptions)
    );

    CTextRendering::KeyTree tree;

    setProgressRange(list.count());
    CTextRendering::KeyTreeItem::Settings itemSettings;
    itemSettings.highlight = false;

    QListIterator<CSwordKey*> it(list);
    while (it.hasNext() && !progressWasCancelled()) {
        CSwordKey* k = it.next();
        tree.append( new CTextRendering::KeyTreeItem(k->key(), k->module(), itemSettings) );
        incProgress();
    };

    const QString text = render->renderKeyTree(tree);

    if (!progressWasCancelled()) {
        util::tool::savePlainFile(filename, text, false, (format == HTML) ? QTextCodec::codecForName("UTF-8") : QTextCodec::codecForLocale() );
        closeProgressDialog();
        return true;
    }
    return false;
}

bool CExportManager::copyKey(CSwordKey* key, const Format format, const bool addText) {
    if (!key) {
        return false;
    }
    if (!key->module()) {
        return false;
    }

    CSwordBackend::FilterOptions filterOptions = m_filterOptions;
    filterOptions.footnotes = false;
    filterOptions.strongNumbers = false;
    filterOptions.morphTags = false;
    filterOptions.lemmas = false;
    filterOptions.scriptureReferences = false;
    filterOptions.textualVariants = false;

    CHTMLExportRendering::Settings settings(addText);
    boost::scoped_ptr<CTextRendering> render (
        (format == HTML)
        ? new CHTMLExportRendering(settings, m_displayOptions, filterOptions)
        : new CPlainTextExportRendering(settings, m_displayOptions, filterOptions)
    );

    QString text;
    QString startKey;
    QString stopKey;

    QList<CSwordModuleInfo*> modules;
    modules.append(key->module());

    CSwordVerseKey *vk = dynamic_cast<CSwordVerseKey*>(key);
    if (vk && vk->isBoundSet()) {
        text = render->renderKeyRange(
                   QString::fromUtf8(vk->LowerBound()),
                   QString::fromUtf8(vk->UpperBound()),
                   modules
               );
    }
    else { //no range supported
        text = render->renderSingleKey(key->key(), modules);
    }

    QApplication::clipboard()->setText(text);
    return true;
}

bool CExportManager::copyKeyList(sword::ListKey* list, CSwordModuleInfo* module, const Format format, const bool addText) {
    if (!list->Count())
        return false;

    CSwordBackend::FilterOptions filterOptions = m_filterOptions;
    filterOptions.footnotes = false;
    filterOptions.strongNumbers = false;
    filterOptions.morphTags = false;
    filterOptions.lemmas = false;
    filterOptions.scriptureReferences = false;
    filterOptions.textualVariants = false;

    CHTMLExportRendering::Settings settings(addText);
    boost::scoped_ptr<CTextRendering> render (
        (format == HTML)
        ? new CHTMLExportRendering(settings, m_displayOptions, filterOptions)
        : new CPlainTextExportRendering(settings, m_displayOptions, filterOptions)
    );

    CTextRendering::KeyTree tree;
    CTextRendering::KeyTreeItem::Settings itemSettings;
    itemSettings.highlight = false;

    *list = sword::TOP;
    while (!list->Error() && !progressWasCancelled()) {
        tree.append( new CTextRendering::KeyTreeItem(QString::fromLocal8Bit((const char*)(*list)) , module, itemSettings) );

        (*list)++;
    }

    const QString text = render->renderKeyTree(tree);
    QApplication::clipboard()->setText(text);
    return true;
}


bool CExportManager::copyKeyList(QList<CSwordKey*>& list, const Format format, const bool addText ) {
    if (!list.count())
        return false;

    CSwordBackend::FilterOptions filterOptions = m_filterOptions;
    filterOptions.footnotes = false;
    filterOptions.strongNumbers = false;
    filterOptions.morphTags = false;
    filterOptions.lemmas = false;
    filterOptions.scriptureReferences = false;
    filterOptions.textualVariants = false;

    CHTMLExportRendering::Settings settings(addText);
    boost::scoped_ptr<CTextRendering> render (
        (format == HTML)
        ? new CHTMLExportRendering(settings, m_displayOptions, filterOptions)
        : new CPlainTextExportRendering(settings, m_displayOptions, filterOptions)
    );

    CTextRendering::KeyTree tree;

    CTextRendering::KeyTreeItem::Settings itemSettings;
    itemSettings.highlight = false;

    QListIterator<CSwordKey*> it(list);
    while (it.hasNext() && !progressWasCancelled()) {
        CSwordKey* k = it.next();
        tree.append( new CTextRendering::KeyTreeItem(k->key(), k->module(), itemSettings) );
        incProgress();
    };

    const QString text = render->renderKeyTree(tree);
    QApplication::clipboard()->setText(text);
    if (!progressWasCancelled()) {
        closeProgressDialog();
    }
    return true;
}

bool CExportManager::printKeyList(sword::ListKey* list, CSwordModuleInfo* module, CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions) {
    CPrinter::KeyTreeItem::Settings settings;
    CPrinter::KeyTree tree;

    QString startKey, stopKey;
    setProgressRange(list->Count());

    (*list) = sword::TOP;
    while (!list->Error() && !progressWasCancelled()) {
        sword::VerseKey* vk = dynamic_cast<sword::VerseKey*>(list);
        if (vk) {
            startKey = QString::fromUtf8((const char*)(vk->LowerBound()) );
            stopKey = QString::fromUtf8((const char*)(vk->UpperBound()) );
            tree.append( new CPrinter::KeyTreeItem(startKey, stopKey, module, settings) );
        }
        else {
            startKey = QString::fromUtf8((const char*) * list);
            tree.append( new CPrinter::KeyTreeItem(startKey, module, settings) );
        }

        (*list)++;
        incProgress();
    }

    boost::scoped_ptr<CPrinter> printer(new CPrinter(0, displayOptions, filterOptions));

    if (!progressWasCancelled()) {
        printer->printKeyTree(tree);
        closeProgressDialog();
        return true;
    }

    return false;
}

bool CExportManager::printKey( CSwordModuleInfo* module, const QString& startKey, const QString& stopKey, CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions ) {
    CPrinter::KeyTreeItem::Settings settings;
    settings.keyRenderingFace =
        displayOptions.verseNumbers
        ? CPrinter::KeyTreeItem::Settings::SimpleKey
        : CPrinter::KeyTreeItem::Settings::NoKey;

    CPrinter::KeyTree tree;
    if (startKey != stopKey) {
        tree.append( new CPrinter::KeyTreeItem(startKey, stopKey, module, settings) );
    }
    else {
        tree.append( new CPrinter::KeyTreeItem(startKey, module, settings) );
    }

    boost::scoped_ptr<CPrinter> printer(new CPrinter(0, displayOptions, filterOptions));
    printer->printKeyTree(tree);
    return true;
}

bool CExportManager::printKey( CSwordKey* key, CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions) {
    CPrinter::KeyTreeItem::Settings settings;
    settings.keyRenderingFace =
        displayOptions.verseNumbers
        ? CPrinter::KeyTreeItem::Settings::SimpleKey
        : CPrinter::KeyTreeItem::Settings::NoKey;

    CPrinter::KeyTree tree;
    tree.append( new CPrinter::KeyTreeItem(key->key(), key->module(), settings) );

    boost::scoped_ptr<CPrinter> printer(new CPrinter(0, displayOptions, filterOptions));
    printer->printKeyTree(tree);
    return true;
}

/** Prints a key using the hyperlink created by CReferenceManager. */
bool CExportManager::printByHyperlink( const QString& hyperlink, CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions ) {
    QString moduleName;
    QString keyName;
    ReferenceManager::Type type;

    ReferenceManager::decodeHyperlink(hyperlink, moduleName, keyName, type);
    if (moduleName.isEmpty()) {
        moduleName = ReferenceManager::preferredModule(type);
    }

    CPrinter::KeyTree tree;
    CPrinter::KeyTreeItem::Settings settings;
    settings.keyRenderingFace =
        displayOptions.verseNumbers
        ? CPrinter::KeyTreeItem::Settings::SimpleKey
        : CPrinter::KeyTreeItem::Settings::NoKey;

    CSwordModuleInfo* module = backend()->findModuleByName(moduleName);
    Q_ASSERT(module);

    if (module) {
        //check if we have a range of entries or a single one
        if ((module->type() == CSwordModuleInfo::Bible) || (module->type() == CSwordModuleInfo::Commentary)) {
            sword::ListKey verses = sword::VerseKey().ParseVerseList((const char*)keyName.toUtf8(), "Genesis 1:1", true);

            for (int i = 0; i < verses.Count(); ++i) {
                sword::VerseKey* element = dynamic_cast<sword::VerseKey*>(verses.GetElement(i));
                if (element) {
                    const QString startKey = QString::fromUtf8(element->LowerBound().getText());
                    const QString stopKey =  QString::fromUtf8(element->UpperBound().getText());

                    tree.append( new CPrinter::KeyTreeItem(startKey, stopKey, module, settings) );
                }
                else if (verses.GetElement(i)) {
                    const QString key =  QString::fromUtf8(verses.GetElement(i)->getText());

                    tree.append( new CPrinter::KeyTreeItem(key, module, settings) );
                }
            }
        }
        else {
            tree.append( new CPrinter::KeyTreeItem(keyName, module, settings) );
        }
    }

    boost::scoped_ptr<CPrinter> printer(new CPrinter(0, displayOptions, filterOptions));
    printer->printKeyTree(tree);
    return true;
}

bool CExportManager::printKeyList(const QStringList& list, CSwordModuleInfo* module,  CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions) {
    CPrinter::KeyTreeItem::Settings settings;
    settings.keyRenderingFace =
        displayOptions.verseNumbers
        ? CPrinter::KeyTreeItem::Settings::SimpleKey
        : CPrinter::KeyTreeItem::Settings::NoKey;

    CPrinter::KeyTree tree;
    setProgressRange(list.count());

    const QStringList::const_iterator end = list.constEnd();
    for (QStringList::const_iterator it = list.constBegin(); (it != end) && !progressWasCancelled(); ++it) {
        tree.append( new CPrinter::KeyTreeItem(*it, module, settings) );
        incProgress();
    }

    boost::scoped_ptr<CPrinter> printer(new CPrinter(0, displayOptions, filterOptions));

    if (!progressWasCancelled()) {
        printer->printKeyTree(tree);
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

/** Returns a string containing the linebreak for the current format. */
const QString CExportManager::lineBreak(const Format format) {
    if (static_cast<bool>(m_displayOptions.lineBreaks))
        return (format == HTML) ? QString::fromLatin1("<br/>\n") : QString::fromLatin1("\n");

    return QString::null;
}

/** No descriptions */
void CExportManager::setProgressRange( const int items ) {
    if (QProgressDialog* dlg = progressDialog()) {
        dlg->setMaximum(items);
        dlg->setValue(0);
        dlg->setMinimumDuration(0);
        dlg->show();
        //     dlg->repaint();
        qApp->processEvents(); //do not lock the GUI!
    }
}

/** Creates the progress dialog with the correct settings. */
QProgressDialog* CExportManager::progressDialog() {
    if (!m_showProgress) {
        return 0;
    }

    if (!m_progressDialog) {
        m_progressDialog = new QProgressDialog(0, Qt::Dialog );
        m_progressDialog->setLabelText(m_progressLabel);

        m_progressDialog->setWindowTitle("BibleTime");
    }

    return m_progressDialog;
}

/** Increments the progress by one item. */
void CExportManager::incProgress() {
    if (QProgressDialog* dlg = progressDialog()) {
        dlg->setValue( dlg->value() + 1 );
    }
}

/** No descriptions */
bool CExportManager::progressWasCancelled() {
    if (QProgressDialog* dlg = progressDialog()) {
        return dlg->wasCanceled();
    }

    return true;
}

/** Closes the progress dialog immediatly. */
void CExportManager::closeProgressDialog() {
    if (QProgressDialog* dlg = progressDialog()) {
        dlg->close();
        dlg->reset();
    }

    qApp->processEvents(); //do not lock the GUI!
}
