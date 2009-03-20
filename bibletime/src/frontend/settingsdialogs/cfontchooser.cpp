/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "cfontchooser.h"
#include "cfontchooser.moc"
#include "clistwidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QFrame>
#include <QFontDatabase>
#include <QListWidgetItem>
#include <QWebSettings>
#include <QFrame>
#include <QWebView>


// ***********************
// Container for KHTHMView to control its size
class WebViewerWidget : public QWidget 
{
	public:
		WebViewerWidget(QWidget* parent = 0);
		~WebViewerWidget();
		virtual QSize sizeHint () const;
};

WebViewerWidget::WebViewerWidget(QWidget* parent)
	: QWidget(parent)
{
}

WebViewerWidget::~WebViewerWidget() 
{
}

QSize WebViewerWidget::sizeHint () const
{
	return QSize(100,85);
}
// ************************



CFontChooser::CFontChooser(QWidget* parent)
 :  QFrame(parent), m_fontWidget(0),
	m_fontListWidget(0), m_styleListWidget(0), m_sizeListWidget(0)
{
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	createLayout();
	connectListWidgets();
	loadFonts();
	setFrameStyle(QFrame::Box);
	setFrameShadow(QFrame::Raised);
}


CFontChooser::~CFontChooser() 
{
}


void CFontChooser::createFontAreaLayout() 
{
	QHBoxLayout* fontStyleSizeHBoxLayout = new QHBoxLayout();

	// font column
	QVBoxLayout* fontLayout = new QVBoxLayout();
	fontStyleSizeHBoxLayout->addLayout(fontLayout);

	QLabel* fontLabel = new QLabel(tr("Font name:"));
	fontLayout->addWidget(fontLabel);

	m_fontListWidget = new CListWidget();
	m_fontListWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
	fontLayout->addWidget(m_fontListWidget);

	// style column
	QVBoxLayout* styleLayout = new QVBoxLayout();
	fontStyleSizeHBoxLayout->addLayout(styleLayout);

	QLabel* styleLabel = new QLabel(tr("Font style:"));
	styleLayout->addWidget(styleLabel);

	m_styleListWidget = new CListWidget();
	m_styleListWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
	m_styleListWidget->setCharWidth(12);
	styleLayout->addWidget(m_styleListWidget);

	// size column
	QVBoxLayout* sizeLayout = new QVBoxLayout();
	fontStyleSizeHBoxLayout->addLayout(sizeLayout);

	QLabel* sizeLabel = new QLabel(tr("Size:"));
	sizeLayout->addWidget(sizeLabel);

	m_sizeListWidget = new CListWidget();
	m_sizeListWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
	m_sizeListWidget->setCharWidth(5);
	sizeLayout->addWidget(m_sizeListWidget);

	m_vBoxLayout->addLayout(fontStyleSizeHBoxLayout);
}


void CFontChooser::createLayout()
{
	m_vBoxLayout = new QVBoxLayout(this);
	createFontAreaLayout();
	createTextAreaLayout();
}


void CFontChooser::createTextAreaLayout() 
{
	QWidget* webViewWidget = new WebViewerWidget(this);
	QLayout* webViewLayout = new QVBoxLayout(webViewWidget);
	
	m_webView = new QWebView(webViewWidget);
	webViewLayout->addWidget(m_webView);
	m_vBoxLayout->addWidget(webViewWidget);
}


void CFontChooser::connectListWidgets() 
{

	bool ok = connect(
		m_fontListWidget,
		SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
		this,
		SLOT(fontChanged(QListWidgetItem *, QListWidgetItem *)));
	Q_ASSERT(ok);

	ok = connect(
		m_styleListWidget,
		SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
		this,
		SLOT(styleChanged(QListWidgetItem *, QListWidgetItem *)));
	Q_ASSERT(ok);

	ok = connect(
		m_sizeListWidget,
		SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
		this,
		SLOT(sizeChanged(QListWidgetItem *, QListWidgetItem *)));
	Q_ASSERT(ok);
}


void CFontChooser::fontChanged(QListWidgetItem* current, QListWidgetItem* /*previous*/) 
{
	if (current == 0)
		return;
	QString fontFamily = current->text();
	m_font.setFamily(fontFamily);
	loadStyles(fontFamily);
	outputHtmlText();
	emit fontSelected(m_font);
}


