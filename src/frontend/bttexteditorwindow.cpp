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
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QFontDialog>
#include <QIODevice>
#include <QInputDialog>
#include <QLabel>
#include <QKeySequence>
#include <QLineEdit>
#include <QListWidget>
#include <QLocale>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPalette>
#include <QPointer>
#include <QPrintDialog>
#include <QPrinter>
#include <QProcess>
#include <QRegularExpression>
#include <QSet>
#include <QSize>
#include <QSpinBox>
#include <QStandardPaths>
#include <QStatusBar>
#include <QTextBlockFormat>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextEdit>
#include <QTextFrame>
#include <QTextFrameFormat>
#include <QTextImageFormat>
#include <QTextListFormat>
#include <QTextStream>
#include <QTextTableFormat>
#include <QTimer>
#include <QToolBar>
#include <QUrl>
#include <QVBoxLayout>
#include <optional>
#include "../backend/config/btconfig.h"
#include "../util/btconnect.h"
#include "../util/cresmgr.h"


namespace {

auto const LastFileKey = QStringLiteral("GUI/TextEditor/lastFile");
auto const CursorPositionKey = QStringLiteral("GUI/TextEditor/cursorPosition");

QPointer<BtTextEditorWindow> activeTextEditorWindow;

QString autoSaveFileName() {
    auto dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (dir.isEmpty())
        dir = QDir::tempPath();

    QDir().mkpath(dir);
    return QDir(dir).filePath(QStringLiteral("text-editor-autosave.html"));
}

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

std::optional<QColor> colorFromCssValue(QString value) {
    value = value.trimmed();
    if (value.endsWith(';'))
        value.chop(1);

    auto const color = QColor(value.trimmed());
    if (color.isValid())
        return color;

    return std::nullopt;
}

std::optional<QColor> bodyBackgroundColor(QString const & html) {
    auto const bodyMatch =
            QRegularExpression(
                QStringLiteral("<body\\b([^>]*)>"),
                QRegularExpression::CaseInsensitiveOption)
            .match(html);
    if (!bodyMatch.hasMatch())
        return std::nullopt;

    auto const bodyAttributes = bodyMatch.captured(1);
    auto const bgcolorMatch =
            QRegularExpression(
                QStringLiteral(
                    "\\bbgcolor\\s*=\\s*(?:\"([^\"]*)\"|'([^']*)'|([^\\s>]*))"),
                QRegularExpression::CaseInsensitiveOption)
            .match(bodyAttributes);
    if (bgcolorMatch.hasMatch()) {
        for (int i = 1; i <= 3; i++) {
            auto const captured = bgcolorMatch.captured(i);
            if (captured.isEmpty())
                continue;
            auto const color = QColor(captured.trimmed());
            if (color.isValid())
                return color;
        }
    }

    auto const styleMatch =
            QRegularExpression(
                QStringLiteral(
                    "\\bstyle\\s*=\\s*(?:\"([^\"]*)\"|'([^']*)')"),
                QRegularExpression::CaseInsensitiveOption)
            .match(bodyAttributes);
    if (!styleMatch.hasMatch())
        return std::nullopt;

    auto const style =
            !styleMatch.captured(1).isEmpty()
            ? styleMatch.captured(1)
            : styleMatch.captured(2);
    auto const backgroundMatch =
            QRegularExpression(
                QStringLiteral(
                    "(?:^|;)\\s*background(?:-color)?\\s*:\\s*([^;]+)"),
                QRegularExpression::CaseInsensitiveOption)
            .match(style);
    if (!backgroundMatch.hasMatch())
        return std::nullopt;

    return colorFromCssValue(backgroundMatch.captured(1));
}

QString bodyTagWithBackground(QString bodyTag, QColor const & color) {
    auto const colorName = color.name(QColor::HexRgb);

    bodyTag.remove(
                QRegularExpression(
                    QStringLiteral(
                        "\\s+bgcolor\\s*=\\s*(?:\"[^\"]*\"|'[^']*'|[^\\s>]*)"),
                    QRegularExpression::CaseInsensitiveOption));

    auto const styleExpression =
            QRegularExpression(
                QStringLiteral("\\bstyle\\s*=\\s*(\"([^\"]*)\"|'([^']*)')"),
                QRegularExpression::CaseInsensitiveOption);
    auto const styleMatch = styleExpression.match(bodyTag);
    if (styleMatch.hasMatch()) {
        auto style =
                !styleMatch.captured(2).isEmpty()
                ? styleMatch.captured(2)
                : styleMatch.captured(3);
        style.remove(
                    QRegularExpression(
                        QStringLiteral(
                            "(?:^|;)\\s*background(?:-color)?\\s*:[^;]*;?"),
                        QRegularExpression::CaseInsensitiveOption));
        style = QStringLiteral("background-color:%1; %2")
                .arg(colorName, style.trimmed());
        bodyTag.replace(styleMatch.capturedStart(1),
                        styleMatch.capturedLength(1),
                        QStringLiteral("\"%1\"").arg(style.trimmed()));
    } else {
        bodyTag.insert(bodyTag.size() - 1,
                       QStringLiteral(" style=\"background-color:%1;\"")
                       .arg(colorName));
    }

    bodyTag.insert(bodyTag.size() - 1,
                   QStringLiteral(" bgcolor=\"%1\"").arg(colorName));
    return bodyTag;
}

QString aspellPath() {
    return QStandardPaths::findExecutable(QStringLiteral("aspell"));
}

QString normalizeWord(QString word) {
    word = word.trimmed();
    while (word.startsWith('\''))
        word.remove(0, 1);
    while (word.endsWith('\''))
        word.chop(1);
    return word;
}

QTextCharFormat statusCharFormat(QTextEdit * const editor,
                                 QTextCursor cursor)
{
    auto * const document = editor->document();
    auto const characterCount = document->characterCount();
    if (characterCount > 1) {
        auto position =
                cursor.hasSelection()
                ? cursor.selectionStart()
                : cursor.position();
        position = qBound(0, position, characterCount - 2);

        QTextCursor probe(document);
        probe.setPosition(position);
        probe.movePosition(QTextCursor::NextCharacter,
                           QTextCursor::KeepAnchor);
        cursor = probe;
    }

    return cursor.charFormat();
}

std::optional<QString> selectReplacement(QWidget * const parent,
                                         QString const & title,
                                         QString const & prompt,
                                         QStringList const & choices)
{
    QDialog dialog(parent);
    dialog.setWindowTitle(title);

    auto * const layout = new QVBoxLayout(&dialog);
    layout->addWidget(new QLabel(prompt, &dialog));

    auto * const list = new QListWidget(&dialog);
    list->addItems(choices);
    list->setCurrentRow(0);
    layout->addWidget(list);

    auto * const buttons =
            new QDialogButtonBox(QDialogButtonBox::Ok
                                 | QDialogButtonBox::Cancel,
                                 &dialog);
    layout->addWidget(buttons);

    QString selectedChoice;
    QObject::connect(list, &QListWidget::itemClicked, &dialog,
                     [&dialog, &selectedChoice](QListWidgetItem * const item) {
                        selectedChoice = item->text();
                        dialog.accept();
                     });
    QObject::connect(list, &QListWidget::itemActivated, &dialog,
                     [&dialog, &selectedChoice](QListWidgetItem * const item) {
                        selectedChoice = item->text();
                        dialog.accept();
                     });
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog,
                     [&dialog, list, &selectedChoice] {
                        if (auto const * const item = list->currentItem())
                            selectedChoice = item->text();
                        dialog.accept();
                     });
    QObject::connect(buttons, &QDialogButtonBox::rejected,
                     &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted || selectedChoice.isEmpty())
        return std::nullopt;

