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

#include "btedittextpage.h"
#include "btedittextwizard.h"

#include <QAction>
#include <QApplication>
#include <QFontComboBox>
#include <QTextEdit>
#include <QToolBar>
#include <QVBoxLayout>
#include "../../backend/config/btconfig.h"
#include "../display/btfontsizewidget.h"
#include "../display/btcolorwidget.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"

namespace {
const QString CHTMLWriteDisplayFontKey = "HtmlWriteDisplay/font";
const QString CHTMLWriteDisplayFontColorKey = "HtmlWriteDisplay/fontColor";
}


BtEditTextPage::BtEditTextPage(QWidget * parent)
    : QWizardPage(parent)
    , m_plainTextEdit(new QTextEdit(this))
    , m_htmlTextEdit(new QTextEdit(this))
    , m_toolBar(new QToolBar(this))
    , m_wizard(nullptr)
    , m_handingFormatChangeFromEditor(false) {

    m_wizard = qobject_cast<BtEditTextWizard *>(parent);
    QVBoxLayout * verticalLayout = new QVBoxLayout(this);
    verticalLayout->addWidget(m_toolBar);
    verticalLayout->addWidget(m_plainTextEdit);
    verticalLayout->addWidget(m_htmlTextEdit);

    setMinimumWidth(560);
    initActions();
    setupToolBar();

    m_plainTextEdit->setAcceptDrops(true);
    m_htmlTextEdit->setAcceptDrops(true);
    m_plainTextEdit->setAutoFormatting(QTextEdit::AutoNone);
    m_htmlTextEdit->setAutoFormatting(QTextEdit::AutoAll);

    BT_CONNECT(m_htmlTextEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
               this, SLOT(slotCurrentCharFormatChanged(QTextCharFormat)),
               Qt::DirectConnection);

    retranslateUi();
}

void BtEditTextPage::cleanupPage() {
    m_htmlMode = m_wizard->htmlMode();
    if (m_htmlMode)
        m_text = m_htmlTextEdit->toHtml();
    else
        m_text = m_plainTextEdit->toPlainText();
}

void BtEditTextPage::initializePage() {
    m_htmlMode = m_wizard->htmlMode();
    if (m_htmlMode) {
        initHtmlFont();
        m_htmlTextEdit->setHtml(m_text);
        m_plainTextEdit->clear();
    }
    else {
        m_plainTextEdit->setFont(m_plainTextFont);
        m_plainTextEdit->setText(m_text);

        { // Needed to strip html when switching to plain text.
            QString tmp = m_plainTextEdit->toPlainText();
            m_plainTextEdit->setText(tmp);
        }

        m_htmlTextEdit->clear();
    }
    m_plainTextEdit->setAcceptRichText(false);
    m_htmlTextEdit->setAcceptRichText(true);

    m_plainTextEdit->setVisible(!m_htmlMode);
    m_htmlTextEdit->setVisible(m_htmlMode);
    m_toolBar->setVisible(m_htmlMode);
}

bool BtEditTextPage::validatePage() {
    m_htmlMode = m_wizard->htmlMode();
    if (m_htmlMode)
        m_text = m_htmlTextEdit->toHtml();
    else
        m_text = m_plainTextEdit->toPlainText();
    return true;
}

QString BtEditTextPage::text() const {
    return m_text;
}

void BtEditTextPage::setText(const QString& text) {
    m_text = text;
}

void BtEditTextPage::setFont(const QFont& font) {
    m_plainTextFont = font;
}

void BtEditTextPage::retranslateUi() {
}

QFont BtEditTextPage::initHtmlFont() {
    BtConfig & conf = btConfig();
    m_htmlTextEdit->setTextColor(conf.sessionValue(CHTMLWriteDisplayFontColorKey, m_htmlTextEdit->textColor()));
    QFont f = conf.sessionValue(CHTMLWriteDisplayFontKey, m_htmlTextEdit->currentFont());
    m_htmlTextEdit->setCurrentFont(f);
    return f;
}

void BtEditTextPage::initActions() {
    QFont f = initHtmlFont();

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

void BtEditTextPage::toggleBold(bool checked) {
    if (!m_handingFormatChangeFromEditor)
        m_htmlTextEdit->setFontWeight(checked ? QFont::Bold : QFont::Normal);
}

void BtEditTextPage::toggleItalic(bool checked) {
    if (!m_handingFormatChangeFromEditor)
        m_htmlTextEdit->setFontItalic(checked);
}

void BtEditTextPage::toggleUnderline(bool checked) {
    if (!m_handingFormatChangeFromEditor)
        m_htmlTextEdit->setFontUnderline(checked);
}

void BtEditTextPage::alignLeft(bool set) {
    if (!m_handingFormatChangeFromEditor && set && (m_htmlTextEdit->alignment() != Qt::AlignLeft)) {
        m_htmlTextEdit->setAlignment(Qt::AlignLeft);
        alignmentChanged(Qt::AlignLeft);
    }
}

void BtEditTextPage::alignCenter(bool set) {
    if (!m_handingFormatChangeFromEditor && set && (m_htmlTextEdit->alignment() != Qt::AlignHCenter)) {
        m_htmlTextEdit->setAlignment(Qt::AlignHCenter);
        alignmentChanged(Qt::AlignHCenter);
    }
}

void BtEditTextPage::alignRight(bool set) {
    if (!m_handingFormatChangeFromEditor && set && (m_htmlTextEdit->alignment() != Qt::AlignRight)) {
        m_htmlTextEdit->setAlignment(Qt::AlignRight);
        alignmentChanged(Qt::AlignRight);
    }
}

/** The text's alignment changed. Enable the right buttons. */
void BtEditTextPage::alignmentChanged( int a ) {
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


void BtEditTextPage::setupToolBar() {

    QFont f = m_htmlTextEdit->currentFont();

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
    fontColorChooser->setColor(m_htmlTextEdit->textColor());
    fontColorChooser->setToolTip(tr("Font color"));
    m_toolBar->addWidget(fontColorChooser);
    BT_CONNECT(fontColorChooser, SIGNAL(changed(QColor const &)),
               m_htmlTextEdit,       SLOT(setTextColor(QColor const &)),
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

void BtEditTextPage::slotCurrentCharFormatChanged(const QTextCharFormat &) {
    BT_ASSERT(!m_handingFormatChangeFromEditor);
    m_handingFormatChangeFromEditor = true;
    QFont f = m_htmlTextEdit->currentFont();
    emit signalFontChanged(f);
    emit signalFontSizeChanged(f.pointSize());
    emit signalFontColorChanged(m_htmlTextEdit->textColor());

    m_actions.bold->setChecked(f.bold());
    m_actions.italic->setChecked(f.italic());
    m_actions.underline->setChecked(f.underline());
    m_handingFormatChangeFromEditor = false;

    BtConfig & conf = btConfig();
    conf.setSessionValue(CHTMLWriteDisplayFontKey, m_htmlTextEdit->currentFont());
    conf.setSessionValue(CHTMLWriteDisplayFontColorKey, m_htmlTextEdit->textColor());
}

void BtEditTextPage::slotFontSizeChosen(int newSize) {
    if (!m_handingFormatChangeFromEditor)
        m_htmlTextEdit->setFontPointSize(static_cast<qreal>(newSize));
}

void BtEditTextPage::slotFontFamilyChosen(const QFont& font) {
    if (!m_handingFormatChangeFromEditor)
        m_htmlTextEdit->setFontFamily(font.family());
}
