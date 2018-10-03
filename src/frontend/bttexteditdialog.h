/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BT_TEXT_EDIT_DIALOG_H
#define BT_TEXT_EDIT_DIALOG_H

/** Dialog for editing html text.
 * Used for editing entries of the Personal commentary.
 */

#include <QDialog>

class BtActionCollection;
class QDialogButtonBox;
class QAction;
class QTextCharFormat;
class QTextEdit;
class QToolBar;

class BtTextEditDialog : public QDialog {
    Q_OBJECT
public:
    BtTextEditDialog(QWidget* parent = nullptr);
    QString text() const;
    void setText(const QString& text);
    void setTitle(const QString& text);

private slots:
    void toggleBold(bool checked);
    void toggleItalic(bool checked);
    void toggleUnderline(bool checked);

    void alignLeft(bool);
    void alignCenter(bool);
    void alignRight(bool);

    void slotFontFamilyChosen(const QFont&);
    void slotFontSizeChosen(int);

    void slotCurrentCharFormatChanged(const QTextCharFormat &);

signals:

    void signalFontChanged(const QFont &);
    void signalFontSizeChanged(int);
    void signalFontColorChanged(const QColor &);

private:
    void alignmentChanged(int);
    void initActions();
    void setupToolBar();

    QDialogButtonBox * m_buttons;
    QTextEdit * m_textEdit;
    QToolBar * m_toolBar;
    bool m_handingFormatChangeFromEditor;

    struct {
        QAction* bold;
        QAction* italic;
        QAction* underline;

        QAction* alignLeft;
        QAction* alignCenter;
        QAction* alignRight;
    }
    m_actions;

};

#endif