    return selectedChoice;
}

} // anonymous namespace

BtTextEditorWindow::BtTextEditorWindow(QWidget * const parent)
    : QMainWindow(parent)
    , m_editor(new QTextEdit(this))
    , m_highlightColor(QColor(255, 242, 128))
    , m_pageBackgroundColor(Qt::white)
    , m_formatStatus(new QLabel(this))
    , m_dateTimeStatus(new QLabel(this))
    , m_positionStatus(new QLabel(this))
    , m_saveStatus(new QLabel(this))
    , m_autoSaveTimer(new QTimer(this))
    , m_clockTimer(new QTimer(this))
{
    activeTextEditorWindow = this;
    setCentralWidget(m_editor);
    m_editor->setAcceptRichText(true);
    m_editor->viewport()->installEventFilter(this);
    setPageBackgroundColor(m_pageBackgroundColor, false);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    BT_CONNECT(m_editor->document(), &QTextDocument::modificationChanged,
               this,                 &BtTextEditorWindow::updateWindowTitle);
    BT_CONNECT(m_editor->document(), &QTextDocument::modificationChanged,
               this,                 &BtTextEditorWindow::updateEditorStatus);
    BT_CONNECT(m_editor, &QTextEdit::cursorPositionChanged,
               this,     &BtTextEditorWindow::updateEditorStatus);
    BT_CONNECT(m_editor, &QTextEdit::selectionChanged,
               this,     &BtTextEditorWindow::updateEditorStatus);
    BT_CONNECT(m_editor, &QTextEdit::currentCharFormatChanged,
               this,     [this] { updateEditorStatus(); });
    BT_CONNECT(m_clockTimer, &QTimer::timeout,
               this,         &BtTextEditorWindow::updateEditorStatus);
    BT_CONNECT(m_autoSaveTimer, &QTimer::timeout,
               this,           &BtTextEditorWindow::saveAutoSave);
    m_clockTimer->start(60000);
    m_autoSaveTimer->start(60000);

    resize(760, 600);
    restoreEditorState();
    restoreAutoSave();
    updateWindowTitle();
    updateEditorStatus();
}

BtTextEditorWindow::~BtTextEditorWindow() {
    if (activeTextEditorWindow == this)
        activeTextEditorWindow.clear();
}

BtTextEditorWindow * BtTextEditorWindow::activeWindow()
{ return activeTextEditorWindow.data(); }

bool BtTextEditorWindow::appendToActiveDocument(QString const & text,
                                                QString const & title)
{
    auto * const editor = activeWindow();
    if (!editor)
        return false;
    editor->appendText(text, title);
    return true;
}

