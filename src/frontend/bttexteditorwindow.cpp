/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2026 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "bttexteditorwindow.h"

#include <QAction>
#include <QByteArray>
#include <QCloseEvent>
#include <QColor>
#include <QColorDialog>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDesktopServices>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QIODevice>
#include <QLabel>
#include <QKeySequence>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextEdit>
#include <QTextListFormat>
#include <QTextStream>
#include <QUrl>
#include <QVBoxLayout>
#include "../backend/config/btconfig.h"
#include "../util/btconnect.h"


namespace {

auto const GeometryKey = QStringLiteral("GUI/TextEditor/geometry");
auto const LastFileKey = QStringLiteral("GUI/TextEditor/lastFile");
auto const CursorPositionKey = QStringLiteral("GUI/TextEditor/cursorPosition");

bool isHtmlDocument(QString const & fileName, QString const & text) {
    auto const suffix = QFileInfo(fileName).suffix().toLower();
    if (suffix == QStringLiteral("html") || suffix == QStringLiteral("htm"))
        return true;

    auto const trimmed = text.trimmed();
    return trimmed.startsWith(QStringLiteral("<!DOCTYPE"), Qt::CaseInsensitive)
           || trimmed.startsWith(QStringLiteral("<html"), Qt::CaseInsensitive)
           || trimmed.contains(QStringLiteral("<meta name=\"qrichtext\""),
                               Qt::CaseInsensitive);
}

} // anonymous namespace

BtTextEditorWindow::BtTextEditorWindow(QWidget * const parent)
    : QMainWindow(parent)
    , m_editor(new QTextEdit(this))
    , m_highlightColor(QColor(255, 242, 128))
{
    setCentralWidget(m_editor);
    m_editor->setAcceptRichText(true);
    m_editor->viewport()->installEventFilter(this);

    createActions();
    createMenus();

    BT_CONNECT(m_editor->document(), &QTextDocument::modificationChanged,
               this,                 &BtTextEditorWindow::updateWindowTitle);

    resize(760, 600);
    restoreEditorState();
    updateWindowTitle();
}

void BtTextEditorWindow::closeEvent(QCloseEvent * const event) {
    if (maybeSave()) {
        saveEditorState();
        event->accept();
    } else {
        event->ignore();
    }
}