QString CFontChooser::formatAsHtml(const QString& text) 
{

	QString htmlText;
    htmlText.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    htmlText.append("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
    htmlText.append("<html xmlns=\"http://www.w3.org/1999/xhtml\">\n");
    htmlText.append("<head>\n");
    htmlText.append("    <style type=\"text/css\">\n");
    htmlText.append("        *[lang=en] { font-family:#FONT-FAMILY#; font-size:#FONT-SIZE#pt; font-weight:#FONT-WEIGHT#; font-style:#FONT-STYLE#; }\n");
    htmlText.append("    </style>\n");
    htmlText.append("</head>\n");
    htmlText.append("<body>\n");
    htmlText.append("    <div>\n");
    htmlText.append("        <div style=\"display: inline;\" lang=\"en\">\n");
	htmlText.append(text);
    htmlText.append("        </div>\n");
    htmlText.append("</body>\n");
    htmlText.append("</html>\n");
	return htmlText;
}


void CFontChooser::loadFonts() 
{
	m_fontListWidget->clear();
	QFontDatabase database;
	foreach (QString font, database.families()) {
		m_fontListWidget->addItem(font);
	}
	// This triggers loading the styles for the first font
	m_fontListWidget->setCurrentRow(0);
}


void CFontChooser::loadStyles(const QString& font) 
{
	m_styleListWidget->clear();
	QFontDatabase database;
	foreach (QString style, database.styles(font)) 
	{
		m_styleListWidget->addItem(style);
	// This triggers loading the sizes for the first style
	restoreListWidgetValue(m_styleListWidget,m_choosenStyle);
	}
}


void CFontChooser::loadSizes(const QString& font, const QString& style) 
{

	QString saveText = saveListWidgetValue(m_sizeListWidget);

	// Put new values into listWidget
	m_sizeListWidget->clear();
	QFontDatabase database;
	foreach (int size, database.pointSizes(font, style)) 
	{
		m_sizeListWidget->addItem(QString::number(size));
	}

	restoreListWidgetValue(m_sizeListWidget, saveText);
}


void CFontChooser::outputHtmlText() 
{
	QString text = formatAsHtml(m_htmlText);
	text.replace("#FONT-FAMILY#", m_font.family());
	text.replace("#FONT-SIZE#", QString::number(m_font.pointSize()));
	text.replace("#FONT-WEIGHT#", (m_font.bold() ? "bold" : "normal") );
	text.replace("#FONT-STYLE#", m_font.italic() ? "italic" : "normal");
	m_webView->setHtml(text);
}


void CFontChooser::restoreListWidgetValue(QListWidget* listWidget, const QString& value) 
{
	if (value == "") {
		listWidget->setCurrentRow(0);
		return;
	}

	for (int i=0; i< listWidget->count(); i++)
	{
		if (listWidget->item(i)->text() == value) 
		{
			listWidget->setCurrentRow(i);
			return;
		}
	}
	listWidget->setCurrentRow(0);
}


QString CFontChooser::saveListWidgetValue(QListWidget* listWidget) 
{
	QString saveText;
	int row = listWidget->currentRow();
	if (row >= 0) {
		saveText = listWidget->item(row)->text();
	}
	return saveText;
}


void CFontChooser::setFont(const QFont& font) 
{
	disconnect(m_fontListWidget, 0, 0, 0);
	disconnect(m_styleListWidget, 0, 0, 0);
	disconnect(m_sizeListWidget, 0, 0, 0);

	// set the font
	m_font = font;
	restoreListWidgetValue(m_fontListWidget, m_font.family());

	// set the style
	loadStyles( m_font.family());
	QFontDatabase database;
	QString styleString = database.styleString(m_font);
	m_choosenStyle = styleString;
	restoreListWidgetValue(m_styleListWidget, styleString);

	// set the size
	loadSizes(m_font.family(), styleString);
	restoreListWidgetValue(m_sizeListWidget, QString::number(m_font.pointSize()) );

	outputHtmlText();
	connectListWidgets();
}


void CFontChooser::setFontStyle(const QString& styleString, QFont* font) 
{
	if (styleString.contains("bold",Qt::CaseInsensitive))
		font->setBold(true);
	else 
		font->setBold(false);

	if (styleString.contains("italic",Qt::CaseInsensitive) || styleString.contains("oblique",Qt::CaseInsensitive) )
		font->setItalic(true);
	else 
		font->setItalic(false);
}


void CFontChooser::setSampleText(const QString& htmlText) 
{
	m_htmlText = htmlText;
	outputHtmlText();
}


void CFontChooser::sizeChanged(QListWidgetItem* current, QListWidgetItem* /*previous*/)
{
	if (current == 0)
		return;

	QString size = m_sizeListWidget->currentItem()->text();
	m_font.setPointSize(size.toInt());

	outputHtmlText();
	emit fontSelected(m_font);
}


QSize CFontChooser::sizeHint() const 
{
	return QSize(170,170);
}

void CFontChooser::styleChanged(QListWidgetItem* current, QListWidgetItem* /*previous*/)
{
	if (current == 0)
		return;

	QString styleString = current->text();
	setFontStyle(styleString, &m_font);

	// Save style if the user choose it
	bool focus = m_styleListWidget->hasFocus();
	if (focus)
		m_choosenStyle = styleString;

	QString font  = m_fontListWidget->currentItem()->text();
	loadSizes(font, styleString);

	outputHtmlText();
	emit fontSelected(m_font);
}


