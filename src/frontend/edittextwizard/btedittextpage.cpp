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

#include "btedittextpage.h"

#include <QAction>
#include <QDebug>
#include <QFontComboBox>
#include <Qt>
#include <QtGlobal>
#include <QTextEdit>
#include <QToolBar>
#include <QVBoxLayout>
#include "../../backend/config/btconfig.h"
#include "../../backend/config/btconfigcore.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../display/btcolorwidget.h"
#include "../display/btfontsizewidget.h"
#include "btedittextwizard.h"


class QTextCharFormat;

namespace {
const QString CHTMLWriteDisplayGroup = "HtmlWriteDisplay";
const QString CHTMLWriteDisplayFontKey = "font";
const QString CHTMLWriteDisplayFontColorKey = "fontColor";
}


BtEditTextPage::BtEditTextPage(BtEditTextWizard & parent)
    : QWizardPage(&parent)
    , m_wizard(parent)
    , m_plainTextEdit(new QTextEdit(this))
    , m_htmlTextEdit(new QTextEdit(this))
    , m_toolBar(new QToolBar(this))
    , m_handingFormatChangeFromEditor(false)
{
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

    BT_CONNECT(m_htmlTextEdit, &QTextEdit::currentCharFormatChanged,
               [this](QTextCharFormat const &) {
                   BT_ASSERT(!m_handingFormatChangeFromEditor);
                   m_handingFormatChangeFromEditor = true;
                   QFont f = m_htmlTextEdit->currentFont();
                   Q_EMIT signalFontChanged(f);
                   Q_EMIT signalFontSizeChanged(f.pointSize());
                   Q_EMIT signalFontColorChanged(m_htmlTextEdit->textColor());

                   m_actions.bold->setChecked(f.bold());
                   m_actions.italic->setChecked(f.italic());
                   m_actions.underline->setChecked(f.underline());
                   m_handingFormatChangeFromEditor = false;

                   auto conf = btConfig().session().group(
                                   CHTMLWriteDisplayGroup);
                   conf.setValue(CHTMLWriteDisplayFontKey,
                                 m_htmlTextEdit->currentFont());
                   conf.setValue(CHTMLWriteDisplayFontColorKey,
                                 m_htmlTextEdit->textColor());
               });

    retranslateUi();
}

void BtEditTextPage::cleanupPage() {
    if (m_htmlTextEdit->isVisible())
        m_text = m_htmlTextEdit->toHtml();
    else
        m_text = m_plainTextEdit->toPlainText();
}

