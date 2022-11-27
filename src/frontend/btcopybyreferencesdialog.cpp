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
#include "../frontend/display/btmodelviewreaddisplay.h"
#include "../frontend/displaywindow/cdisplaywindow.h"
#include "../util/btassert.h"
#include "../util/btconnect.h"
#include "keychooser/ckeychooser.h"
#include "messagedialog.h"


BtCopyByReferencesDialog::BtCopyByReferencesDialog(
        BtModuleTextModel const * model,
        CDisplayWindow * parent)
    : QDialog(parent)
    , m_key(parent->key())
    , m_moduleTextModel(model)
{
    setWindowTitle(tr("Copy by References"));
    setMinimumWidth(400);

    auto * const vLayout = new QVBoxLayout(this);
    setLayout(vLayout);

    auto * const gridLayout = new QGridLayout();
    gridLayout->setHorizontalSpacing(15);
    gridLayout->setVerticalSpacing(15);
    gridLayout->setContentsMargins(11,11,11,16);
    vLayout->addLayout(gridLayout);

    auto * const label1 = new QLabel(tr("First"));
    gridLayout->addWidget(label1, 0,0);

    auto * const historyPtr = parent->history();
    auto const modules = parent->constModules();
    {
        auto const type = modules.at(0)->type();
        m_copyThreshold = (type == CSwordModuleInfo::Bible
                           || type == CSwordModuleInfo::Commentary)
                          ? 2700
                          : 100;
    }

    m_keyChooser1 =
            CKeyChooser::createInstance(modules, historyPtr, m_key->copy(), this);
    gridLayout->addWidget(m_keyChooser1,0,1);

    auto * const hLayout = new QHBoxLayout;
    vLayout->addLayout(hLayout);

    auto * const label2 = new QLabel(tr("Last"));
    gridLayout->addWidget(label2, 1,0);

    m_keyChooser2 =
            CKeyChooser::createInstance(modules, historyPtr, m_key->copy(), this);
    gridLayout->addWidget(m_keyChooser2,1,1);

    m_moduleNameCombo = new QComboBox();
    gridLayout->addWidget(m_moduleNameCombo, 2,1);

    m_sizeTooLargeLabel = new QLabel(tr("Copy size is too large."));
    m_sizeTooLargeLabel->setVisible(false);
    hLayout->addWidget(m_sizeTooLargeLabel);

    auto const buttons =
            new QDialogButtonBox(QDialogButtonBox::Ok
                                 | QDialogButtonBox::Cancel);
    message::prepareDialogBox(buttons);
    hLayout->addWidget(buttons);
    m_okButton = buttons->button(QDialogButtonBox::Ok);

    { // Load selection keys:
        for (auto const * const m : modules)
            m_moduleNameCombo->addItem(m->name(),
                                       QVariant::fromValue(
                                           const_cast<void *>(
                                               static_cast<void const *>(m))));

        auto column = parent->getSelectedColumn();
        if (column < 0)
            column = 0;
        BT_ASSERT(column < modules.size());

        auto const first = parent->getFirstSelectedIndex();
        auto const last = parent->getLastSelectedIndex();
        if (first >= 0 && last >= 0) {
            m_keyChooser1->setKey(m_moduleTextModel->indexToKey(first, 0));
            m_keyChooser2->setKey(m_moduleTextModel->indexToKey(last, 0));
            m_moduleNameCombo->setCurrentIndex(column);
        } // else default to top of view.
    }

    auto const handleKeyChanged = [this]{
        // Calculate result:
        m_result.reference1 = m_keyChooser1->key()->key();
        m_result.reference2 = m_keyChooser2->key()->key();
        {
            std::unique_ptr<CSwordKey> key(m_key->copy());
            key->setKey(m_result.reference1);
            m_result.index1 = m_moduleTextModel->keyToIndex(*key);
            key->setKey(m_result.reference2);
            m_result.index2 = m_moduleTextModel->keyToIndex(*key);
        }
        if (m_result.index1 > m_result.index2) {
            m_result.reference1.swap(m_result.reference2);
            std::swap(m_result.index1, m_result.index2);
        }

        bool const tooLarge =
                m_result.index2 - m_result.index1 > m_copyThreshold;
        m_sizeTooLargeLabel->setVisible(tooLarge);
        m_okButton->setEnabled(!tooLarge);
    };

    BT_CONNECT(m_keyChooser1, &CKeyChooser::keyChanged, handleKeyChanged);
    BT_CONNECT(m_keyChooser2, &CKeyChooser::keyChanged, handleKeyChanged);

    BT_CONNECT(buttons, &QDialogButtonBox::accepted,
               [this] {
                   auto const userData = m_moduleNameCombo->currentData();
                   m_result.module =
                           static_cast<CSwordModuleInfo const *>(
                               userData.value<void *>());
                   accept();
               });
    BT_CONNECT(buttons, &QDialogButtonBox::rejected,
               this, &BtCopyByReferencesDialog::reject);

    handleKeyChanged();
}
