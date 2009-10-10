/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/display/chtmlwritedisplay.h"

#include <QFontComboBox>
#include <QMenu>
#include <QTextEdit>
#include <QToolBar>
#include <QToolTip>
#include "frontend/display/btcolorwidget.h"
#include "frontend/display/btfontsizewidget.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/displaywindow/cwritewindow.h"
#include "util/cresmgr.h"
#include "util/directoryutil.h"


class BtActionCollection;

CHTMLWriteDisplay::CHTMLWriteDisplay(CWriteWindow* parentWindow, QWidget* parent)
        : CPlainWriteDisplay(parentWindow, parent), m_fontFamilyChooser(0),
        m_fontSizeChooser(0), m_colorChooser(0) {
    m_actions.bold = 0;
    m_actions.italic = 0;
    m_actions.underline = 0;
    m_actions.selectAll = 0;

    setAcceptRichText(true);
    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);
}

CHTMLWriteDisplay::~CHTMLWriteDisplay() {
}

void CHTMLWriteDisplay::setText( const QString& newText ) {
    QTextEdit::setHtml(newText);
}

const QString CHTMLWriteDisplay::plainText() {
    return QTextEdit::toPlainText();
}

void CHTMLWriteDisplay::toggleBold(bool) {
    setFontWeight( m_actions.bold->isChecked() ? QFont::Bold : QFont::Normal );
}

void CHTMLWriteDisplay::toggleItalic(bool) {
    setFontItalic( m_actions.italic->isChecked() );
}

void CHTMLWriteDisplay::toggleUnderline(bool) {
    setFontUnderline( m_actions.underline->isChecked() );
}


void CHTMLWriteDisplay::alignLeft(bool set) {
    if (set && (alignment() != Qt::AlignLeft)) {
        setAlignment(Qt::AlignLeft);
        slotAlignmentChanged(Qt::AlignLeft);
    }
}

void CHTMLWriteDisplay::alignCenter(bool set) {
    if (set && (alignment() != Qt::AlignHCenter)) {
        setAlignment(Qt::AlignHCenter);
        slotAlignmentChanged(Qt::AlignHCenter);
    }
}

void CHTMLWriteDisplay::alignRight(bool set) {
    if (set && (alignment() != Qt::AlignRight)) {
        setAlignment(Qt::AlignRight);
        slotAlignmentChanged(Qt::AlignRight);
    }
}

/** The text's alignment changed. Enable the right buttons. */
void CHTMLWriteDisplay::slotAlignmentChanged( int a ) {
    bool alignLeft = false;
    bool alignCenter = false;
    bool alignRight = false;

    if (a & Qt::AlignLeft) {
        alignLeft = true;
    }
    else if ((a & Qt::AlignHCenter) || (a & Qt::AlignCenter)) {
        alignCenter = true;
    }
    else if (a & Qt::AlignRight) {
        alignRight = true;
    }
    else {
        alignLeft = true;
        qWarning("unknown alignment %i", a);
    }

    m_actions.alignLeft->setChecked( alignLeft );
    m_actions.alignCenter->setChecked( alignCenter );
    m_actions.alignRight->setChecked( alignRight );
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
    m_colorChooser->setColor(c);
}

void CHTMLWriteDisplay::slotFontChanged( const QFont& font ) {
    m_fontFamilyChooser->setCurrentFont(font);
    m_fontSizeChooser->setFontSize( font.pointSize() );

    m_actions.bold->setChecked( font.bold() );
    m_actions.italic->setChecked( font.italic() );
    m_actions.underline->setChecked( font.underline() );
}

void CHTMLWriteDisplay::slotFontFamilyChoosen(const QFont& font) {
    setFontFamily(font.family());
}

