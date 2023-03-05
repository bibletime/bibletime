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
#include <QGridLayout>
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
    , m_firstKeyLabel(new QLabel(this))
    , m_lastKeyLabel(new QLabel(this))
    , m_sizeTooLargeLabel(new QLabel(this))
{
    setMinimumWidth(400);

    auto * const vLayout = new QVBoxLayout(this);
    setLayout(vLayout);

    auto * const gridLayout = new QGridLayout();
    gridLayout->setHorizontalSpacing(15);
    gridLayout->setVerticalSpacing(15);
    gridLayout->setContentsMargins(11,11,11,16);
    vLayout->addLayout(gridLayout);

    auto const modules = parent->constModules();

    m_workCombo = new QComboBox(this);
    for (auto const * const m : modules)
        m_workCombo->addItem(m->name(),
                             QVariant::fromValue(
                                 const_cast<void *>(
                                     static_cast<void const *>(m))));
    gridLayout->addWidget(m_workCombo, 0, 1);

    gridLayout->addWidget(m_firstKeyLabel, 1, 0);

    {
        auto const type = modules.at(0)->type();
        m_copyThreshold = (type == CSwordModuleInfo::Bible
                           || type == CSwordModuleInfo::Commentary)
                          ? 2700
                          : 100;
    }

    auto const parentKey = parent->key();

    m_firstKeyChooser =
            CKeyChooser::createInstance(modules, parentKey->copy(), this);
    gridLayout->addWidget(m_firstKeyChooser, 1, 1);

    auto * const hLayout = new QHBoxLayout;
    vLayout->addLayout(hLayout);

    gridLayout->addWidget(m_lastKeyLabel, 2, 0);

    m_lastKeyChooser =
            CKeyChooser::createInstance(modules, parentKey->copy(), this);
    gridLayout->addWidget(m_lastKeyChooser, 2, 1);

    m_sizeTooLargeLabel->setVisible(false);
    hLayout->addWidget(m_sizeTooLargeLabel);

    auto const buttons =
            new QDialogButtonBox(QDialogButtonBox::Ok
                                 | QDialogButtonBox::Cancel);
    message::prepareDialogBox(buttons);
    hLayout->addWidget(buttons);
    auto * const okButton = buttons->button(QDialogButtonBox::Ok);

    // Apply selection, if present:
    if (selection.has_value()) {
        BT_ASSERT(selection->column < modules.size());
        m_firstKeyChooser->setKey(
                    model->indexToKey(selection->startIndex, 0));
        m_lastKeyChooser->setKey(model->indexToKey(selection->endIndex, 0));
        m_workCombo->setCurrentIndex(selection->column);
    } // else default to top of view.

    auto const handleKeyChanged = [this, model, okButton]{
        // Calculate result:
        m_result.key1 = m_firstKeyChooser->key();
        m_result.key2 = m_lastKeyChooser->key();
        m_result.index1 = model->keyToIndex(*m_result.key1);
        m_result.index2 = model->keyToIndex(*m_result.key2);
        if (m_result.index1 > m_result.index2) { // ensure order:
            std::swap(m_result.key1, m_result.key2);
            std::swap(m_result.index1, m_result.index2);
        }

        bool const tooLarge =
                m_result.index2 - m_result.index1 > m_copyThreshold;
        m_sizeTooLargeLabel->setVisible(tooLarge);
        okButton->setEnabled(!tooLarge);
    };

    BT_CONNECT(m_firstKeyChooser, &CKeyChooser::keyChanged, handleKeyChanged);
    BT_CONNECT(m_lastKeyChooser, &CKeyChooser::keyChanged, handleKeyChanged);

    BT_CONNECT(buttons, &QDialogButtonBox::accepted,
               [this] {
                   auto const userData = m_workCombo->currentData();
                   m_result.module =
                           static_cast<CSwordModuleInfo const *>(
                               userData.value<void *>());
                   accept();
               });
    BT_CONNECT(buttons, &QDialogButtonBox::rejected,
               this, &BtCopyByReferencesDialog::reject);

    retranslateUi();
    handleKeyChanged();
}

void BtCopyByReferencesDialog::retranslateUi() {
    setWindowTitle(tr("Copy by References"));
    m_firstKeyLabel->setText(tr("First"));
    m_lastKeyLabel->setText(tr("Last"));
    m_sizeTooLargeLabel->setText(tr("Copy size is too large!"));
}