void BtTextEditorWindow::appendText(QString const & text,
                                    QString const & title)
{
    auto cursor = m_editor->textCursor();
    auto const startedWithSelection = cursor.hasSelection();

    if (!title.isEmpty()) {
        QTextCharFormat titleFormat;
        titleFormat.setFontWeight(QFont::Bold);
        cursor.insertText(title, titleFormat);
        cursor.insertBlock();
    } else if (startedWithSelection) {
        cursor.removeSelectedText();
    }

    cursor.insertText(text);
    cursor.insertBlock();
    m_editor->setTextCursor(cursor);
    m_editor->ensureCursorVisible();
    m_editor->document()->setModified(true);
    raise();
    activateWindow();
}

void BtTextEditorWindow::closeEvent(QCloseEvent * const event) {
    if (maybeSave()) {
        saveEditorState();
        removeAutoSave();
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
    removeAutoSave();
    m_fileName.clear();
    m_editor->clear();
    setPageBackgroundColor(Qt::white, false);
    resetCurrentFormat();
    m_editor->document()->setModified(false);
    updateWindowTitle();
}

void BtTextEditorWindow::openDocument() {
    if (!maybeSave())
        return;
    removeAutoSave();
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

void BtTextEditorWindow::printDocument() {
    QPrinter printer;
    QPrintDialog printDialog(&printer, this);
    printDialog.setWindowTitle(tr("Print Text Document"));
    if (printDialog.exec() == QDialog::Accepted)
        m_editor->print(&printer);
}

void BtTextEditorWindow::findText() {
    bool ok = false;
    auto const text =
            QInputDialog::getText(this,
                                  tr("Find"),
                                  tr("Find:"),
                                  QLineEdit::Normal,
                                  m_lastSearchText,
                                  &ok);
    if (!ok || text.isEmpty())
        return;

    m_lastSearchText = text;
    findNext();
}

void BtTextEditorWindow::findNext() {
    if (m_lastSearchText.isEmpty()) {
        findText();
        return;
    }

    if (!m_editor->find(m_lastSearchText)) {
        auto cursor = m_editor->textCursor();
        cursor.movePosition(QTextCursor::Start);
        m_editor->setTextCursor(cursor);
        m_editor->find(m_lastSearchText);
    }
}

void BtTextEditorWindow::findPrevious() {
    if (m_lastSearchText.isEmpty()) {
        findText();
        return;
    }

    if (!m_editor->find(m_lastSearchText, QTextDocument::FindBackward)) {
        auto cursor = m_editor->textCursor();
        cursor.movePosition(QTextCursor::End);
        m_editor->setTextCursor(cursor);
        m_editor->find(m_lastSearchText, QTextDocument::FindBackward);
    }
}

void BtTextEditorWindow::insertTable() {
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Insert Table"));

    auto * const layout = new QVBoxLayout(&dialog);

    auto * const rowsLabel = new QLabel(tr("Rows:"), &dialog);
    auto * const rows = new QSpinBox(&dialog);
    rows->setRange(1, 100);
    rows->setValue(2);
    rowsLabel->setBuddy(rows);
    layout->addWidget(rowsLabel);
    layout->addWidget(rows);

    auto * const columnsLabel = new QLabel(tr("Columns:"), &dialog);
    auto * const columns = new QSpinBox(&dialog);
    columns->setRange(1, 20);
    columns->setValue(2);
    columnsLabel->setBuddy(columns);
    layout->addWidget(columnsLabel);
    layout->addWidget(columns);

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

    QTextTableFormat format;
    format.setBorder(1);
    format.setCellPadding(4);
    format.setCellSpacing(0);
    m_editor->textCursor().insertTable(rows->value(), columns->value(),
                                       format);
    m_editor->document()->setModified(true);
}

void BtTextEditorWindow::insertPicture() {
    auto const fileName =
            QFileDialog::getOpenFileName(
                this,
                tr("Insert Picture"),
                QString(),
                tr("Images (*.png *.jpg *.jpeg *.bmp *.gif);;All files (*)"));
    if (fileName.isEmpty())
        return;

    QTextImageFormat format;
    format.setName(QUrl::fromLocalFile(fileName).toString());
    m_editor->textCursor().insertImage(format);
    m_editor->document()->setModified(true);
}

void BtTextEditorWindow::chooseFont() {
    bool ok = false;
    auto const font =
            QFontDialog::getFont(&ok,
                                 m_editor->currentFont(),
                                 this,
                                 tr("Select Font"));
    if (!ok)
        return;

    QTextCharFormat format;
    format.setFont(font);
    mergeCurrentCharFormat(format);
}

void BtTextEditorWindow::checkSpelling() {
    if (aspellPath().isEmpty()) {
        QMessageBox::warning(
                    this,
                    tr("Spell Check"),
                    tr("Aspell was not found. Install aspell and a dictionary "
                       "to use spell check."));
        return;
    }

    auto cursor = m_editor->textCursor();
    cursor.movePosition(QTextCursor::Start);
    m_editor->setTextCursor(cursor);

    QSet<QString> ignoredWords;
    QRegularExpression wordPattern(QStringLiteral("\\b[\\p{L}'][\\p{L}'-]*\\b"));
    while (true) {
        cursor = m_editor->document()->find(wordPattern, cursor);
        if (cursor.isNull())
            break;

        auto const word = normalizeWord(cursor.selectedText());
        if (word.size() < 2 || ignoredWords.contains(word.toLower())
            || isWordSpelledCorrectly(word))
        {
            continue;
        }

        QTextEdit::ExtraSelection misspelledSelection;
        misspelledSelection.cursor = cursor;
        misspelledSelection.format.setBackground(QColor(255, 220, 220));
        misspelledSelection.format.setUnderlineColor(Qt::red);
        misspelledSelection.format.setUnderlineStyle(
                    QTextCharFormat::SpellCheckUnderline);
        m_editor->setExtraSelections({misspelledSelection});
        m_editor->setTextCursor(cursor);
        m_editor->ensureCursorVisible();

        auto suggestions = spellingSuggestions(word);
        suggestions.prepend(tr("Ignore"));
        suggestions.append(tr("Add to ignored words"));

        auto const replacement =
                selectReplacement(
                    this,
                    tr("Spell Check"),
                    tr("Replace \"%1\" with:").arg(word),
                    suggestions);
        if (!replacement) {
            m_editor->setExtraSelections({});
            return;
        }

        if (*replacement == tr("Ignore")) {
            m_editor->setExtraSelections({});
            continue;
        }
        if (*replacement == tr("Add to ignored words")) {
            ignoredWords.insert(word.toLower());
            m_editor->setExtraSelections({});
            continue;
        }

        cursor.insertText(*replacement);
        m_editor->document()->setModified(true);
        m_editor->setExtraSelections({});
    }

    m_editor->setExtraSelections({});

    QMessageBox::information(this,
                             tr("Spell Check"),
                             tr("Spell check is complete."));
}

void BtTextEditorWindow::showThesaurus() {
    auto const word = wordAtCursor();
    if (word.isEmpty())
        return;

    auto suggestions = thesaurusSuggestions(word);
    if (suggestions.isEmpty()) {
        QMessageBox::information(
                    this,
                    tr("Thesaurus"),
                    tr("No thesaurus entries were found for \"%1\".")
                        .arg(word));
        return;
    }

    auto const replacement =
            selectReplacement(this,
                              tr("Thesaurus"),
                              tr("Replace \"%1\" with:").arg(word),
                              suggestions);
    if (!replacement)
        return;

    auto cursor = m_editor->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.insertText(*replacement);
    m_editor->setTextCursor(cursor);
    m_editor->document()->setModified(true);
}

void BtTextEditorWindow::updateWindowTitle() {
    auto const displayName =
            m_fileName.isEmpty()
            ? tr("Untitled")
            : QFileInfo(m_fileName).fileName();
    setWindowTitle(tr("%1[*] - Text Editor").arg(displayName));
    setWindowModified(m_editor->document()->isModified());
}

void BtTextEditorWindow::updateEditorStatus() {
    auto const cursor = m_editor->textCursor();
    auto const charFormat = statusCharFormat(m_editor, cursor);
    auto const blockFormat = cursor.blockFormat();
    auto const font = charFormat.font();
    auto const defaultFont = m_editor->document()->defaultFont();

    QStringList formats;
    auto const fontFamilies = charFormat.fontFamilies().toStringList();
    formats.append(charFormat.hasProperty(QTextFormat::FontFamilies)
                   && !fontFamilies.isEmpty()
                   ? fontFamilies.constFirst()
                   : defaultFont.family());
    auto const pointSize =
            charFormat.hasProperty(QTextFormat::FontPointSize)
            && charFormat.fontPointSize() > 0.0
            ? charFormat.fontPointSize()
            : defaultFont.pointSizeF();
    if (pointSize > 0.0)
        formats.append(tr("%1 pt").arg(pointSize));
    if (charFormat.fontWeight() >= QFont::Bold)
        formats.append(tr("Bold"));
    if (font.italic())
        formats.append(tr("Italic"));
    if (font.underline())
        formats.append(tr("Underline"));
    if (charFormat.background().style() != Qt::NoBrush)
        formats.append(tr("Highlight"));
    if (cursor.currentList())
        formats.append(tr("List"));
    if (cursor.hasSelection())
        formats.append(tr("Selection"));

    switch (blockFormat.alignment() & Qt::AlignHorizontal_Mask) {
        case Qt::AlignHCenter:
            formats.append(tr("Centered"));
            break;
        case Qt::AlignRight:
            formats.append(tr("Right"));
            break;
        default:
            formats.append(tr("Left"));
            break;
    }

    auto const lineHeight =
            blockFormat.lineHeightType() == QTextBlockFormat::ProportionalHeight
            ? blockFormat.lineHeight()
            : 100.0;
    formats.append(qFuzzyCompare(lineHeight, 200.0)
                   ? tr("Double spacing")
                   : tr("Single spacing"));

    m_formatStatus->setText(tr("Format: %1").arg(formats.join(QStringLiteral(", "))));
    m_positionStatus->setText(
                tr("Line %1, Column %2, Pos %3")
                .arg(cursor.blockNumber() + 1)
                .arg(cursor.positionInBlock() + 1)
                .arg(cursor.position()));
    m_dateTimeStatus->setText(
                QLocale().toString(QDateTime::currentDateTime(),
                                   QLocale::ShortFormat));
    m_saveStatus->setText(m_editor->document()->isModified()
                          ? tr("Unsaved")
                          : tr("Saved"));
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

    auto * const printAction = new QAction(this);
    printAction->setShortcut(QKeySequence::Print);
    printAction->setText(tr("&Print..."));
    BT_CONNECT(printAction, &QAction::triggered,
               this,        &BtTextEditorWindow::printDocument);
    addAction(printAction);
    m_fileActions.append(printAction);

    addEditorAction(m_editActions, tr("&Undo"), QKeySequence::Undo,
                    [this] { m_editor->undo(); });
    addEditorAction(m_editActions, tr("&Redo"), QKeySequence::Redo,
                    [this] { m_editor->redo(); });
    addEditorAction(m_editActions, tr("Cu&t"), QKeySequence::Cut,
                    [this] { m_editor->cut(); });
    addEditorAction(m_editActions, tr("&Copy"), QKeySequence::Copy,
                    [this] { m_editor->copy(); });
    addEditorAction(m_editActions, tr("&Paste"), QKeySequence::Paste,
                    [this] { m_editor->paste(); });
    addEditorAction(m_editActions, tr("Select &All"), QKeySequence::SelectAll,
                    [this] { m_editor->selectAll(); });

    addEditorAction(m_searchActions, tr("&Find..."), QKeySequence::Find,
                    &BtTextEditorWindow::findText);
    addEditorAction(m_searchActions, tr("Find &Next"), QKeySequence::FindNext,
                    &BtTextEditorWindow::findNext);
    addEditorAction(m_searchActions, tr("Find &Previous"),
                    QKeySequence::FindPrevious,
                    &BtTextEditorWindow::findPrevious);

    addEditorAction(m_insertActions, tr("&Table..."),
                    QKeySequence(tr("Ctrl+Shift+T",
                                    "Text Editor|Insert Table")),
                    &BtTextEditorWindow::insertTable);
    addEditorAction(m_insertActions, tr("&Picture..."),
                    QKeySequence(tr("Ctrl+Shift+P",
                                    "Text Editor|Insert Picture")),
                    &BtTextEditorWindow::insertPicture);

    addEditorAction(m_toolsActions, tr("&Spell Check..."),
                    QKeySequence(tr("F7", "Text Editor|Spell Check")),
                    &BtTextEditorWindow::checkSpelling);
    addEditorAction(m_toolsActions, tr("&Thesaurus..."),
                    QKeySequence(tr("Shift+F7", "Text Editor|Thesaurus")),
                    &BtTextEditorWindow::showThesaurus);

    addEditorAction(m_formatActions, tr("&Font..."),
                    QKeySequence(tr("Ctrl+Shift+F", "Text Editor|Font")),
                    &BtTextEditorWindow::chooseFont);
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
    addEditorAction(m_formatActions, tr("&Single Spacing"),
                    QKeySequence(tr("Ctrl+1", "Text Editor|Single Spacing")),
                    &BtTextEditorWindow::formatSingleSpacing);
    addEditorAction(m_formatActions, tr("&Double Spacing"),
                    QKeySequence(tr("Ctrl+2", "Text Editor|Double Spacing")),
                    &BtTextEditorWindow::formatDoubleSpacing);
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

    m_fileActions.at(0)->setIcon(CResMgr::icon_clearEdit());
    m_fileActions.at(1)->setIcon(CResMgr::mainWindow::icon_openAction());
    m_fileActions.at(2)->setIcon(CResMgr::searchdialog::result::moduleList::saveMenu::icon());
    m_fileActions.at(3)->setIcon(CResMgr::searchdialog::result::moduleList::saveMenu::icon());
    m_fileActions.at(4)->setIcon(CResMgr::searchdialog::result::moduleList::printMenu::icon());

    m_editActions.at(0)->setIcon(CResMgr::findWidget::icon_previous());
    m_editActions.at(1)->setIcon(CResMgr::findWidget::icon_next());
    m_editActions.at(2)->setIcon(CResMgr::icon_editCut());
    m_editActions.at(3)->setIcon(CResMgr::searchdialog::result::moduleList::copyMenu::icon());
    m_editActions.at(4)->setIcon(CResMgr::icon_editPaste());
    m_editActions.at(5)->setIcon(CResMgr::categories::bibles::icon());

    m_searchActions.at(0)->setIcon(CResMgr::mainIndex::search::icon());
    m_searchActions.at(1)->setIcon(CResMgr::findWidget::icon_next());
    m_searchActions.at(2)->setIcon(CResMgr::findWidget::icon_previous());

    m_insertActions.at(0)->setIcon(CResMgr::icon_insertTable());
    m_insertActions.at(1)->setIcon(CResMgr::icon_insertPicture());

    m_toolsActions.at(0)->setIcon(CResMgr::icon_spellCheck());
    m_toolsActions.at(1)->setIcon(CResMgr::categories::lexicons::icon());

    m_formatActions.at(0)->setIcon(CResMgr::settings::fonts::icon());
    m_formatActions.at(1)->setIcon(CResMgr::displaywindows::writeWindow::boldText::icon());
    m_formatActions.at(2)->setIcon(CResMgr::displaywindows::writeWindow::italicText::icon());
    m_formatActions.at(3)->setIcon(CResMgr::displaywindows::writeWindow::underlinedText::icon());
    m_formatActions.at(4)->setIcon(CResMgr::icon_highlight());
    m_formatActions.at(5)->setIcon(CResMgr::icon_highlight());
    m_formatActions.at(6)->setIcon(CResMgr::settings::fonts::icon());
    m_formatActions.at(7)->setIcon(CResMgr::displaywindows::displaySettings::icon());
    m_formatActions.at(8)->setIcon(CResMgr::settings::fonts::icon());
    m_formatActions.at(9)->setIcon(CResMgr::categories::books::icon());
    m_formatActions.at(10)->setIcon(CResMgr::categories::books::icon());
    m_formatActions.at(11)->setIcon(CResMgr::icon_bulletedList());
    m_formatActions.at(12)->setIcon(CResMgr::icon_numberedList());
    m_formatActions.at(13)->setIcon(CResMgr::icon_link());
    m_formatActions.at(16)->setIcon(CResMgr::displaywindows::writeWindow::alignLeft::icon());
    m_formatActions.at(17)->setIcon(CResMgr::displaywindows::writeWindow::alignCenter::icon());
    m_formatActions.at(18)->setIcon(CResMgr::displaywindows::writeWindow::alignRight::icon());

    m_helpActions.at(0)->setIcon(CResMgr::mainMenu::help::handbook::icon());
}

void BtTextEditorWindow::createMenus() {
    auto * const fileMenu = menuBar()->addMenu(tr("&File"));
    for (auto * const action : m_fileActions)
        fileMenu->addAction(action);

    auto * const editMenu = menuBar()->addMenu(tr("&Edit"));
    for (int i = 0; i < m_editActions.size(); i++) {
        if (i == 2 || i == 5)
            editMenu->addSeparator();
        editMenu->addAction(m_editActions.at(i));
    }

    auto * const searchMenu = menuBar()->addMenu(tr("&Search"));
    for (auto * const action : m_searchActions)
        searchMenu->addAction(action);

    auto * const insertMenu = menuBar()->addMenu(tr("&Insert"));
    for (auto * const action : m_insertActions)
        insertMenu->addAction(action);

    auto * const formatMenu = menuBar()->addMenu(tr("F&ormat"));
    for (int i = 0; i < m_formatActions.size(); i++) {
        if (i == 8 || i == 11 || i == 14 || i == 16)
            formatMenu->addSeparator();
        formatMenu->addAction(m_formatActions.at(i));
    }

    auto * const toolsMenu = menuBar()->addMenu(tr("&Tools"));
    for (auto * const action : m_toolsActions)
        toolsMenu->addAction(action);

    auto * const helpMenu = menuBar()->addMenu(tr("&Help"));
    for (auto * const action : m_helpActions)
        helpMenu->addAction(action);
}

void BtTextEditorWindow::createStatusBar() {
    m_formatStatus->setMinimumWidth(260);
    m_positionStatus->setMinimumWidth(190);
    m_dateTimeStatus->setMinimumWidth(180);
    m_saveStatus->setMinimumWidth(90);

    statusBar()->addWidget(m_formatStatus, 1);
    statusBar()->addPermanentWidget(m_positionStatus);
    statusBar()->addPermanentWidget(m_dateTimeStatus);
    statusBar()->addPermanentWidget(m_saveStatus);
}

void BtTextEditorWindow::createToolBars() {
    auto * const toolbar = addToolBar(tr("Text Editor"));
    toolbar->setObjectName(QStringLiteral("textEditorToolBar"));
    toolbar->setMovable(false);
    toolbar->setIconSize(QSize(18, 18));
    toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);

    toolbar->addAction(m_fileActions.at(0)); // New
    toolbar->addAction(m_fileActions.at(1)); // Open
    toolbar->addAction(m_fileActions.at(2)); // Save
    toolbar->addAction(m_fileActions.at(4)); // Print
    toolbar->addSeparator();

    toolbar->addAction(m_editActions.at(0)); // Undo
    toolbar->addAction(m_editActions.at(1)); // Redo
    toolbar->addSeparator();

    toolbar->addAction(m_editActions.at(2)); // Cut
    toolbar->addAction(m_editActions.at(3)); // Copy
    toolbar->addAction(m_editActions.at(4)); // Paste
    toolbar->addSeparator();

    toolbar->addAction(m_searchActions.at(0)); // Find
    toolbar->addAction(m_toolsActions.at(0)); // Spell Check
    toolbar->addAction(m_toolsActions.at(1)); // Thesaurus
    toolbar->addSeparator();

    toolbar->addAction(m_formatActions.at(1)); // Bold
    toolbar->addAction(m_formatActions.at(2)); // Italic
    toolbar->addAction(m_formatActions.at(3)); // Underline
    toolbar->addAction(m_formatActions.at(4)); // Highlight
    toolbar->addAction(m_formatActions.at(6)); // Font Color
    toolbar->addSeparator();

    toolbar->addAction(m_formatActions.at(11)); // Bullet List
    toolbar->addAction(m_formatActions.at(12)); // Numbered List
    toolbar->addAction(m_formatActions.at(13)); // Link
    toolbar->addSeparator();

    toolbar->addAction(m_insertActions.at(0)); // Table
    toolbar->addAction(m_insertActions.at(1)); // Picture
}

void BtTextEditorWindow::restoreAutoSave() {
    auto const fileName = autoSaveFileName();
    if (!QFileInfo::exists(fileName))
        return;

    auto const result =
            QMessageBox::question(
                this,
                tr("Text Editor"),
                tr("An autosaved recovery copy was found. Restore it?"),
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::Yes);
    if (result != QMessageBox::Yes) {
        removeAutoSave();
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream stream(&file);
    auto const text = stream.readAll();
    m_editor->setHtml(text);
    setPageBackgroundColor(bodyBackgroundColor(text).value_or(Qt::white),
                           false);
    m_editor->document()->setModified(true);
    updateWindowTitle();
    updateEditorStatus();
}

void BtTextEditorWindow::removeAutoSave()
{ QFile::remove(autoSaveFileName()); }

bool BtTextEditorWindow::createBackupFile(QString const & fileName) {
    if (!QFileInfo::exists(fileName))
        return true;

    auto const backupFileName = fileName + QStringLiteral(".bak");
    QFile::remove(backupFileName);
    if (QFile::copy(fileName, backupFileName))
        return true;

    auto const result =
            QMessageBox::warning(
                this,
                tr("Text Editor"),
                tr("Could not create backup file %1. Save anyway?")
                    .arg(backupFileName),
                QMessageBox::Save | QMessageBox::Cancel,
                QMessageBox::Cancel);
    return result == QMessageBox::Save;
}

void BtTextEditorWindow::restoreEditorState() {
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
                m_pageBackgroundColor,
                this,
                tr("Select Background Color"));
    if (!color.isValid())
        return;

    setPageBackgroundColor(color);
}

void BtTextEditorWindow::formatBulletList() {
    formatSingleSpacing();
    auto cursor = m_editor->textCursor();
    cursor.createList(QTextListFormat::ListDisc);
}

void BtTextEditorWindow::formatNumberedList() {
    formatSingleSpacing();
    auto cursor = m_editor->textCursor();
    cursor.createList(QTextListFormat::ListDecimal);
}

void BtTextEditorWindow::formatSingleSpacing()
{ mergeCurrentBlockFormat(100, 0.0, 0.0); }

void BtTextEditorWindow::formatDoubleSpacing()
{ mergeCurrentBlockFormat(200, 0.0, 0.0); }

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
                   "<tr><td><b>Ctrl+Alt+H</b></td><td>Choose highlight color</td></tr>"
                   "<tr><td><b>Ctrl+Shift+C</b></td><td>Choose font color</td></tr>"
                   "<tr><td><b>Ctrl+Shift+B</b></td><td>Choose page background color</td></tr>"
                   "<tr><td><b>Ctrl+Alt+1</b></td><td>Heading 1</td></tr>"
                   "<tr><td><b>Ctrl+Alt+2</b></td><td>Heading 2</td></tr>"
                   "<tr><td><b>Ctrl+Shift+8</b></td><td>Bullet list</td></tr>"
                   "<tr><td><b>Ctrl+Shift+7</b></td><td>Numbered list</td></tr>"
                   "<tr><td><b>Ctrl+K</b></td><td>Insert link</td></tr>"
                   "<tr><td><b>Ctrl+1</b></td><td>Single spacing</td></tr>"
                   "<tr><td><b>Ctrl+2</b></td><td>Double spacing</td></tr>"
                   "<tr><td><b>Ctrl+P</b></td><td>Print</td></tr>"
                   "<tr><td><b>Ctrl+F</b></td><td>Find text</td></tr>"
                   "<tr><td><b>F7</b></td><td>Spell check</td></tr>"
                   "<tr><td><b>Shift+F7</b></td><td>Thesaurus</td></tr>"
                   "</table>"
                   "<p>Documents are saved as HTML by default so formatting is "
                   "preserved. Plain text files do not preserve formatting.</p>"
                   "<p>Use the Insert menu to add pictures and simple tables.</p>"
                   "<p>Hold Ctrl and click a link to open it in your browser.</p>"));
}

