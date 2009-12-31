/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CFONTCHOOSER_H
#define CFONTCHOOSER_H

#include <QFrame>

#include <QWidget>


class CListWidget;
class QListWidget;
class QListWidgetItem;
class QString;
class QVBoxLayout;
class QWebView;

class CFontChooser : public QFrame {
        Q_OBJECT

    public:
        CFontChooser(QWidget *parent = 0);
        ~CFontChooser();
        void setFont(const QFont& font);
        void setSampleText(const QString& text);
        QSize sizeHint() const;

    private:
        void createFontAreaLayout();
        void createLayout();
        void createTextAreaLayout();
        void connectListWidgets();
        QString formatAsHtml(const QString& text);
        void loadFonts();
        void loadSizes(const QString& font, const QString& style);
        void loadStyles(const QString& font);
        void outputHtmlText();
        void restoreListWidgetValue(QListWidget* listWidget, const QString& value);
        QString saveListWidgetValue(QListWidget* listWidget);

        QFrame* m_fontWidget;
        QWebView* m_webView;
        CListWidget* m_fontListWidget;
        CListWidget* m_styleListWidget;
        CListWidget* m_sizeListWidget;
        QString m_htmlText;
        QFont m_font;
        QVBoxLayout* m_vBoxLayout;
        QString m_choosenStyle;

    private slots:
        void fontChanged(QListWidgetItem* current, QListWidgetItem* previous);
        void setFontStyle(const QString& styleString, QFont* font);
        void sizeChanged(QListWidgetItem* current, QListWidgetItem* previous);
        void styleChanged(QListWidgetItem* current, QListWidgetItem* previous);

    signals:
        void fontSelected(const QFont&);
};

#endif