bool BtTextEditorWindow::eventFilter(QObject * const watched,
                                     QEvent * const event)
{
    if (watched == m_editor->viewport()
        && event->type() == QEvent::MouseButtonRelease)
    {
        auto * const mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton
            && (mouseEvent->modifiers() & Qt::ControlModifier))
        {
            auto const href = m_editor->anchorAt(mouseEvent->pos());
            if (!href.isEmpty()) {
                QDesktopServices::openUrl(QUrl(href));
                return true;
            }
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

void BtTextEditorWindow::newDocument() {
    if (!maybeSave())
        return;
    m_fileName.clear();
    m_editor->clear();
    resetCurrentFormat();
    m_editor->document()->setModified(false);
    updateWindowTitle();
}

void BtTextEditorWindow::openDocument() {
    if (!maybeSave())
        return;
    auto const fileName =
            QFileDialog::getOpenFileName(
                this,
                tr("Open Text Document"),
                QString(),
                tr("Text documents (*.html *.htm *.txt);;All files (*)"));
    if (!fileName.isEmpty())
        loadFile(fileName);
}

bool BtTextEditorWindow::saveDocument() {
    if (m_fileName.isEmpty())
        return saveDocumentAs();
    return saveFile(m_fileName);
}

bool BtTextEditorWindow::saveDocumentAs() {
    QFileDialog dialog(this, tr("Save Text Document"), m_fileName);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilters({
        tr("HTML documents (*.html *.htm)"),
        tr("Plain text (*.txt)"),
        tr("All files (*)")
    });
    if (dialog.exec() != QDialog::Accepted)
        return false;

    auto const files = dialog.selectedFiles();
    if (files.isEmpty())
        return false;

    auto fileName = files.constFirst();
    if (QFileInfo(fileName).suffix().isEmpty()) {
        auto const selectedFilter = dialog.selectedNameFilter();
        if (selectedFilter.contains(QStringLiteral("*.txt")))
            fileName += QStringLiteral(".txt");
        else if (selectedFilter.contains(QStringLiteral("*.html"))
                 || selectedFilter.contains(QStringLiteral("*.htm")))
            fileName += QStringLiteral(".html");
    }
    return saveFile(fileName);
}

void BtTextEditorWindow::updateWindowTitle() {
    auto const displayName =
            m_fileName.isEmpty()
            ? tr("Untitled")
            : QFileInfo(m_fileName).fileName();
    setWindowTitle(tr("%1[*] - Text Editor").arg(displayName));
    setWindowModified(m_editor->document()->isModified());
}

void BtTextEditorWindow::createActions() {
    auto const addEditorAction =
            [this](QList<QAction *> & actionList,
                   QString const & text,
                   QKeySequence const & shortcut,
                   auto slot)
            {
                auto * const action = new QAction(this);
                action->setText(text);
                action->setShortcut(shortcut);
                BT_CONNECT(action, &QAction::triggered, this, slot);
                addAction(action);
                actionList.append(action);
                return action;
            };

    auto * const newAction = new QAction(this);
    newAction->setShortcut(QKeySequence::New);
    newAction->setText(tr("&New"));
    BT_CONNECT(newAction, &QAction::triggered,
               this,      &BtTextEditorWindow::newDocument);
    addAction(newAction);
    m_fileActions.append(newAction);

    auto * const openAction = new QAction(this);
    openAction->setShortcut(QKeySequence::Open);
    openAction->setText(tr("&Open..."));
    BT_CONNECT(openAction, &QAction::triggered,
               this,       &BtTextEditorWindow::openDocument);
    addAction(openAction);
    m_fileActions.append(openAction);

    auto * const saveAction = new QAction(this);
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setText(tr("&Save"));
    BT_CONNECT(saveAction, &QAction::triggered,
               this,       &BtTextEditorWindow::saveDocument);
    addAction(saveAction);
    m_fileActions.append(saveAction);

    auto * const saveAsAction = new QAction(this);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    saveAsAction->setText(tr("Save &As..."));
    BT_CONNECT(saveAsAction, &QAction::triggered,
               this,         &BtTextEditorWindow::saveDocumentAs);
    addAction(saveAsAction);
    m_fileActions.append(saveAsAction);

    addEditorAction(m_formatActions, tr("&Bold"), QKeySequence::Bold,
                    &BtTextEditorWindow::formatBold);
    addEditorAction(m_formatActions, tr("&Italic"), QKeySequence::Italic,
                    &BtTextEditorWindow::formatItalic);
    addEditorAction(m_formatActions, tr("&Underline"), QKeySequence::Underline,
                    &BtTextEditorWindow::formatUnderline);
    addEditorAction(m_formatActions, tr("&Highlight"),
                    QKeySequence(tr("Ctrl+Shift+H",
                                    "Text Editor|Highlight")),
                    &BtTextEditorWindow::formatHighlight);
    addEditorAction(m_formatActions, tr("Highlight &Color..."),
                    QKeySequence(tr("Ctrl+Alt+H",
                                    "Text Editor|Highlight Color")),
                    &BtTextEditorWindow::chooseHighlightColor);
    addEditorAction(m_formatActions, tr("Font &Color..."),
                    QKeySequence(tr("Ctrl+Shift+C",
                                    "Text Editor|Font Color")),
                    &BtTextEditorWindow::chooseFontColor);
    addEditorAction(m_formatActions, tr("Page &Background Color..."),
                    QKeySequence(tr("Ctrl+Shift+B",
                                    "Text Editor|Page Background Color")),
                    &BtTextEditorWindow::chooseBackgroundColor);
    addEditorAction(m_formatActions, tr("&Normal Text"),
                    QKeySequence(tr("Ctrl+Alt+0",
                                    "Text Editor|Normal Text")),
                    &BtTextEditorWindow::formatNormalText);
    addEditorAction(m_formatActions, tr("Heading &1"),
                    QKeySequence(tr("Ctrl+Alt+1",
                                    "Text Editor|Heading 1")),
                    &BtTextEditorWindow::formatHeading1);
    addEditorAction(m_formatActions, tr("Heading &2"),
                    QKeySequence(tr("Ctrl+Alt+2",
                                    "Text Editor|Heading 2")),
                    &BtTextEditorWindow::formatHeading2);
    addEditorAction(m_formatActions, tr("&Bullet List"),
                    QKeySequence(tr("Ctrl+Shift+8",
                                    "Text Editor|Bullet List")),
                    &BtTextEditorWindow::formatBulletList);
    addEditorAction(m_formatActions, tr("&Numbered List"),
                    QKeySequence(tr("Ctrl+Shift+7",
                                    "Text Editor|Numbered List")),
                    &BtTextEditorWindow::formatNumberedList);
    addEditorAction(m_formatActions, tr("&Link"),
                    QKeySequence(tr("Ctrl+K", "Text Editor|Link")),
                    &BtTextEditorWindow::formatLink);
    addEditorAction(m_formatActions, tr("Align &Left"),
                    QKeySequence(tr("Ctrl+L", "Text Editor|Align Left")),
                    &BtTextEditorWindow::alignLeft);
    addEditorAction(m_formatActions, tr("Align &Center"),
                    QKeySequence(tr("Ctrl+E", "Text Editor|Align Center")),
                    &BtTextEditorWindow::alignCenter);
    addEditorAction(m_formatActions, tr("Align &Right"),
                    QKeySequence(tr("Ctrl+R", "Text Editor|Align Right")),
                    &BtTextEditorWindow::alignRight);

    addEditorAction(m_helpActions, tr("&Text Editor Help"), QKeySequence::HelpContents,
                    &BtTextEditorWindow::showEditorHelp);
}

void BtTextEditorWindow::createMenus() {
    auto * const fileMenu = menuBar()->addMenu(tr("&File"));
    for (auto * const action : m_fileActions)
        fileMenu->addAction(action);

    auto * const formatMenu = menuBar()->addMenu(tr("F&ormat"));
    for (int i = 0; i < m_formatActions.size(); i++) {
        if (i == 7 || i == 10 || i == 13)
            formatMenu->addSeparator();
        formatMenu->addAction(m_formatActions.at(i));
    }

    auto * const helpMenu = menuBar()->addMenu(tr("&Help"));
    for (auto * const action : m_helpActions)
        helpMenu->addAction(action);
}

void BtTextEditorWindow::restoreEditorState() {
    restoreGeometry(btConfig().value<QByteArray>(GeometryKey, QByteArray()));

    auto const lastFile = btConfig().value<QString>(LastFileKey, QString());
    if (lastFile.isEmpty() || !QFileInfo::exists(lastFile))
        return;

    if (!loadFile(lastFile))
        return;

    auto const requestedPosition =
            btConfig().value<int>(CursorPositionKey, 0);
    auto const maxPosition =
            qMax(0, m_editor->document()->characterCount() - 1);

    QTextCursor cursor(m_editor->document());
    cursor.setPosition(qBound(0, requestedPosition, maxPosition));
    m_editor->setTextCursor(cursor);
    m_editor->ensureCursorVisible();
}

void BtTextEditorWindow::saveEditorState() {
    btConfig().setValue(GeometryKey, saveGeometry());
    btConfig().setValue(LastFileKey, m_fileName);
    btConfig().setValue(CursorPositionKey, m_editor->textCursor().position());
}

void BtTextEditorWindow::formatBold() {
    QTextCharFormat format;
    format.setFontWeight(m_editor->fontWeight() == QFont::Bold
                         ? QFont::Normal
                         : QFont::Bold);
    mergeCurrentCharFormat(format);
}

void BtTextEditorWindow::formatItalic() {
    QTextCharFormat format;
    format.setFontItalic(!m_editor->fontItalic());
    mergeCurrentCharFormat(format);
}

void BtTextEditorWindow::formatUnderline() {
    QTextCharFormat format;
    format.setFontUnderline(!m_editor->fontUnderline());
    mergeCurrentCharFormat(format);
}

void BtTextEditorWindow::formatHighlight() {
    QTextCharFormat format;
    format.setBackground(m_highlightColor);
    mergeCurrentCharFormat(format);
}

void BtTextEditorWindow::chooseHighlightColor() {
    auto const color =
            QColorDialog::getColor(
                m_highlightColor,
                this,
                tr("Select Highlight Color"));
    if (!color.isValid())
        return;

    m_highlightColor = color;
    formatHighlight();
}

void BtTextEditorWindow::chooseFontColor() {
    auto const color =
            QColorDialog::getColor(
                m_editor->textColor(),
                this,
                tr("Select Font Color"));
    if (!color.isValid())
        return;

    QTextCharFormat format;
    format.setForeground(color);
    mergeCurrentCharFormat(format);
}

void BtTextEditorWindow::chooseBackgroundColor() {
    auto const color =
            QColorDialog::getColor(
                m_editor->palette().base().color(),
                this,
                tr("Select Background Color"));
    if (!color.isValid())
        return;

    auto palette = m_editor->palette();
    palette.setColor(QPalette::Base, color);
    m_editor->setPalette(palette);
    m_editor->document()->setModified(true);
}

void BtTextEditorWindow::formatBulletList() {
    auto cursor = m_editor->textCursor();
    cursor.createList(QTextListFormat::ListDisc);
}

void BtTextEditorWindow::formatNumberedList() {
    auto cursor = m_editor->textCursor();
    cursor.createList(QTextListFormat::ListDecimal);
}

void BtTextEditorWindow::formatLink() {
    auto cursor = m_editor->textCursor();
    auto const selectedText = cursor.selectedText();

    QDialog dialog(this);
    dialog.setWindowTitle(tr("Insert Link"));

    auto * const layout = new QVBoxLayout(&dialog);

    auto * const textLabel = new QLabel(tr("Text:"), &dialog);
    auto * const textEdit = new QLineEdit(&dialog);
    textEdit->setText(selectedText.isEmpty() ? tr("link text") : selectedText);
    textLabel->setBuddy(textEdit);
    layout->addWidget(textLabel);
    layout->addWidget(textEdit);

    auto * const urlLabel = new QLabel(tr("URL:"), &dialog);
    auto * const urlEdit = new QLineEdit(&dialog);
    urlEdit->setText(QStringLiteral("https://"));
    urlLabel->setBuddy(urlEdit);
    layout->addWidget(urlLabel);
    layout->addWidget(urlEdit);

    auto * const buttons =
            new QDialogButtonBox(
                QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                &dialog);
    layout->addWidget(buttons);

    BT_CONNECT(buttons, &QDialogButtonBox::accepted,
               &dialog, &QDialog::accept);
    BT_CONNECT(buttons, &QDialogButtonBox::rejected,
               &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted)
        return;

    auto const linkText = textEdit->text().trimmed();
    auto const url = urlEdit->text().trimmed();
    if (linkText.isEmpty() || url.isEmpty())
        return;

    QTextCharFormat format;
    format.setAnchor(true);
    format.setAnchorHref(url);
    format.setForeground(Qt::blue);
    format.setFontUnderline(true);

    cursor.beginEditBlock();
    if (cursor.hasSelection())
        cursor.removeSelectedText();
    cursor.insertText(linkText, format);
    cursor.endEditBlock();
    m_editor->setTextCursor(cursor);
}

void BtTextEditorWindow::formatHeading1()
{ setBlockHeading(22, QFont::Bold); }

void BtTextEditorWindow::formatHeading2()
{ setBlockHeading(18, QFont::Bold); }

void BtTextEditorWindow::formatNormalText()
{ setBlockHeading(10, QFont::Normal); }

void BtTextEditorWindow::alignLeft()
{ m_editor->setAlignment(Qt::AlignLeft); }

void BtTextEditorWindow::alignCenter()
{ m_editor->setAlignment(Qt::AlignHCenter); }

void BtTextEditorWindow::alignRight()
{ m_editor->setAlignment(Qt::AlignRight); }

void BtTextEditorWindow::showEditorHelp() {
    QMessageBox::information(
                this,
                tr("Text Editor Help"),
                tr("<h3>Text editor basics</h3>"
                   "<p>Type directly into the editor and use the Format menu "
                   "or keyboard shortcuts to change the selected text.</p>"
                   "<table>"
                   "<tr><td><b>Ctrl+B</b></td><td>Bold</td></tr>"
                   "<tr><td><b>Ctrl+I</b></td><td>Italic</td></tr>"
                   "<tr><td><b>Ctrl+U</b></td><td>Underline</td></tr>"
                   "<tr><td><b>Ctrl+Shift+H</b></td><td>Highlight</td></tr>"
                   "<tr><td><b>Ctrl+Alt+1</b></td><td>Heading 1</td></tr>"
                   "<tr><td><b>Ctrl+Alt+2</b></td><td>Heading 2</td></tr>"
                   "<tr><td><b>Ctrl+Shift+8</b></td><td>Bullet list</td></tr>"
                   "<tr><td><b>Ctrl+Shift+7</b></td><td>Numbered list</td></tr>"
                   "<tr><td><b>Ctrl+K</b></td><td>Insert link styling</td></tr>"
                   "</table>"
                   "<p>Documents are saved as HTML by default so formatting is "
                   "preserved.</p>"));
}

void BtTextEditorWindow::mergeCurrentCharFormat(
        QTextCharFormat const & format)
{
    auto cursor = m_editor->textCursor();
    cursor.mergeCharFormat(format);
    m_editor->mergeCurrentCharFormat(format);
}

void BtTextEditorWindow::setBlockHeading(int const pointSize,
                                         int const weight)
{
    QTextCharFormat format;
    format.setFontPointSize(pointSize);
    format.setFontWeight(weight);
    mergeCurrentCharFormat(format);
}

void BtTextEditorWindow::resetCurrentFormat() {
    QTextCharFormat format;
    format.setFontWeight(QFont::Normal);
    format.setFontItalic(false);
    format.setFontUnderline(false);
    format.setFontPointSize(10);
    format.setBackground(Qt::transparent);

    auto cursor = m_editor->textCursor();
    cursor.clearSelection();
    m_editor->setTextCursor(cursor);
    m_editor->setCurrentCharFormat(format);
    m_editor->setAlignment(Qt::AlignLeft);
}

bool BtTextEditorWindow::maybeSave() {
    if (!m_editor->document()->isModified())
        return true;

    auto const result =
            QMessageBox::warning(
                this,
                tr("Text Editor"),
                tr("The current document has unsaved changes."),
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                QMessageBox::Save);
    if (result == QMessageBox::Save)
        return saveDocument();
    return result == QMessageBox::Discard;
}

bool BtTextEditorWindow::loadFile(QString const & fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(
                    this,
                    tr("Text Editor"),
                    tr("Could not open %1.").arg(fileName));
        return false;
    }

    QTextStream stream(&file);
    auto const text = stream.readAll();
    if (isHtmlDocument(fileName, text))
        m_editor->setHtml(text);
    else {
        m_editor->setPlainText(text);
        resetCurrentFormat();
    }
    m_fileName = fileName;
    m_editor->document()->setModified(false);
    updateWindowTitle();
    return true;
}

bool BtTextEditorWindow::saveFile(QString const & fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(
                    this,
                    tr("Text Editor"),
                    tr("Could not save %1.").arg(fileName));
        return false;
    }

    QTextStream stream(&file);
    auto const suffix = QFileInfo(fileName).suffix().toLower();
    stream << ((suffix == QStringLiteral("txt"))
               ? m_editor->toPlainText()
               : m_editor->toHtml());
    m_fileName = fileName;
    m_editor->document()->setModified(false);
    updateWindowTitle();
    return true;
}