void BtEditTextPage::initializePage() {
    auto const htmlMode = m_wizard.htmlMode();
    if (htmlMode) {
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

    m_plainTextEdit->setVisible(!htmlMode);
    m_htmlTextEdit->setVisible(htmlMode);
    m_toolBar->setVisible(htmlMode);
}

bool BtEditTextPage::validatePage() {
    if (m_htmlTextEdit->isVisible())
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
    auto const conf = btConfig().session().group(CHTMLWriteDisplayGroup);
    m_htmlTextEdit->setTextColor(conf.value(CHTMLWriteDisplayFontColorKey, m_htmlTextEdit->textColor()));
    QFont f = conf.value(CHTMLWriteDisplayFontKey, m_htmlTextEdit->currentFont());
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
    BT_CONNECT(m_actions.bold, &QAction::toggled,
               [this](bool const checked) {
                   if (!m_handingFormatChangeFromEditor)
                       m_htmlTextEdit->setFontWeight(
                                   checked ? QFont::Bold : QFont::Normal);
               });

    //--------------------italic toggle-------------------------
    m_actions.italic = new QAction(
                CResMgr::displaywindows::writeWindow::italicText::icon(),
                tr("Italic"),
                this );
    m_actions.italic->setCheckable(true);
    m_actions.italic->setChecked(f.italic());
    m_actions.bold->setShortcut(CResMgr::displaywindows::writeWindow::italicText::accel);
    BT_CONNECT(m_actions.italic, &QAction::toggled,
               [this](bool const checked) {
                   if (!m_handingFormatChangeFromEditor)
                       m_htmlTextEdit->setFontItalic(checked);
               });
    m_actions.italic->setToolTip( tr("Italic") );

    //--------------------underline toggle-------------------------
    m_actions.underline = new QAction(
                CResMgr::displaywindows::writeWindow::underlinedText::icon(),
                tr("Underline"),
                this );
    m_actions.underline->setCheckable(true);
    m_actions.underline->setChecked(f.underline());
    m_actions.underline->setShortcut(CResMgr::displaywindows::writeWindow::underlinedText::accel);
    BT_CONNECT(m_actions.underline, &QAction::toggled,
               [this](bool const checked) {
                   if (!m_handingFormatChangeFromEditor)
                       m_htmlTextEdit->setFontUnderline(checked);
               });
    m_actions.underline->setToolTip( tr("Underline") );

    //--------------------align left toggle-------------------------
    m_actions.alignLeft = new QAction(
                CResMgr::displaywindows::writeWindow::alignLeft::icon(),
                tr("Left"), this);
    m_actions.alignLeft->setCheckable(true);
    m_actions.alignLeft->setShortcut(CResMgr::displaywindows::writeWindow::alignLeft::accel);
    BT_CONNECT(m_actions.alignLeft, &QAction::toggled,
               [this](bool const set) {
                   if (!m_handingFormatChangeFromEditor
                       && set
                       && (m_htmlTextEdit->alignment() != Qt::AlignLeft))
                   {
                       m_htmlTextEdit->setAlignment(Qt::AlignLeft);
                       alignmentChanged(Qt::AlignLeft);
                   }
               });
    m_actions.alignLeft->setToolTip( tr("Align left") );

    //--------------------align center toggle-------------------------
    m_actions.alignCenter = new QAction(
                CResMgr::displaywindows::writeWindow::alignCenter::icon(),
                tr("Center"), this);
    m_actions.alignCenter->setCheckable(true);
    m_actions.alignCenter->setShortcut(CResMgr::displaywindows::writeWindow::alignCenter::accel);
    BT_CONNECT(m_actions.alignCenter, &QAction::toggled,
               [this](bool const set) {
                   if (!m_handingFormatChangeFromEditor
                       && set
                       && (m_htmlTextEdit->alignment() != Qt::AlignHCenter))
                   {
                       m_htmlTextEdit->setAlignment(Qt::AlignHCenter);
                       alignmentChanged(Qt::AlignHCenter);
                   }
               });
    m_actions.alignCenter->setToolTip( tr("Center") );

    //--------------------align right toggle-------------------------
    m_actions.alignRight = new QAction(
                CResMgr::displaywindows::writeWindow::alignRight::icon(),
                tr("Right"), this);
    m_actions.alignRight->setCheckable(true);
    m_actions.alignRight->setShortcut(CResMgr::displaywindows::writeWindow::alignRight::accel);
    BT_CONNECT(m_actions.alignRight, &QAction::toggled,
               [this](bool const set) {
                   if (!m_handingFormatChangeFromEditor
                       && set
                       && (m_htmlTextEdit->alignment() != Qt::AlignRight))
                   {
                       m_htmlTextEdit->setAlignment(Qt::AlignRight);
                       alignmentChanged(Qt::AlignRight);
                   }
               });
    m_actions.alignRight->setToolTip( tr("Align right") );
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
    BT_CONNECT(fontFamilyCombo, &QFontComboBox::currentFontChanged,
               [this](QFont const & font) {
                   if (!m_handingFormatChangeFromEditor)
                       m_htmlTextEdit->setFontFamily(font.family());
               });
    BT_CONNECT(this,            &BtEditTextPage::signalFontChanged,
               fontFamilyCombo, &QFontComboBox::setCurrentFont,
               Qt::DirectConnection);

    //--------------------font size chooser-------------------------
    BtFontSizeWidget* fontSizeChooser = new BtFontSizeWidget(this);
    fontSizeChooser->setFontSize(f.pointSize());
    fontSizeChooser->setToolTip( tr("Font size") );
    m_toolBar->addWidget(fontSizeChooser);
    BT_CONNECT(fontSizeChooser, &BtFontSizeWidget::fontSizeChanged,
               [this](int const newSize) {
                   if (!m_handingFormatChangeFromEditor)
                       m_htmlTextEdit->setFontPointSize(
                                   static_cast<qreal>(newSize));
               });
    BT_CONNECT(this,            &BtEditTextPage::signalFontSizeChanged,
               fontSizeChooser, &BtFontSizeWidget::setFontSize,
               Qt::DirectConnection);

    //--------------------color button-------------------------
    BtColorWidget* fontColorChooser = new BtColorWidget();
    fontColorChooser->setColor(m_htmlTextEdit->textColor());
    fontColorChooser->setToolTip(tr("Font color"));
    m_toolBar->addWidget(fontColorChooser);
    BT_CONNECT(fontColorChooser, &BtColorWidget::changed,
               m_htmlTextEdit,   &QTextEdit::setTextColor,
               Qt::DirectConnection);
    BT_CONNECT(this,             &BtEditTextPage::signalFontColorChanged,
               fontColorChooser, &BtColorWidget::setColor,
               Qt::DirectConnection);

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
