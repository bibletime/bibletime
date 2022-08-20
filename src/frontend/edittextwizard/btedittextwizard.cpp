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

#include "btedittextwizard.h"

#include <QByteArray>
#include <QDialog>
#include "btedittextpage.h"
#include "btplainorhtmlpage.h"
#include "../../backend/config/btconfig.h"


void translateQWizardStandardButtons(QWizard * wizard);

namespace {
auto const GeometryKey = QStringLiteral("GUI/EditTextWizard/geometry");
auto const HtmlModeKey = QStringLiteral("GUI/EditTextWizard/htmlMode");
} // anonymous namespace

BtEditTextWizard::BtEditTextWizard(QWidget * parent, Qt::WindowFlags flags)
    : QWizard(parent, flags)
    , m_plainOrHtmlPage(new BtPlainOrHtmlPage(this))
    , m_editTextPage(new BtEditTextPage(*this))
{
    addPage(m_plainOrHtmlPage);
    addPage(m_editTextPage);

    retranslateUi();
    restoreGeometry(btConfig().value<QByteArray>(GeometryKey, QByteArray()));
    m_plainOrHtmlPage->setHtmlMode(btConfig().value<bool>(HtmlModeKey, true));
}

void BtEditTextWizard::accept() {
    btConfig().setValue(GeometryKey, saveGeometry());
    btConfig().setValue(HtmlModeKey, htmlMode());
    QDialog::accept();
}

bool BtEditTextWizard::htmlMode() const {
    return m_plainOrHtmlPage->htmlMode();
}

void BtEditTextWizard::setFont(const QFont& font) {
    m_editTextPage->setFont(font);
}

void BtEditTextWizard::setText(const QString &text) {
    m_editTextPage->setText(text);
}

void BtEditTextWizard::setTitle(const QString& text) {
    setWindowTitle(text);
}

QString BtEditTextWizard::text() const {
    return m_editTextPage->text();
}

void BtEditTextWizard::retranslateUi() {
    translateQWizardStandardButtons(this);
}
