//
// C++ Interface: CFontChooser
//
// Description: BibleTime font chooser
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 1999-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CFONTCHOOSER_H
#define CFONTCHOOSER_H

// These following two defines allow chosing between using KDE and 
// Qt only for rendering the preview text
#define USE_KHTML
//#define USE_QWEBKIT


#include <QWidget>
#include <QFrame>

class QString;
class QFrame;
class QListWidget;
class QListWidgetItem;
class QVBoxLayout;
#ifdef USE_KHTML
class KHTMLPart;
#endif
#ifdef USE_QWEBKIT
class QWebView;
#endif
class CListWidget;

class CFontChooser : public QFrame {
	Q_OBJECT

public:
	CFontChooser(QWidget* parent = 0);
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
#ifdef USE_KHTML
	KHTMLPart* m_kHtmlPart;
#endif
#ifdef USE_QWEBKIT
	QWebView* m_webView;
#endif
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
