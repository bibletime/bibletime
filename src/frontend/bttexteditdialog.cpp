/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bttexteditdialog.h"

#include <QAction>
#include <QDialogButtonBox>
#include <QFontComboBox>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QToolBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "backend/config/btconfig.h"
#include "frontend/display/btcolorwidget.h"
#include "frontend/display/btfontsizewidget.h"
#include "util/btconnect.h"
#include "util/cresmgr.h"

namespace {
const QString CHTMLWriteDisplayFontKey = "HtmlWriteDisplay/font";
const QString CHTMLWriteDisplayFontColorKey = "HtmlWriteDisplay/fontColor";
}

BtTextEditDialog::BtTextEditDialog(QWidget* parent)
        : QDialog(parent),
        m_buttons(new QDialogButtonBox(this)),
        m_textEdit(new QTextEdit(this)),
        m_toolBar(new QToolBar(this)),
        m_handingFormatChangeFromEditor(false) {

    m_buttons->addButton(QDialogButtonBox::Save);
    m_buttons->addButton(QDialogButtonBox::Cancel);
    BT_CONNECT(m_buttons, SIGNAL(accepted()), this, SLOT(accept()));
    BT_CONNECT(m_buttons, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout * vLayout = new QVBoxLayout(this);
    vLayout->addWidget(m_toolBar);
    vLayout->addWidget(m_textEdit);
    vLayout->addWidget(m_buttons);

    setMinimumWidth(400);

    initActions();
    setupToolBar();

    m_textEdit->setAcceptRichText(true);
    m_textEdit->setAcceptDrops(true);

    BT_CONNECT(m_textEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
               this, SLOT(slotCurrentCharFormatChanged(QTextCharFormat)),
               Qt::DirectConnection);

}

void BtTextEditDialog::setTitle(const QString& text) {
    setWindowTitle(text);
}

void BtTextEditDialog::initActions() {

    BtConfig & conf = btConfig();
    m_textEdit->setTextColor(conf.sessionValue(CHTMLWriteDisplayFontColorKey, m_textEdit->textColor()));
    QFont f = conf.sessionValue(CHTMLWriteDisplayFontKey, m_textEdit->currentFont());
    m_textEdit->setCurrentFont(f);

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

}

void BtTextEditDialog::toggleBold(bool checked) {
    if (!m_handingFormatChangeFromEditor)
        m_textEdit->setFontWeight(checked ? QFont::Bold : QFont::Normal);
}

void BtTextEditDialog::toggleItalic(bool checked) {
    if (!m_handingFormatChangeFromEditor)
        m_textEdit->setFontItalic(checked);
}

void BtTextEditDialog::toggleUnderline(bool checked) {
    if (!m_handingFormatChangeFromEditor)
        m_textEdit->setFontUnderline(checked);
}

void BtTextEditDialog::alignLeft(bool set) {
    if (!m_handingFormatChangeFromEditor && set && (m_textEdit->alignment() != Qt::AlignLeft)) {
        m_textEdit->setAlignment(Qt::AlignLeft);
        alignmentChanged(Qt::AlignLeft);
    }
}

void BtTextEditDialog::alignCenter(bool set) {
    if (!m_handingFormatChangeFromEditor && set && (m_textEdit->alignment() != Qt::AlignHCenter)) {
        m_textEdit->setAlignment(Qt::AlignHCenter);
        alignmentChanged(Qt::AlignHCenter);
    }
}

void BtTextEditDialog::alignRight(bool set) {
    if (!m_handingFormatChangeFromEditor && set && (m_textEdit->alignment() != Qt::AlignRight)) {
        m_textEdit->setAlignment(Qt::AlignRight);
        alignmentChanged(Qt::AlignRight);
    }
}

/** The text's alignment changed. Enable the right buttons. */
void BtTextEditDialog::alignmentChanged( int a ) {
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


void BtTextEditDialog::setupToolBar() {

    QFont f = m_textEdit->currentFont();

    //--------------------font chooser-------------------------
    QFontComboBox* fontFamilyCombo = new QFontComboBox(this);
    fontFamilyCombo->setCurrentFont(f);
    fontFamilyCombo->setToolTip( tr("Font") );
    m_toolBar->addWidget(fontFamilyCombo);
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
    m_toolBar->addWidget(fontSizeChooser);
    BT_CONNECT(fontSizeChooser, SIGNAL(fontSizeChanged(int)),
               this,            SLOT(slotFontSizeChosen(int)),
               Qt::DirectConnection);
    BT_CONNECT(this,            SIGNAL(signalFontSizeChanged(int)),
               fontSizeChooser, SLOT(setFontSize(int)), Qt::DirectConnection);

    //--------------------color button-------------------------
    BtColorWidget* fontColorChooser = new BtColorWidget();
    fontColorChooser->setColor(m_textEdit->textColor());
    fontColorChooser->setToolTip(tr("Font color"));
    m_toolBar->addWidget(fontColorChooser);
    BT_CONNECT(fontColorChooser, SIGNAL(changed(QColor const &)),
               m_textEdit,       SLOT(setTextColor(QColor const &)),
               Qt::DirectConnection);
    BT_CONNECT(this,             SIGNAL(signalFontColorChanged(QColor const &)),
               fontColorChooser, SLOT(setColor(QColor)), Qt::DirectConnection);

    m_toolBar->addSeparator();

    m_toolBar->addAction(m_actions.bold);
    m_toolBar->addAction(m_actions.italic);
    m_toolBar->addAction(m_actions.underline);

    //seperate formatting from alignment buttons
    m_toolBar->addSeparator();

    m_toolBar->addAction(m_actions.alignLeft);
    m_toolBar->addAction(m_actions.alignCenter);
    m_toolBar->addAction(m_actions.alignRight);
}

void BtTextEditDialog::slotCurrentCharFormatChanged(const QTextCharFormat &) {
    BT_ASSERT(!m_handingFormatChangeFromEditor);
    m_handingFormatChangeFromEditor = true;
    QFont f = m_textEdit->currentFont();
    emit signalFontChanged(f);
    emit signalFontSizeChanged(f.pointSize());
    emit signalFontColorChanged(m_textEdit->textColor());

    m_actions.bold->setChecked(f.bold());
    m_actions.italic->setChecked(f.italic());
    m_actions.underline->setChecked(f.underline());
    m_handingFormatChangeFromEditor = false;

    BtConfig & conf = btConfig();
    conf.setSessionValue(CHTMLWriteDisplayFontKey, m_textEdit->currentFont());
    conf.setSessionValue(CHTMLWriteDisplayFontColorKey, m_textEdit->textColor());
}

void BtTextEditDialog::slotFontSizeChosen(int newSize) {
    if (!m_handingFormatChangeFromEditor)
        m_textEdit->setFontPointSize(static_cast<qreal>(newSize));
}

void BtTextEditDialog::slotFontFamilyChosen(const QFont& font) {
    if (!m_handingFormatChangeFromEditor)
        m_textEdit->setFontFamily(font.family());
}

QString BtTextEditDialog::text() const {
    return m_textEdit->toHtml();
}

void BtTextEditDialog::setText(const QString& text) {
    m_textEdit->setHtml(text);
}