void CHTMLWriteDisplay::setupToolbar(QToolBar * bar, BtActionCollection * actions) {
    namespace DU = util::directoryutil;

    //--------------------font chooser-------------------------
    m_fontFamilyChooser = new QFontComboBox(this);
    actions->addAction(CResMgr::displaywindows::writeWindow::fontFamily::actionName, m_fontFamilyChooser);
    m_fontFamilyChooser->setToolTip( tr("Font") );
    bar->addWidget(m_fontFamilyChooser);
    bool ok = connect(m_fontFamilyChooser, SIGNAL(currentFontChanged(const QFont&)),
                      this, SLOT(slotFontFamilyChoosen(const QFont&)));
    Q_ASSERT(ok);

    //--------------------font size chooser-------------------------
    m_fontSizeChooser = new BtFontSizeWidget(this);
    m_fontSizeChooser->setToolTip( tr("Font size") );
    bar->addWidget(m_fontSizeChooser);
    ok = connect(m_fontSizeChooser, SIGNAL(fontSizeChanged(int)), this, SLOT(changeFontSize(int)));
    Q_ASSERT(ok);

    //--------------------color button-------------------------
    m_colorChooser = new BtColorWidget();
    m_colorChooser->setToolTip(tr("Font color"));
    bar->addWidget(m_colorChooser);
    ok = connect(m_colorChooser, SIGNAL(changed(const QColor&)), this, SLOT(slotColorSelected(const QColor&)));
    Q_ASSERT(ok);

    bar->addSeparator();

    //--------------------bold toggle-------------------------
    m_actions.bold = new QAction(
        DU::getIcon(CResMgr::displaywindows::writeWindow::boldText::icon),
        tr("Bold"),
        actions);
    m_actions.bold->setCheckable(true);
    m_actions.bold->setShortcut(CResMgr::displaywindows::writeWindow::boldText::accel);
    actions->addAction(CResMgr::displaywindows::writeWindow::boldText::actionName, m_actions.bold);
    m_actions.bold->setToolTip( tr("Bold") );
    connect(m_actions.bold, SIGNAL(toggled(bool)), this, SLOT(toggleBold(bool)));

    bar->addAction(m_actions.bold);

    //--------------------italic toggle-------------------------
    m_actions.italic = new QAction(
        DU::getIcon(CResMgr::displaywindows::writeWindow::italicText::icon),
        tr("Italic"),
        actions );
    m_actions.italic->setCheckable(true);
    m_actions.bold->setShortcut(CResMgr::displaywindows::writeWindow::italicText::accel);
    actions->addAction(CResMgr::displaywindows::writeWindow::italicText::actionName, m_actions.italic);
    connect(m_actions.italic, SIGNAL(toggled(bool)), this, SLOT(toggleItalic(bool)));
    m_actions.italic->setToolTip( tr("Italic") );
    bar->addAction(m_actions.italic);

    //--------------------underline toggle-------------------------
    m_actions.underline = new QAction(
        DU::getIcon(CResMgr::displaywindows::writeWindow::underlinedText::icon),
        tr("Underline"),
        actions );
    m_actions.underline->setCheckable(true);
    m_actions.underline->setShortcut(CResMgr::displaywindows::writeWindow::underlinedText::accel);
    actions->addAction(CResMgr::displaywindows::writeWindow::underlinedText::actionName, m_actions.underline);
    connect(m_actions.underline, SIGNAL(toggled(bool)), this, SLOT(toggleUnderline(bool)));
    m_actions.underline->setToolTip( tr("Underline") );
    bar->addAction(m_actions.underline);

    //seperate formatting from alignment buttons
    bar->addSeparator();

    //--------------------align left toggle-------------------------
    m_actions.alignLeft = new QAction(
        DU::getIcon(CResMgr::displaywindows::writeWindow::alignLeft::icon),
        tr("Left"), actions);
    m_actions.alignLeft->setCheckable(true);
    m_actions.alignLeft->setShortcut(CResMgr::displaywindows::writeWindow::alignLeft::accel);
    actions->addAction(CResMgr::displaywindows::writeWindow::alignLeft::actionName, m_actions.alignLeft);
    connect(m_actions.alignLeft, SIGNAL(toggled(bool)), this, SLOT(alignLeft(bool)));
    m_actions.alignLeft->setToolTip( tr("Align left") );
    bar->addAction(m_actions.alignLeft);

    //--------------------align center toggle-------------------------
    m_actions.alignCenter = new QAction(
        DU::getIcon(CResMgr::displaywindows::writeWindow::alignCenter::icon),
        tr("Center"), actions);
    m_actions.alignCenter->setCheckable(true);
    m_actions.alignCenter->setShortcut(CResMgr::displaywindows::writeWindow::alignCenter::accel);
    actions->addAction(CResMgr::displaywindows::writeWindow::alignCenter::actionName, m_actions.alignCenter);
    connect(m_actions.alignCenter, SIGNAL(toggled(bool)), this, SLOT(alignCenter(bool)));
    m_actions.alignCenter->setToolTip( tr("Center") );
    bar->addAction(m_actions.alignCenter);

    //--------------------align right toggle-------------------------
    m_actions.alignRight = new QAction(
        DU::getIcon(CResMgr::displaywindows::writeWindow::alignRight::icon),
        tr("Right"), actions);
    m_actions.alignRight->setCheckable(true);
    m_actions.alignRight->setShortcut(CResMgr::displaywindows::writeWindow::alignRight::accel);
    actions->addAction(CResMgr::displaywindows::writeWindow::alignRight::actionName, m_actions.alignRight);
    connect(m_actions.alignRight, SIGNAL(toggled(bool)), this, SLOT(alignRight(bool)));
    m_actions.alignRight->setToolTip( tr("Align right") );
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
        m_actions.selectAll = new QAction(tr("Select all"), this);
        connect(m_actions.selectAll, SIGNAL(triggered(bool)), SLOT(selectAll()));
    }

    QMenu* popup = new QMenu(this);
    popup->setTitle(tr("HTML editor window"));
    popup->addAction(m_actions.selectAll);
    return popup;
}
