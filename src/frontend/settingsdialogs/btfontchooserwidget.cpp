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

#include "frontend/settingsdialogs/btfontchooserwidget.h"

#include <QFontDatabase>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTextBrowser>
#include "frontend/settingsdialogs/clistwidget.h"
#include "util/btconnect.h"


namespace {

const QString DEFAULT_FONT_PREVIEW_TEXT =
    "1 In the beginning God created the heaven and the earth. "
    "2 And the earth was without form, and void; and darkness was on the face of the deep. "
    " And the Spirit of God moved on the face of the waters.";

} // anonymous namespace

BtFontChooserWidget::BtFontChooserWidget(QWidget* parent)
        : QFrame(parent)
        , m_htmlText(DEFAULT_FONT_PREVIEW_TEXT)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    createLayout();
    connectListWidgets();
    loadFonts();
    setFrameStyle(QFrame::Box);
    setFrameShadow(QFrame::Raised);

    retranslateUi();
}

void BtFontChooserWidget::createLayout() {
    m_fontNameLabel = new QLabel(this);

    m_fontListWidget = new CListWidget();
    m_fontListWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    m_fontListWidget->setMinimumHeight(50);

    m_fontStyleLabel = new QLabel(this);

    m_styleListWidget = new CListWidget();
    m_styleListWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    m_styleListWidget->setMinimumHeight(50);
    m_styleListWidget->setCharWidth(12);

    m_fontSizeLabel = new QLabel(this);

    m_sizeListWidget = new CListWidget();
    m_sizeListWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    m_sizeListWidget->setMinimumHeight(50);
    m_sizeListWidget->setCharWidth(5);

    m_fontPreview = new QTextBrowser(this);

    QGridLayout *l = new QGridLayout;
    l->addWidget(m_fontNameLabel, 0, 0);
    l->addWidget(m_fontListWidget, 1, 0);
    l->addWidget(m_fontStyleLabel, 0, 1);
    l->addWidget(m_styleListWidget, 1, 1);
    l->addWidget(m_fontSizeLabel, 0, 2);
    l->addWidget(m_sizeListWidget, 1, 2);
    l->addWidget(m_fontPreview, 2, 0, 1, 3);
    setLayout(l);
}

void BtFontChooserWidget::retranslateUi() {
    m_fontNameLabel->setText(tr("Font name:"));
    m_fontStyleLabel->setText(tr("Font style:"));
    m_fontSizeLabel->setText(tr("Size:"));
}

void BtFontChooserWidget::connectListWidgets() {
    BT_CONNECT(m_fontListWidget,
               SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
               this,
               SLOT(fontChanged(QListWidgetItem *, QListWidgetItem *)));
    BT_CONNECT(
             m_styleListWidget,
             SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
             this,
             SLOT(styleChanged(QListWidgetItem *, QListWidgetItem *)));
    BT_CONNECT(
             m_sizeListWidget,
             SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
             this,
             SLOT(sizeChanged(QListWidgetItem *, QListWidgetItem *)));
}

void BtFontChooserWidget::fontChanged(QListWidgetItem* current, QListWidgetItem* /*previous*/) {
    if (current == nullptr)
        return;

    const QString fontFamily = current->text();
    m_font.setFamily(fontFamily);
    loadStyles(fontFamily);
    outputHtmlText();
    emit fontSelected(m_font);
}

void BtFontChooserWidget::loadFonts() {
    m_fontListWidget->clear();
    QFontDatabase database;
    Q_FOREACH(QString const & font, database.families())
        m_fontListWidget->addItem(font);

    // This triggers loading the styles for the first font
    m_fontListWidget->setCurrentRow(0);
}

void BtFontChooserWidget::loadStyles(const QString& font) {
    m_styleListWidget->clear();
    QFontDatabase database;
    Q_FOREACH (const QString &style, database.styles(font)) {
        m_styleListWidget->addItem(style);
        // This triggers loading the sizes for the first style
        restoreListWidgetValue(m_styleListWidget, m_choosenStyle);
    }
}

