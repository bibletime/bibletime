/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BT_EDIT_TEXT_PAGE
#define BT_EDIT_TEXT_PAGE

#include <QWizardPage>
#include <QTextCharFormat>

class BtEditTextWizard;
class QTextEdit;
class QToolBar;
class QVBoxLayout;

class BtEditTextPage final: public QWizardPage {

    Q_OBJECT

public: /* Methods: */

    BtEditTextPage(QWidget * parent = 0);

    void setFont(const QFont& font);
    void setText(const QString& text);
    void setTitle(const QString& text);
    QString text() const;

    void cleanupPage() final override;
    void initializePage() final override;
    bool validatePage() final override;

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
    void signalFontColorChanged(const QColor &);
    void signalFontSizeChanged(int);

private:
    void alignmentChanged(int);
    void initActions();
    QFont initHtmlFont();
    void retranslateUi();
    void setupToolBar();


    QTextEdit * m_plainTextEdit;
    QTextEdit * m_htmlTextEdit;
    QToolBar * m_toolBar;
    BtEditTextWizard * m_wizard;
    bool m_handingFormatChangeFromEditor;

    bool m_htmlMode;
    QFont m_plainTextFont;
    QString m_text;

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
