/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "frontend/display/chtmlwritedisplay.h"

#include <QFontComboBox>
#include <QMenu>
#include <QTextEdit>
#include <QToolBar>
#include <QToolTip>
#include "backend/config/btconfig.h"
#include "frontend/bibletimeapp.h"
#include "frontend/display/btcolorwidget.h"
#include "frontend/display/btfontsizewidget.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/displaywindow/chtmlwritewindow.h"
#include "util/btassert.h"
#include "util/btconnect.h"
#include "util/cresmgr.h"


class BtActionCollection;

namespace {
const QString CHTMLWriteDisplayFontKey = "HtmlWriteDisplay/font";
const QString CHTMLWriteDisplayFontColorKey = "HtmlWriteDisplay/fontColor";
}

CHTMLWriteDisplay::CHTMLWriteDisplay(CHTMLWriteWindow * parentWindow, QWidget* parent)
    : CPlainWriteDisplay(parentWindow, parent)
    , m_handingFormatChangeFromEditor(false)
{

    BtConfig & conf = btConfig();
    setTextColor(conf.sessionValue(CHTMLWriteDisplayFontColorKey, textColor()));
    QFont f = conf.sessionValue(CHTMLWriteDisplayFontKey, currentFont());
    setCurrentFont(f);


    //--------------------bold toggle-------------------------
    m_actions.bold = new QAction(
        CResMgr::displaywindows::writeWindow::boldText::icon(),
        tr("Bold"),
        this);
    m_actions.bold->setCheckable(true);
    m_actions.bold->setChecked(f.bold());
    m_actions.bold->setShortcut(CResMgr::displaywindows::writeWindow::boldText::accel);
    m_actions.bold->setToolTip( tr("Bold") );
    BT_CONNECT(m_actions.bold, SIGNAL(toggled(bool)),
               this,           SLOT(toggleBold(bool)),
               Qt::DirectConnection);

    //--------------------italic toggle-------------------------
    m_actions.italic = new QAction(
        CResMgr::displaywindows::writeWindow::italicText::icon(),
        tr("Italic"),
        this );
    m_actions.italic->setCheckable(true);
    m_actions.italic->setChecked(f.italic());
    m_actions.bold->setShortcut(CResMgr::displaywindows::writeWindow::italicText::accel);
    BT_CONNECT(m_actions.italic, SIGNAL(toggled(bool)),
               this,             SLOT(toggleItalic(bool)),
               Qt::DirectConnection);
    m_actions.italic->setToolTip( tr("Italic") );

    //--------------------underline toggle-------------------------
    m_actions.underline = new QAction(
        CResMgr::displaywindows::writeWindow::underlinedText::icon(),
        tr("Underline"),
        this );
    m_actions.underline->setCheckable(true);
    m_actions.underline->setChecked(f.underline());
    m_actions.underline->setShortcut(CResMgr::displaywindows::writeWindow::underlinedText::accel);
    BT_CONNECT(m_actions.underline, SIGNAL(toggled(bool)),
               this,                SLOT(toggleUnderline(bool)),
               Qt::DirectConnection);
    m_actions.underline->setToolTip( tr("Underline") );

    //--------------------align left toggle-------------------------
    m_actions.alignLeft = new QAction(
        CResMgr::displaywindows::writeWindow::alignLeft::icon(),
        tr("Left"), this);
    m_actions.alignLeft->setCheckable(true);
    m_actions.alignLeft->setShortcut(CResMgr::displaywindows::writeWindow::alignLeft::accel);
    BT_CONNECT(m_actions.alignLeft, SIGNAL(toggled(bool)),
               this,                SLOT(alignLeft(bool)),
               Qt::DirectConnection);
    m_actions.alignLeft->setToolTip( tr("Align left") );

    //--------------------align center toggle-------------------------
    m_actions.alignCenter = new QAction(
        CResMgr::displaywindows::writeWindow::alignCenter::icon(),
        tr("Center"), this);
    m_actions.alignCenter->setCheckable(true);
    m_actions.alignCenter->setShortcut(CResMgr::displaywindows::writeWindow::alignCenter::accel);
    BT_CONNECT(m_actions.alignCenter, SIGNAL(toggled(bool)),
               this,                  SLOT(alignCenter(bool)),
               Qt::DirectConnection);
    m_actions.alignCenter->setToolTip( tr("Center") );

    //--------------------align right toggle-------------------------
    m_actions.alignRight = new QAction(
        CResMgr::displaywindows::writeWindow::alignRight::icon(),
        tr("Right"), this);
    m_actions.alignRight->setCheckable(true);
    m_actions.alignRight->setShortcut(CResMgr::displaywindows::writeWindow::alignRight::accel);
    BT_CONNECT(m_actions.alignRight, SIGNAL(toggled(bool)),
               this,                 SLOT(alignRight(bool)),
               Qt::DirectConnection);
    m_actions.alignRight->setToolTip( tr("Align right") );

    setAcceptRichText(true);
    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);

    BT_CONNECT(this, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
               this, SLOT(slotCurrentCharFormatChanged(QTextCharFormat)),
               Qt::DirectConnection);
}

void CHTMLWriteDisplay::setText(const QString & newText) {
    QTextEdit::setHtml(newText);
}

const QString CHTMLWriteDisplay::plainText() {
    return QTextEdit::toPlainText();
}