void BtFontChooserWidget::loadSizes(const QString& font, const QString& style) {
    const QString saveText = saveListWidgetValue(m_sizeListWidget);

    // Put new values into listWidget
    m_sizeListWidget->clear();
    QFontDatabase database;
    Q_FOREACH(int const size, database.pointSizes(font, style))
        m_sizeListWidget->addItem(QString::number(size));

    restoreListWidgetValue(m_sizeListWidget, saveText);
}

void BtFontChooserWidget::outputHtmlText() {
    m_fontPreview->setHtml(
        QString(
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
            "<html xmlns=\"http://www.w3.org/1999/xhtml\">"
            "<head>"
                "<style type=\"text/css\">"
                    "*{background-color:%1;"
                      "color:%2;"
                      "font-family:%3;"
                      "font-size:%4pt;"
                      "font-weight:%5;"
                      "font-style:%6}"
                "</style>"
                "</head>"
                "<body>"
                    "<div style=\"display: inline;\" lang=\"en\">"
                        "%7"
                    "</div>"
                "</body>"
            "</html>"
        )
        .arg(palette().base().color().name())
        .arg(palette().text().color().name())
        .arg(m_font.family())
        .arg(QString::number(m_font.pointSize()))
        .arg(m_font.bold() ? "bold" : "normal")
        .arg(m_font.italic() ? "italic" : "normal")
        .arg(m_htmlText)
    );
}

void BtFontChooserWidget::restoreListWidgetValue(QListWidget* listWidget, const QString& value) {
    if (!value.isEmpty()) {
        for (int i = 0; i < listWidget->count(); i++) {
            if (listWidget->item(i)->text() == value) {
                listWidget->setCurrentRow(i);
                return;
            }
        }
    }
    listWidget->setCurrentRow(0);
}

QString BtFontChooserWidget::saveListWidgetValue(QListWidget* listWidget) {
    int row = listWidget->currentRow();
    if (row >= 0)
        return listWidget->item(row)->text();

    return QString();
}

void BtFontChooserWidget::setFont(const QFont& font) {
    disconnect(m_fontListWidget, nullptr, nullptr, nullptr);
    disconnect(m_styleListWidget, nullptr, nullptr, nullptr);
    disconnect(m_sizeListWidget, nullptr, nullptr, nullptr);

    // set the font
    m_font = font;
    const QString fontFamily = font.family();
    restoreListWidgetValue(m_fontListWidget, fontFamily);

    // set the style
    loadStyles(fontFamily);
    QFontDatabase database;
    const QString styleString = database.styleString(m_font);
    m_choosenStyle = styleString;
    restoreListWidgetValue(m_styleListWidget, styleString);

    // set the size
    loadSizes(fontFamily, styleString);
    restoreListWidgetValue(m_sizeListWidget, QString::number(m_font.pointSize()) );

    outputHtmlText();
    connectListWidgets();
}

void BtFontChooserWidget::setFontStyle(const QString& styleString, QFont* font) {
    font->setBold(styleString.contains("bold", Qt::CaseInsensitive));
    font->setItalic(styleString.contains("italic", Qt::CaseInsensitive)
                    || styleString.contains("oblique", Qt::CaseInsensitive));
}

void BtFontChooserWidget::setSampleText(const QString& htmlText) {
    m_htmlText = htmlText;
    outputHtmlText();
}

void BtFontChooserWidget::sizeChanged(QListWidgetItem* current, QListWidgetItem* /*previous*/) {
    if (current == nullptr)
        return;

    m_font.setPointSize(m_sizeListWidget->currentItem()->text().toInt());

    outputHtmlText();
    emit fontSelected(m_font);
}

QSize BtFontChooserWidget::sizeHint() const {
    return QSize(170, 100);
}

void BtFontChooserWidget::styleChanged(QListWidgetItem* current, QListWidgetItem* /*previous*/) {
    if (current == nullptr)
        return;

    QString styleString = current->text();
    setFontStyle(styleString, &m_font);

    // Save style if the user choose it
    if (m_styleListWidget->hasFocus())
        m_choosenStyle = styleString;

    loadSizes(m_fontListWidget->currentItem()->text(), styleString);

    outputHtmlText();
    emit fontSelected(m_font);
}
