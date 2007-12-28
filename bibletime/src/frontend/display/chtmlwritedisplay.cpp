/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "chtmlwritedisplay.h"
#include "chtmlwritedisplay.moc"

#include "frontend/displaywindow/cwritewindow.h"

#include "util/cresmgr.h"

//Qt includes
#include <QMenu>
#include <QToolTip>
#include <QTextEdit>
#include <QFontComboBox>

//KDE includes
#include <kaction.h>
#include <kactioncollection.h>
#include <ktoggleaction.h>
#include <kfontaction.h>
#include <kfontsizeaction.h>
#include <ktoolbar.h>

#include <kcolorbutton.h>
#include <kmenu.h>

CHTMLWriteDisplay::CHTMLWriteDisplay(CWriteWindow* parentWindow, QWidget* parent)
: CPlainWriteDisplay(parentWindow,parent) {
	m_actions.bold = 0;
	m_actions.italic = 0;
	m_actions.underline = 0;
	m_actions.selectAll = 0;

	setAcceptRichText(true);
	setAcceptDrops(true);
	viewport()->setAcceptDrops(true);
}

CHTMLWriteDisplay::~CHTMLWriteDisplay() {}

void CHTMLWriteDisplay::setText( const QString& newText ) {
	QString text = newText;
// 	text.replace("\n<br/><!-- BT newline -->\n", "\n");

	QTextEdit::setHtml(text);
}

const QString CHTMLWriteDisplay::plainText() {
	return QTextEdit::toPlainText();
};

void CHTMLWriteDisplay::toggleBold(bool) {
	setFontWeight( m_actions.bold->isChecked() ? QFont::Bold : QFont::Normal );
};

void CHTMLWriteDisplay::toggleItalic(bool) {
	setFontItalic( m_actions.italic->isChecked() );
};

void CHTMLWriteDisplay::toggleUnderline(bool) {
	setFontUnderline( m_actions.underline->isChecked() );
};


void CHTMLWriteDisplay::alignLeft(bool set) {
	if (set && (alignment() != Qt::AlignLeft)){
		setAlignment(Qt::AlignLeft);
		slotAlignmentChanged(Qt::AlignLeft);
	}
};

void CHTMLWriteDisplay::alignCenter(bool set) {
	if (set && (alignment() != Qt::AlignHCenter)){
		setAlignment(Qt::AlignHCenter);
		slotAlignmentChanged(Qt::AlignHCenter);
	}
};

void CHTMLWriteDisplay::alignRight(bool set) {
	if (set && (alignment() != Qt::AlignRight)){
		setAlignment(Qt::AlignRight);
		slotAlignmentChanged(Qt::AlignRight);
	}
};


/** The text's alignment changed. Enable the right buttons. */
void CHTMLWriteDisplay::slotAlignmentChanged( int a ) {
	bool alignLeft = false;
	bool alignCenter = false;
	bool alignRight = false;
	//  bool alignJustify = false;

	if (a & Qt::AlignLeft) {
		alignLeft = true;
	}
	else if ((a & Qt::AlignHCenter) || (a & Qt::AlignCenter)) {
		alignCenter = true;
	}
	else if (a & Qt::AlignRight) {
		alignRight = true;
	}
	//  else if (a & Qt::AlignJustify) {
	//    alignJustify = true;
	//  }
	else {
		alignLeft = true;
		qWarning("unknown alignment %i", a);
	}

	m_actions.alignLeft->setChecked( alignLeft );
	m_actions.alignCenter->setChecked( alignCenter );
	m_actions.alignRight->setChecked( alignRight );
	//  m_actions.alignJustify->setChecked( alignJustify );
}

void CHTMLWriteDisplay::changeFontSize(int newSize) {
	setFontPointSize((qreal)newSize);
}

/** Is called when a new color was selected. */
void CHTMLWriteDisplay::slotColorSelected( const QColor& c) {
	setTextColor( c );
}

/** Is called when a text with another color was selected. */
void CHTMLWriteDisplay::slotColorChanged(const QColor& c) {
	m_colorButton->setColor(c);
}


