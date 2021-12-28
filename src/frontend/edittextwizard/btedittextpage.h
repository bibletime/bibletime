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

#pragma once

#include <QWizardPage>

#include <QColor>
#include <QFont>
#include <QObject>
#include <QString>


class BtColorWidget;
class BtEditTextWizard;
class BtFontSizeWidget;
class QAction;
class QFontComboBox;
class QTextEdit;
class QToolBar;

class BtEditTextPage final: public QWizardPage {

    Q_OBJECT

public: // methods:

    BtEditTextPage(BtEditTextWizard & parent);

    void setFont(const QFont& font);
    void setText(const QString& text);
    void setTitle(const QString& text);
    QString text() const;

    void cleanupPage() final override;
    void initializePage() final override;
    bool validatePage() final override;

Q_SIGNALS:

    void signalFontChanged(const QFont &);
    void signalFontColorChanged(const QColor &);
    void signalFontSizeChanged(int);

private:
    void alignmentChanged(int);
    void initActions();
    QFont initHtmlFont();
    void retranslateUi();
    void setupToolBar();


    BtEditTextWizard & m_wizard;
    QTextEdit * m_plainTextEdit;
    QTextEdit * m_htmlTextEdit;
    QToolBar * m_toolBar;
        QFontComboBox * m_fontFamilyComboBox;
        BtFontSizeWidget * m_fontSizeWidget;
        BtColorWidget * m_fontColorChooser;
    bool m_handingFormatChangeFromEditor;

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
