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

#include "crangechooserdialog.h"

#include <QByteArray>
#include <QDialogButtonBox>
#include <QFlags>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMap>
#include <QPushButton>
#include <Qt>
#include <QTextEdit>
#include <QVBoxLayout>
#include "../../backend/config/btconfig.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../backend/managers/cswordbackend.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../messagedialog.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsuggest-destructor-override"
#endif
#include <versekey.h>
#include <listkey.h>
#include <swkey.h>
#include <swmodule.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#pragma GCC diagnostic pop

namespace Search {

CRangeChooserDialog::CRangeChooserDialog(const QStringList& scopeModules, QWidget *parentDialog)
    : QDialog(parentDialog),
      m_scopeModules(scopeModules)
{
    initView();
    initConnections();

    retranslateUi();

    // Add the existing scopes
    BtConfig::StringMap map = btConfig().getSearchScopesForCurrentLocale(scopeModules);
    for (auto it = map.begin(); it != map.end(); ++it)
        new RangeItem(it.key(), it.value(), m_rangeList);
    resetEditControls();
}

void CRangeChooserDialog::initView() {
    m_rangeList = new QListWidget(this);
    m_rangeListLabel = new QLabel(this);
    m_rangeListLabel->setBuddy(m_rangeList);

    m_newRangeButton = new QPushButton(this);
    m_deleteRangeButton = new QPushButton(this);

    m_nameEdit = new QLineEdit(this);
    m_nameEditLabel = new QLabel(this);
    m_nameEditLabel->setBuddy(m_nameEdit);

    m_rangeEdit = new QTextEdit(this);
    m_rangeEditLabel = new QLabel(this);
    m_rangeEditLabel->setBuddy(m_rangeEdit);

    m_resultList = new QListWidget(this);
    m_resultListLabel = new QLabel(this);
    m_resultListLabel->setBuddy(m_resultList);

    QFrame *hSeparatorLine = new QFrame(this);
    hSeparatorLine->setFrameShape(QFrame::HLine);
    hSeparatorLine->setFrameShadow(QFrame::Sunken);

    m_buttonBox = new QDialogButtonBox(this);
    m_buttonBox->setOrientation(Qt::Horizontal);
    m_buttonBox->setStandardButtons(QDialogButtonBox::Ok
                                    | QDialogButtonBox::Cancel
                                    | QDialogButtonBox::RestoreDefaults);
    message::prepareDialogBox(m_buttonBox);

    QHBoxLayout *rangeButtonsLayout = new QHBoxLayout();
    rangeButtonsLayout->addWidget(m_newRangeButton);
    rangeButtonsLayout->addWidget(m_deleteRangeButton);

    QVBoxLayout* rangeListLayout = new QVBoxLayout;
    rangeListLayout->addWidget(m_rangeListLabel);
    rangeListLayout->addWidget(m_rangeList);
    rangeListLayout->addLayout(rangeButtonsLayout);

    QHBoxLayout* nameEditLayout = new QHBoxLayout();
    nameEditLayout->addWidget(m_nameEditLabel);
    nameEditLayout->addWidget(m_nameEdit);

    QVBoxLayout* rangeEditLayout = new QVBoxLayout();
    rangeEditLayout->addLayout(nameEditLayout);
    rangeEditLayout->addWidget(m_rangeEditLabel);
    rangeEditLayout->addWidget(m_rangeEdit);
    rangeEditLayout->addWidget(m_resultListLabel);
    rangeEditLayout->addWidget(m_resultList);

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addLayout(rangeListLayout);
    topLayout->addLayout(rangeEditLayout);

    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
    vboxLayout->addLayout(topLayout);
    vboxLayout->addWidget(hSeparatorLine);
    vboxLayout->addWidget(m_buttonBox);
}

void CRangeChooserDialog::initConnections() {
    BT_CONNECT(m_rangeList, &QListWidget::currentItemChanged,
               [this](QListWidgetItem *, QListWidgetItem * const previous) {
                   if (previous) {
                       BT_ASSERT(dynamic_cast<RangeItem *>(previous));
                       saveCurrentToRange(static_cast<RangeItem*>(previous));
                   }
                   resetEditControls();
               });
    BT_CONNECT(m_nameEdit, &QLineEdit::textEdited,
               this,       &CRangeChooserDialog::nameEditTextChanged);
    BT_CONNECT(m_rangeEdit, &QTextEdit::textChanged,
               [this]{
                   m_resultList->clear();
                   static QRegularExpression const re(
                       QStringLiteral(R"PCRE(\s*-\s*)PCRE"));
                   auto const range =
                           m_rangeEdit->toPlainText().replace(
                               re,
                               QStringLiteral("-"));

                   auto const & backend = CSwordBackend::instance();
                   for (auto const & moduleName : m_scopeModules) {
                       auto * const module =
                               backend.findModuleByName(moduleName);
                       if (!module)
                           continue;
                       auto const verses(
                                   sword::VerseKey(
                                       module->swordModule().getKey())
                                        .parseVerseList(
                                                range.toUtf8().constData(),
                                                "Genesis 1:1",
                                                true));
                       if (verses.getCount() > 0) {
                           for (int i = 0; i < verses.getCount(); i++) {
                               auto const * const elementText =
                                       verses.getElement(i)->getRangeText();
                               new QListWidgetItem(
                                           QString::fromUtf8(elementText),
                                           m_resultList);
                           }
                           break;
                       }
                   }
               });

    // Connect buttons:
    BT_CONNECT(m_buttonBox, &QDialogButtonBox::accepted,
               this,        &CRangeChooserDialog::accept);
    BT_CONNECT(m_buttonBox, &QDialogButtonBox::rejected,
               this,        &CRangeChooserDialog::reject);
    BT_CONNECT(m_newRangeButton, &QPushButton::clicked,
               [this]{
                   m_rangeList->setCurrentItem(
                                new RangeItem(tr("New range"),
                                              QString(),
                                              m_rangeList));
                   resetEditControls();
               });
    BT_CONNECT(m_deleteRangeButton, &QPushButton::clicked,
               [this]{
                   BT_ASSERT(dynamic_cast<RangeItem *>(
                                 m_rangeList->currentItem()));
                   QListWidgetItem * const i = m_rangeList->currentItem();
                   m_rangeList->removeItemWidget(i);
                   delete i;

                   resetEditControls();
               });
    QPushButton * defaultsButton = m_buttonBox->button(QDialogButtonBox::RestoreDefaults);
    BT_CONNECT(defaultsButton, &QPushButton::clicked,
               [this]{
                   m_rangeList->clear();
                   btConfig().deleteSearchScopesWithCurrentLocale();
                   auto const map(
                           btConfig().getSearchScopesForCurrentLocale(
                                   m_scopeModules));
                   for (auto it = map.begin(); it != map.end(); ++it)
                       new RangeItem(it.key(), it.value(), m_rangeList);
                   m_rangeList->setCurrentItem(nullptr);
                   resetEditControls();
               });
}

void CRangeChooserDialog::retranslateUi() {
    setWindowTitle(tr("Setup Search Scopes"));

    m_rangeListLabel->setText(tr("S&earch range:"));
    m_rangeList->setToolTip(tr("Select a scope from the list to edit the search"
                               "ranges"));

    m_newRangeButton->setText(tr("&Add new scope"));
    m_newRangeButton->setToolTip(tr("Add a new search scope. First enter an "
                                    "appropriate name, then edit the search "
                                    "ranges."));

    m_deleteRangeButton->setText(tr("Delete current &scope"));
    m_deleteRangeButton->setToolTip(tr("Delete the selected search scope"));

    m_nameEditLabel->setText(tr("&Name:"));
    m_nameEdit->setToolTip(tr("Change the name of the selected search scope"));

    m_rangeEditLabel->setText(tr("Edi&t current range:"));
    m_rangeEdit->setToolTip(tr("Change the search ranges of the selected search"
                               "scope item. Have a look at the predefined "
                               "search scopes to see how search ranges are "
                               "constructed."));

    m_resultListLabel->setText(tr("Parsed search range:"));
    m_resultList->setToolTip(tr("The search ranges which will be used for the "
                                "search, parsed to the canonical form"));
}

void CRangeChooserDialog::saveCurrentToRange(RangeItem * i) {
    if (!m_nameEdit->text().isEmpty())
        i->setCaption(m_nameEdit->text());

    i->setRange(m_rangeEdit->toPlainText());
}

void CRangeChooserDialog::resetEditControls() {
    const QListWidgetItem * const item = m_rangeList->currentItem();
    BT_ASSERT(!item || dynamic_cast<RangeItem const *>(item));
    const RangeItem * rangeItem = static_cast<const RangeItem *>(item);

    m_nameEdit->setEnabled(item != nullptr);
    m_rangeEdit->setEnabled(item != nullptr);
    m_resultList->setEnabled(item != nullptr);
    m_deleteRangeButton->setEnabled(item != nullptr);
    m_nameEdit->setText(item != nullptr ? rangeItem->caption() : QString());
    m_rangeEdit->setText(item != nullptr ? rangeItem->range() : QString());

    if (item != nullptr)
        m_nameEdit->setFocus();

    nameEditTextChanged(item != nullptr ? rangeItem->caption() : QString());
}

void CRangeChooserDialog::accept() {
    // Update the active item:
    QListWidgetItem *currentItem = m_rangeList->currentItem();
    if (currentItem != nullptr) {
        BT_ASSERT(dynamic_cast<RangeItem *>(currentItem));
        saveCurrentToRange(static_cast<RangeItem*>(currentItem));
    }

    // Save the new sorted map of search scopes:
    m_rangeList->sortItems();
    BtConfig::StringMap map;
    for (int i = 0; i < m_rangeList->count(); i++) {
        BT_ASSERT(dynamic_cast<RangeItem *>(m_rangeList->item(i)));
        const RangeItem * item = static_cast<RangeItem*>(m_rangeList->item(i));
        map[item->caption()] = item->range();
    }
    btConfig().setSearchScopesWithCurrentLocale(m_scopeModules, map);

    QDialog::accept();
}

void CRangeChooserDialog::nameEditTextChanged(const QString &newText) {
    const bool e = !newText.isEmpty() || m_rangeList->count() <= 0;
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(e);
}

} //end of namespace Search
