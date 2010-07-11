/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
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
#include "util/directory.h"


class BtActionCollection;

CHTMLWriteDisplay::CHTMLWriteDisplay(CWriteWindow* parentWindow, QWidget* parent)
: CPlainWriteDisplay(parentWindow, parent) {
    m_actions.bold = 0;
    m_actions.italic = 0;
    m_actions.underline = 0;
    m_actions.selectAll = 0;

    //--------------------bold toggle-------------------------
    namespace DU = util::directory;
    m_actions.bold = new QAction(
        DU::getIcon(CResMgr::displaywindows::writeWindow::boldText::icon),
        tr("Bold"),
        this);
    m_actions.bold->setCheckable(true);
    m_actions.bold->setShortcut(CResMgr::displaywindows::writeWindow::boldText::accel);
    m_actions.bold->setToolTip( tr("Bold") );
    connect(m_actions.bold, SIGNAL(toggled(bool)), this, SLOT(toggleBold(bool)));

    //--------------------italic toggle-------------------------
    m_actions.italic = new QAction(
        DU::getIcon(CResMgr::displaywindows::writeWindow::italicText::icon),
        tr("Italic"),
        this );
    m_actions.italic->setCheckable(true);
    m_actions.bold->setShortcut(CResMgr::displaywindows::writeWindow::italicText::accel);
    connect(m_actions.italic, SIGNAL(toggled(bool)), this, SLOT(toggleItalic(bool)));
    m_actions.italic->setToolTip( tr("Italic") );

    //--------------------underline toggle-------------------------
    m_actions.underline = new QAction(
        DU::getIcon(CResMgr::displaywindows::writeWindow::underlinedText::icon),
        tr("Underline"),
        this );
    m_actions.underline->setCheckable(true);
    m_actions.underline->setShortcut(CResMgr::displaywindows::writeWindow::underlinedText::accel);
    connect(m_actions.underline, SIGNAL(toggled(bool)), this, SLOT(toggleUnderline(bool)));
    m_actions.underline->setToolTip( tr("Underline") );

    //--------------------align left toggle-------------------------
    m_actions.alignLeft = new QAction(
        DU::getIcon(CResMgr::displaywindows::writeWindow::alignLeft::icon),
        tr("Left"), this);
    m_actions.alignLeft->setCheckable(true);
    m_actions.alignLeft->setShortcut(CResMgr::displaywindows::writeWindow::alignLeft::accel);
    connect(m_actions.alignLeft, SIGNAL(toggled(bool)), this, SLOT(alignLeft(bool)));
    m_actions.alignLeft->setToolTip( tr("Align left") );

    //--------------------align center toggle-------------------------
    m_actions.alignCenter = new QAction(
        DU::getIcon(CResMgr::displaywindows::writeWindow::alignCenter::icon),
        tr("Center"), this);
    m_actions.alignCenter->setCheckable(true);
    m_actions.alignCenter->setShortcut(CResMgr::displaywindows::writeWindow::alignCenter::accel);
    connect(m_actions.alignCenter, SIGNAL(toggled(bool)), this, SLOT(alignCenter(bool)));
    m_actions.alignCenter->setToolTip( tr("Center") );

    //--------------------align right toggle-------------------------
    m_actions.alignRight = new QAction(
        DU::getIcon(CResMgr::displaywindows::writeWindow::alignRight::icon),
        tr("Right"), this);
    m_actions.alignRight->setCheckable(true);
    m_actions.alignRight->setShortcut(CResMgr::displaywindows::writeWindow::alignRight::accel);
    connect(m_actions.alignRight, SIGNAL(toggled(bool)), this, SLOT(alignRight(bool)));
    m_actions.alignRight->setToolTip( tr("Align right") );

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
    emit setColor(c);
}

void CHTMLWriteDisplay::slotFontChanged( const QFont& font ) {
    emit fontChanged(font);
    emit fontSizeChanged(font.pointSize());

    m_actions.bold->setChecked( font.bold() );
    m_actions.italic->setChecked( font.italic() );
    m_actions.underline->setChecked( font.underline() );
}

void CHTMLWriteDisplay::slotFontFamilyChoosen(const QFont& font) {
    setFontFamily(font.family());
}

void CHTMLWriteDisplay::setupToolbar(QToolBar * bar, BtActionCollection * actions) {

    //--------------------font chooser-------------------------
    QFontComboBox* fontFamilyCombo = new QFontComboBox(this);
    fontFamilyCombo->setToolTip( tr("Font") );
    bar->addWidget(fontFamilyCombo);
    bool ok = connect(fontFamilyCombo, SIGNAL(currentFontChanged(const QFont&)),
                      this, SLOT(slotFontFamilyChoosen(const QFont&)));
    Q_ASSERT(ok);
    ok = connect(this, SIGNAL(fontChanged(const QFont&)), fontFamilyCombo, SLOT(setCurrentFont(const QFont&)));
    Q_ASSERT(ok);

    //--------------------font size chooser-------------------------
    BtFontSizeWidget* fontSizeChooser = new BtFontSizeWidget(this);
    fontSizeChooser->setToolTip( tr("Font size") );
    bar->addWidget(fontSizeChooser);
    ok = connect(fontSizeChooser, SIGNAL(fontSizeChanged(int)), this, SLOT(changeFontSize(int)));
    Q_ASSERT(ok);
    ok = connect(this, SIGNAL(fontSizeChanged(int)), fontSizeChooser, SLOT(setFontSize(int)));
    Q_ASSERT(ok);

    //--------------------color button-------------------------
    BtColorWidget* colorChooser = new BtColorWidget();
    colorChooser->setToolTip(tr("Font color"));
    bar->addWidget(colorChooser);
    ok = connect(colorChooser, SIGNAL(changed(const QColor&)), this, SLOT(slotColorSelected(const QColor&)));
    Q_ASSERT(ok);
    ok = connect(this, SIGNAL(setColor(const QColor&)), colorChooser, SLOT(setColor(const QColor&)));
    Q_ASSERT(ok);

    bar->addSeparator();

    bar->addAction(m_actions.bold);
    bar->addAction(m_actions.italic);
    bar->addAction(m_actions.underline);

    //seperate formatting from alignment buttons
    bar->addSeparator();

    bar->addAction(m_actions.alignLeft);
    bar->addAction(m_actions.alignCenter);
    bar->addAction(m_actions.alignRight);

    connect(this, SIGNAL(currentFontChanged(const QFont&)), SLOT(slotFontChanged(const QFont&)));
    connect(this, SIGNAL(currentAlignmentChanged(int)), SLOT(slotAlignmentChanged(int)));
    connect(this, SIGNAL(currentColorChanged(const QColor&)), SLOT(slotColorChanged(const QColor&)));

    //set initial values for toolbar items
    slotFontChanged( font() );
    slotAlignmentChanged( alignment() );
    slotColorChanged( textColor() );
}