void CHTMLWriteDisplay::slotFontChanged( const QFont& font ) {
	m_actions.fontChooser->setFont( font.family() );
	m_actions.fontSizeChooser->setFontSize( font.pointSize() );

	m_actions.bold->setChecked( font.bold() );
	m_actions.italic->setChecked( font.italic() );
	m_actions.underline->setChecked( font.underline() );
};

void CHTMLWriteDisplay::setupToolbar(QToolBar * bar, KActionCollection * actions) {
	
	//--------------------font chooser-------------------------
	//TODO: clean the comments after the port works
	m_actions.fontChooser = new KFontAction( tr("Choose a font"),
							actions // I suppose this is the parent
							//CResMgr::displaywindows::writeWindow::underlinedText::accel, //shortcut: setShortcut
							//actions, //KActionCollection::addAction(), see also previous "parent" and KDE4 porting doc
							//CResMgr::displaywindows::writeWindow::fontFamily::actionName //see previous
							);
	m_actions.fontChooser->setShortcut(CResMgr::displaywindows::writeWindow::underlinedText::accel);
	actions->addAction(CResMgr::displaywindows::writeWindow::fontFamily::actionName, m_actions.fontChooser);
	m_actions.fontChooser->setToolTip( CResMgr::displaywindows::writeWindow::fontFamily::tooltip );
	//m_actions.fontChooser->plug(bar); // -> bar->addAction
	bar->addAction(m_actions.fontChooser);
	connect(m_actions.fontChooser, SIGNAL(triggered(const QString&)), this, SLOT(setFontFamily(const QString&)));

	//--------------------font size chooser-------------------------
	m_actions.fontSizeChooser = new KFontSizeAction( tr("Choose a font size"), actions);
	m_actions.fontSizeChooser->setShortcut(CResMgr::displaywindows::writeWindow::fontSize::accel);
	actions->addAction(CResMgr::displaywindows::writeWindow::fontSize::actionName, m_actions.fontSizeChooser);
	m_actions.fontSizeChooser->setToolTip( CResMgr::displaywindows::writeWindow::fontSize::tooltip );
	bar->addAction(m_actions.fontSizeChooser);
	connect(m_actions.fontSizeChooser, SIGNAL(fontSizeChanged(int)), this, SLOT(changeFontSize(int)));

	//--------------------color button-------------------------
	m_colorButton = new KColorButton(bar);
	connect(m_colorButton, SIGNAL(changed(const QColor&)), this, SLOT(slotColorSelected(const QColor&)));
	
	//TODO: this has changed quite much,does it work?
	//bar->insertWidget(50, m_colorButton->sizeHint().width(), m_colorButton);
	bar->addWidget(m_colorButton);
	m_colorButton->setToolTip(CResMgr::displaywindows::writeWindow::fontColor::tooltip);

	//(new KActionSeparator())->plug(bar); //seperate font options from formatting buttons
	bar->addSeparator();

	//--------------------bold toggle-------------------------
	m_actions.bold = new KToggleAction(KIcon(CResMgr::displaywindows::writeWindow::boldText::icon),
						 tr("Bold"), actions);
	m_actions.bold->setShortcut(CResMgr::displaywindows::writeWindow::boldText::accel);
	actions->addAction(CResMgr::displaywindows::writeWindow::boldText::actionName, m_actions.bold);
	m_actions.bold->setToolTip( CResMgr::displaywindows::writeWindow::boldText::tooltip );
	connect(m_actions.bold, SIGNAL(toggled(bool)), this, SLOT(toggleBold(bool)));

	//m_actions.bold->plug(bar);
	bar->addAction(m_actions.bold);

	//--------------------italic toggle-------------------------
	m_actions.italic = new KToggleAction(KIcon(CResMgr::displaywindows::writeWindow::italicText::icon),
							tr("Italic"), actions );
	m_actions.bold->setShortcut(CResMgr::displaywindows::writeWindow::italicText::accel);
	actions->addAction(CResMgr::displaywindows::writeWindow::italicText::actionName, m_actions.italic);
	connect(m_actions.italic, SIGNAL(toggled(bool)), this, SLOT(toggleItalic(bool)));
	m_actions.italic->setToolTip( CResMgr::displaywindows::writeWindow::italicText::tooltip );
	bar->addAction(m_actions.italic);

	//--------------------underline toggle-------------------------
	m_actions.underline = new KToggleAction( KIcon(CResMgr::displaywindows::writeWindow::underlinedText::icon),
								tr("Underline"), actions );
	m_actions.underline->setShortcut(CResMgr::displaywindows::writeWindow::underlinedText::accel);
	actions->addAction(CResMgr::displaywindows::writeWindow::underlinedText::actionName, m_actions.underline);
	connect(m_actions.underline, SIGNAL(toggled(bool)), this, SLOT(toggleUnderline(bool)));
	m_actions.underline->setToolTip( CResMgr::displaywindows::writeWindow::underlinedText::tooltip );
	bar->addAction(m_actions.underline);

	//seperate formatting from alignment buttons
	bar->addSeparator();

	//--------------------align left toggle-------------------------
	m_actions.alignLeft = new KToggleAction( KIcon(CResMgr::displaywindows::writeWindow::alignLeft::icon),
								tr("Left"), actions);
	m_actions.alignLeft->setShortcut(CResMgr::displaywindows::writeWindow::alignLeft::accel);
	actions->addAction(CResMgr::displaywindows::writeWindow::alignLeft::actionName, m_actions.alignLeft);
	connect(m_actions.alignLeft, SIGNAL(toggled(bool)), this, SLOT(alignLeft(bool)));
	m_actions.alignLeft->setToolTip( CResMgr::displaywindows::writeWindow::alignLeft::tooltip );
	bar->addAction(m_actions.alignLeft);

	//--------------------align center toggle-------------------------
	m_actions.alignCenter = new KToggleAction( KIcon(CResMgr::displaywindows::writeWindow::alignCenter::icon),
								tr("Center"), actions);
	m_actions.alignCenter->setShortcut(CResMgr::displaywindows::writeWindow::alignCenter::accel);
	actions->addAction(CResMgr::displaywindows::writeWindow::alignCenter::actionName, m_actions.alignCenter);
	connect(m_actions.alignCenter, SIGNAL(toggled(bool)), this, SLOT(alignCenter(bool)));
	m_actions.alignCenter->setToolTip( CResMgr::displaywindows::writeWindow::alignCenter::tooltip );
	bar->addAction(m_actions.alignCenter);

	//--------------------align right toggle-------------------------
	m_actions.alignRight = new KToggleAction( KIcon(CResMgr::displaywindows::writeWindow::alignRight::icon),
								tr("Right"), actions);
	m_actions.alignRight->setShortcut(CResMgr::displaywindows::writeWindow::alignRight::accel);
	actions->addAction(CResMgr::displaywindows::writeWindow::alignRight::actionName, m_actions.alignRight);
	connect(m_actions.alignRight, SIGNAL(toggled(bool)), this, SLOT(alignRight(bool)));
	m_actions.alignRight->setToolTip( CResMgr::displaywindows::writeWindow::alignRight::tooltip );
	bar->addAction(m_actions.alignRight);


	connect(this, SIGNAL(currentFontChanged(const QFont&)), SLOT(slotFontChanged(const QFont&)));
	connect(this, SIGNAL(currentAlignmentChanged(int)), SLOT(slotAlignmentChanged(int)));
	connect(this, SIGNAL(currentColorChanged(const QColor&)), SLOT(slotColorChanged(const QColor&)));


	//set initial values for toolbar items
	slotFontChanged( font() );
	slotAlignmentChanged( alignment() );
	slotColorChanged( textColor() );

}

/** Reimplementation to show a popup menu if the right mouse button was clicked. */
QMenu* CHTMLWriteDisplay::createPopupMenu( const QPoint& ) {
	if (!m_actions.selectAll) {
		m_actions.selectAll = new KAction(tr("Select all"), this);
		connect(m_actions.selectAll, SIGNAL(triggered(bool)), SLOT(selectAll()));
	}

	KMenu* popup = new KMenu(this);
	popup->addTitle(tr("HTML editor window"));
	popup->addAction(m_actions.selectAll);
	return popup;
};
