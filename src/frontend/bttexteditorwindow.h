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

class QAction;
class QCloseEvent;
class QEvent;
class QTextCharFormat;
class QTextEdit;

class BtTextEditorWindow final: public QMainWindow {

    Q_OBJECT

public:

    explicit BtTextEditorWindow(QWidget * parent = nullptr);

protected:

    void closeEvent(QCloseEvent * event) override;
    bool eventFilter(QObject * watched, QEvent * event) override;

private Q_SLOTS:

    void newDocument();
    void openDocument();
    bool saveDocument();
    bool saveDocumentAs();
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
    void formatHeading1();
    void formatHeading2();
    void formatNormalText();
    void alignLeft();
    void alignCenter();
    void alignRight();
    void showEditorHelp();
    void updateWindowTitle();

private:

    void createActions();
    void createMenus();
    void restoreEditorState();
    void saveEditorState();
    bool maybeSave();
    bool loadFile(QString const & fileName);
    bool saveFile(QString const & fileName);
    void mergeCurrentCharFormat(QTextCharFormat const & format);
    void setBlockHeading(int pointSize, int weight);
    void resetCurrentFormat();

    QTextEdit * m_editor;
    QString m_fileName;
    QColor m_highlightColor;
    QList<QAction *> m_fileActions;
    QList<QAction *> m_formatActions;
    QList<QAction *> m_helpActions;

};
