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

#pragma once

#include <QColor>
#include <QList>
#include <QMainWindow>
#include <QString>
#include <QStringList>

class QAction;
class QCloseEvent;
class QEvent;
class QLabel;
class QTextCharFormat;
class QTextEdit;
class QTimer;

class BtTextEditorWindow final: public QMainWindow {

    Q_OBJECT

public:

    explicit BtTextEditorWindow(QWidget * parent = nullptr);
    ~BtTextEditorWindow() override;

    static BtTextEditorWindow * activeWindow();
    static bool appendToActiveDocument(QString const & text,
                                       QString const & title = QString());

    void appendText(QString const & text, QString const & title = QString());

protected:

    void closeEvent(QCloseEvent * event) override;
    bool eventFilter(QObject * watched, QEvent * event) override;

private Q_SLOTS:

    void newDocument();
    void openDocument();
    bool saveDocument();
    bool saveDocumentAs();
    void printDocument();
    void findText();
    void findNext();
    void findPrevious();
    void insertTable();
    void insertPicture();
    void chooseFont();
    void checkSpelling();
    void showThesaurus();
    void formatBold();
    void formatItalic();
    void formatUnderline();
    void formatHighlight();
    void chooseHighlightColor();
    void chooseFontColor();
    void chooseBackgroundColor();
    void formatBulletList();
    void formatNumberedList();
    void formatLink();
    void formatSingleSpacing();
    void formatDoubleSpacing();
    void formatHeading1();
    void formatHeading2();
    void formatNormalText();
    void alignLeft();
    void alignCenter();
    void alignRight();
    void showEditorHelp();
    void saveAutoSave();
    void updateEditorStatus();
    void updateWindowTitle();

private:

    void createActions();
    void createMenus();
    void createStatusBar();
    void createToolBars();
    void restoreAutoSave();
    void removeAutoSave();
    bool createBackupFile(QString const & fileName);
    void restoreEditorState();
    void saveEditorState();
    bool maybeSave();
    bool loadFile(QString const & fileName);
    bool saveFile(QString const & fileName);
    void setPageBackgroundColor(QColor const & color,
                                bool markModified = true);
    QString htmlWithPageBackground() const;
    void mergeCurrentCharFormat(QTextCharFormat const & format);
    void mergeCurrentBlockFormat(int proportionalLineHeight,
                                 double topMargin,
                                 double bottomMargin);
    void setBlockHeading(int pointSize, int weight);
    void resetCurrentFormat();
    QString wordAtCursor() const;
    QStringList spellingSuggestions(QString const & word) const;
    bool isWordSpelledCorrectly(QString const & word) const;
    QStringList thesaurusSuggestions(QString const & word) const;

    QTextEdit * m_editor;
    QString m_fileName;
    QString m_lastSearchText;
    QColor m_highlightColor;
    QColor m_pageBackgroundColor;
    QLabel * m_formatStatus;
    QLabel * m_dateTimeStatus;
    QLabel * m_positionStatus;
    QLabel * m_saveStatus;
    QTimer * m_autoSaveTimer;
    QTimer * m_clockTimer;
    QList<QAction *> m_fileActions;
    QList<QAction *> m_editActions;
    QList<QAction *> m_searchActions;
    QList<QAction *> m_formatActions;
    QList<QAction *> m_insertActions;
    QList<QAction *> m_toolsActions;
    QList<QAction *> m_helpActions;

};