void BtTextEditorWindow::saveAutoSave() {
    if (!m_editor->document()->isModified())
        return;

    QFile file(autoSaveFileName());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream stream(&file);
    stream << htmlWithPageBackground();
    statusBar()->showMessage(tr("Autosaved recovery copy."), 3000);
}

void BtTextEditorWindow::mergeCurrentCharFormat(
        QTextCharFormat const & format)
{
    auto cursor = m_editor->textCursor();
    cursor.mergeCharFormat(format);
    m_editor->mergeCurrentCharFormat(format);
    m_editor->document()->setModified(true);
}

void BtTextEditorWindow::mergeCurrentBlockFormat(
        int const proportionalLineHeight,
        double const topMargin,
        double const bottomMargin)
{
    QTextBlockFormat format;
    format.setLineHeight(proportionalLineHeight,
                         QTextBlockFormat::ProportionalHeight);
    format.setTopMargin(topMargin);
    format.setBottomMargin(bottomMargin);

    auto cursor = m_editor->textCursor();
    cursor.mergeBlockFormat(format);
    m_editor->setTextCursor(cursor);
    m_editor->document()->setModified(true);
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

QString BtTextEditorWindow::wordAtCursor() const {
    auto cursor = m_editor->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    return normalizeWord(cursor.selectedText());
}

QStringList BtTextEditorWindow::spellingSuggestions(QString const & word) const {
    QProcess aspell;
    aspell.start(aspellPath(),
                 {QStringLiteral("-a"), QStringLiteral("--lang=en_US")});
    if (!aspell.waitForStarted(1000))
        return {};

    aspell.write(word.toUtf8());
    aspell.write("\n");
    aspell.closeWriteChannel();
    aspell.waitForFinished(2000);

    QStringList suggestions;
    auto const lines =
            QString::fromUtf8(aspell.readAllStandardOutput()).split('\n');
    for (auto const & line : lines) {
        auto const colon = line.indexOf(':');
        if (!line.startsWith('&') || colon < 0)
            continue;

        for (auto suggestion :
             line.mid(colon + 1).split(',', Qt::SkipEmptyParts))
        {
            suggestion = suggestion.trimmed();
            if (!suggestion.isEmpty())
                suggestions.append(suggestion);
        }
    }
    suggestions.removeDuplicates();
    return suggestions;
}

bool BtTextEditorWindow::isWordSpelledCorrectly(QString const & word) const {
    QProcess aspell;
    aspell.start(aspellPath(),
                 {QStringLiteral("list"), QStringLiteral("--lang=en_US")});
    if (!aspell.waitForStarted(1000))
        return true;

    aspell.write(word.toUtf8());
    aspell.write("\n");
    aspell.closeWriteChannel();
    aspell.waitForFinished(2000);
    return QString::fromUtf8(aspell.readAllStandardOutput()).trimmed().isEmpty();
}

QStringList BtTextEditorWindow::thesaurusSuggestions(QString const & word) const {
    QFile file(QStringLiteral("/usr/share/mythes/th_en_US_v2.dat"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    auto const lookup = word.toLower();
    QTextStream stream(&file);
    stream.readLine(); // encoding
    QStringList suggestions;
    while (!stream.atEnd()) {
        auto const header = stream.readLine();
        auto const separator = header.lastIndexOf('|');
        if (separator < 0)
            continue;

        auto const entry = header.left(separator).toLower();
        auto const senseCount = header.mid(separator + 1).toInt();
        if (entry != lookup) {
            for (int i = 0; i < senseCount && !stream.atEnd(); i++)
                stream.readLine();
            continue;
        }

        for (int i = 0; i < senseCount && !stream.atEnd(); i++) {
            auto parts = stream.readLine().split('|', Qt::SkipEmptyParts);
            for (auto part : parts) {
                part = part.trimmed();
                if (part.startsWith('(') || part.endsWith(QStringLiteral(" term)"))
                    || part.compare(word, Qt::CaseInsensitive) == 0)
                {
                    continue;
                }
                suggestions.append(part);
            }
        }
        break;
    }

    suggestions.removeDuplicates();
    return suggestions;
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
    if (isHtmlDocument(fileName, text)) {
        m_editor->setHtml(text);
        setPageBackgroundColor(bodyBackgroundColor(text).value_or(Qt::white),
                               false);
    } else {
        m_editor->setPlainText(text);
        setPageBackgroundColor(Qt::white, false);
        resetCurrentFormat();
    }
    m_fileName = fileName;
    m_editor->document()->setModified(false);
    updateWindowTitle();
    return true;
}

bool BtTextEditorWindow::saveFile(QString const & fileName) {
    if (!createBackupFile(fileName))
        return false;

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
               : htmlWithPageBackground());
    m_fileName = fileName;
    m_editor->document()->setModified(false);
    removeAutoSave();
    updateWindowTitle();
    updateEditorStatus();
    return true;
}

void BtTextEditorWindow::setPageBackgroundColor(QColor const & color,
                                                bool const markModified)
{
    if (!color.isValid())
        return;

    m_pageBackgroundColor = color;

    auto palette = m_editor->palette();
    palette.setColor(QPalette::Base, color);
    palette.setColor(QPalette::Window, color);
    m_editor->setPalette(palette);
    m_editor->setAutoFillBackground(true);
    m_editor->viewport()->setPalette(palette);
    m_editor->viewport()->setAutoFillBackground(true);

    auto * const rootFrame = m_editor->document()->rootFrame();
    auto frameFormat = rootFrame->frameFormat();
    frameFormat.setBackground(color);
    rootFrame->setFrameFormat(frameFormat);

    if (markModified)
        m_editor->document()->setModified(true);
}

QString BtTextEditorWindow::htmlWithPageBackground() const {
    auto html = m_editor->toHtml();
    if (!m_pageBackgroundColor.isValid())
        return html;

    auto const bodyExpression =
            QRegularExpression(
                QStringLiteral("<body\\b[^>]*>"),
                QRegularExpression::CaseInsensitiveOption);
    auto const bodyMatch = bodyExpression.match(html);
    if (!bodyMatch.hasMatch())
        return html;

    html.replace(bodyMatch.capturedStart(),
                 bodyMatch.capturedLength(),
                 bodyTagWithBackground(bodyMatch.captured(0),
                                       m_pageBackgroundColor));
    return html;
}