void CHTMLWriteDisplay::toggleBold(bool checked) {
    if (!m_handingFormatChangeFromEditor)
        setFontWeight(checked ? QFont::Bold : QFont::Normal);
}

void CHTMLWriteDisplay::toggleItalic(bool checked) {
    if (!m_handingFormatChangeFromEditor)
        setFontItalic(checked);
}

void CHTMLWriteDisplay::toggleUnderline(bool checked) {
    if (!m_handingFormatChangeFromEditor)
        setFontUnderline(checked);
}

void CHTMLWriteDisplay::alignLeft(bool set) {
    if (!m_handingFormatChangeFromEditor && set && (alignment() != Qt::AlignLeft)) {
        setAlignment(Qt::AlignLeft);
        alignmentChanged(Qt::AlignLeft);
    }
}

void CHTMLWriteDisplay::alignCenter(bool set) {
    if (!m_handingFormatChangeFromEditor && set && (alignment() != Qt::AlignHCenter)) {
        setAlignment(Qt::AlignHCenter);
        alignmentChanged(Qt::AlignHCenter);
    }
}

void CHTMLWriteDisplay::alignRight(bool set) {
    if (!m_handingFormatChangeFromEditor && set && (alignment() != Qt::AlignRight)) {
        setAlignment(Qt::AlignRight);
        alignmentChanged(Qt::AlignRight);
    }
}

/** The text's alignment changed. Enable the right buttons. */
void CHTMLWriteDisplay::alignmentChanged( int a ) {
    BT_ASSERT(!m_handingFormatChangeFromEditor);
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

void CHTMLWriteDisplay::slotCurrentCharFormatChanged(const QTextCharFormat &) {
    BT_ASSERT(!m_handingFormatChangeFromEditor);
    m_handingFormatChangeFromEditor = true;
    QFont f = currentFont();
    emit signalFontChanged(f);
    emit signalFontSizeChanged(f.pointSize());
    emit signalFontColorChanged(textColor());

    m_actions.bold->setChecked(f.bold());
    m_actions.italic->setChecked(f.italic());
    m_actions.underline->setChecked(f.underline());
    m_handingFormatChangeFromEditor = false;

    BtConfig & conf = btConfig();
    conf.setSessionValue(CHTMLWriteDisplayFontKey, currentFont());
    conf.setSessionValue(CHTMLWriteDisplayFontColorKey, textColor());
}

void CHTMLWriteDisplay::slotFontSizeChosen(int newSize) {
    if (!m_handingFormatChangeFromEditor)
        setFontPointSize(static_cast<qreal>(newSize));
}

void CHTMLWriteDisplay::slotFontFamilyChosen(const QFont& font) {
    if (!m_handingFormatChangeFromEditor)
        setFontFamily(font.family());
}

void CHTMLWriteDisplay::setupToolbar(QToolBar * bar, BtActionCollection * actions) {
    Q_UNUSED(actions);

    QFont f = currentFont();

    //--------------------font chooser-------------------------
    QFontComboBox* fontFamilyCombo = new QFontComboBox(this);
    fontFamilyCombo->setCurrentFont(f);
    fontFamilyCombo->setToolTip( tr("Font") );
    bar->addWidget(fontFamilyCombo);
    BT_CONNECT(fontFamilyCombo, SIGNAL(currentFontChanged(QFont const &)),
               this,            SLOT(slotFontFamilyChosen(QFont const &)),
               Qt::DirectConnection);
    BT_CONNECT(this,            SIGNAL(signalFontChanged(QFont const &)),
               fontFamilyCombo, SLOT(setCurrentFont(QFont const &)),
               Qt::DirectConnection);

    //--------------------font size chooser-------------------------
    BtFontSizeWidget* fontSizeChooser = new BtFontSizeWidget(this);
    fontSizeChooser->setFontSize(f.pointSize());
    fontSizeChooser->setToolTip( tr("Font size") );
    bar->addWidget(fontSizeChooser);
    BT_CONNECT(fontSizeChooser, SIGNAL(fontSizeChanged(int)),
               this,            SLOT(slotFontSizeChosen(int)),
               Qt::DirectConnection);
    BT_CONNECT(this,            SIGNAL(signalFontSizeChanged(int)),
               fontSizeChooser, SLOT(setFontSize(int)), Qt::DirectConnection);

    //--------------------color button-------------------------
    BtColorWidget* fontColorChooser = new BtColorWidget();
    fontColorChooser->setColor(textColor());
    fontColorChooser->setToolTip(tr("Font color"));
    bar->addWidget(fontColorChooser);
    BT_CONNECT(fontColorChooser, SIGNAL(changed(QColor const &)),
               this,             SLOT(setTextColor(QColor const &)),
               Qt::DirectConnection);
    BT_CONNECT(this,             SIGNAL(signalFontColorChanged(QColor const &)),
               fontColorChooser, SLOT(setColor(QColor)), Qt::DirectConnection);

    bar->addSeparator();

    bar->addAction(m_actions.bold);
    bar->addAction(m_actions.italic);
    bar->addAction(m_actions.underline);

    //seperate formatting from alignment buttons
    bar->addSeparator();

    bar->addAction(m_actions.alignLeft);
    bar->addAction(m_actions.alignCenter);
    bar->addAction(m_actions.alignRight);
}
