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

#include "btcopybyreferencesdialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "../backend/keys/cswordkey.h"
#include "../backend/models/btmoduletextmodel.h"
#include "../backend/drivers/cswordmoduleinfo.h"
#include "../frontend/displaywindow/cdisplaywindow.h"
#include "../util/btassert.h"
#include "../util/btconnect.h"
#include "keychooser/ckeychooser.h"
#include "messagedialog.h"


BtCopyByReferencesDialog::BtCopyByReferencesDialog(
        BtModuleTextModel const * model,
        std::optional<BtQmlInterface::Selection> const & selection,
        CDisplayWindow * parent)
    : QDialog(parent)
    , m_workLabel(new QLabel(this))
    , m_workCombo(new QComboBox(this))
    , m_firstKeyLabel(new QLabel(this))
    , m_lastKeyLabel(new QLabel(this))
    , m_sizeTooLargeLabel(new QLabel(this))
    , m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok
                                       | QDialogButtonBox::Cancel, this))
{
    setMinimumWidth(400);

    auto * const vLayout = new QVBoxLayout(this);

    auto * const formLayout = new QFormLayout();
    formLayout->setHorizontalSpacing(15);
    formLayout->setVerticalSpacing(15);
    formLayout->setContentsMargins(11, 11, 11, 16);
    vLayout->addLayout(formLayout);

    auto const modules = parent->constModules();

    for (auto const * const m : modules)
        m_workCombo->addItem(m->name(),
                             QVariant::fromValue(
                                 const_cast<void *>(
                                     static_cast<void const *>(m))));
    m_workLabel->setBuddy(m_workCombo);
    BT_CONNECT(m_workCombo, qOverload<int>(&QComboBox::currentIndexChanged),
               this, &BtCopyByReferencesDialog::resetThreshold);
    formLayout->addRow(m_workLabel, m_workCombo);

    auto const parentKey = parent->key();

    m_firstKeyChooser =
            CKeyChooser::createInstance(modules, parentKey->copy(), this);
    m_firstKeyLabel->setBuddy(m_firstKeyChooser);
    formLayout->addRow(m_firstKeyLabel, m_firstKeyChooser);

    auto * const hLayout = new QHBoxLayout;
    vLayout->addLayout(hLayout);

    m_lastKeyChooser =
            CKeyChooser::createInstance(modules, parentKey->copy(), this);
    m_lastKeyLabel->setBuddy(m_lastKeyChooser);
    formLayout->addRow(m_lastKeyLabel, m_lastKeyChooser);

    m_sizeTooLargeLabel->setVisible(false);
    hLayout->addWidget(m_sizeTooLargeLabel);

    message::prepareDialogBox(m_buttonBox);
    hLayout->addWidget(m_buttonBox);

    // Apply selection, if present:
    if (selection.has_value()) {
        BT_ASSERT(selection->column < modules.size());
        m_firstKeyChooser->setKey(
                    model->indexToKey(selection->startIndex, 0));
        m_lastKeyChooser->setKey(model->indexToKey(selection->endIndex, 0));
        m_workCombo->setCurrentIndex(selection->column);
    } // else default to top of view.

    auto const handleKeyChanged = [this, model]{
        // Calculate result:
        m_result.key1 = m_firstKeyChooser->key();
        m_result.key2 = m_lastKeyChooser->key();
        m_result.index1 = model->keyToIndex(*m_result.key1);
        m_result.index2 = model->keyToIndex(*m_result.key2);
        if (m_result.index1 > m_result.index2) { // ensure order:
            std::swap(m_result.key1, m_result.key2);
            std::swap(m_result.index1, m_result.index2);
        }
        resetThreshold();
    };

    BT_CONNECT(m_firstKeyChooser, &CKeyChooser::keyChanged, handleKeyChanged);
    BT_CONNECT(m_lastKeyChooser, &CKeyChooser::keyChanged, handleKeyChanged);

    BT_CONNECT(m_buttonBox, &QDialogButtonBox::accepted,
               [this] {
                   auto const userData = m_workCombo->currentData();
                   m_result.module =
                           static_cast<CSwordModuleInfo const *>(
                               userData.value<void *>());
                   accept();
               });
    BT_CONNECT(m_buttonBox, &QDialogButtonBox::rejected,
               this, &BtCopyByReferencesDialog::reject);

    retranslateUi();
    handleKeyChanged();
}

void BtCopyByReferencesDialog::retranslateUi() {
    setWindowTitle(tr("Copy by References"));
    m_workLabel->setText(tr("&Work:"));
    m_firstKeyLabel->setText(tr("&First:"));
    m_lastKeyLabel->setText(tr("&Last:"));
    m_sizeTooLargeLabel->setText(tr("Copy size is too large!"));
}

void BtCopyByReferencesDialog::resetThreshold() {
    auto const type =
            static_cast<CSwordModuleInfo const *>(
                m_workCombo->currentData().value<void *>())->type();
    auto const copyThreshold = (type == CSwordModuleInfo::Bible
                                || type == CSwordModuleInfo::Commentary)
                               ? 2700
                               : 100;
    bool const tooLarge =
            m_result.index2 - m_result.index1 > copyThreshold;
    m_sizeTooLargeLabel->setVisible(tooLarge);
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!tooLarge);
}
