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
#include "../util/btconnect.h"
#include "keychooser/ckeychooser.h"
#include "messagedialog.h"


BtCopyByReferencesDialog::BtCopyByReferencesDialog(
        BtConstModuleList const & modules,
        BTHistory * historyPtr,
        CSwordKey * key,
        BtModuleTextModel const * model,
        CDisplayWindow * parent)
    : QDialog(parent)
    , m_modules(modules)
    , m_key(key)
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

    m_keyChooser1 =
            CKeyChooser::createInstance(modules, historyPtr, key->copy(), this);
    gridLayout->addWidget(m_keyChooser1,0,1);

    auto * const hLayout = new QHBoxLayout;
    vLayout->addLayout(hLayout);

    auto * const label2 = new QLabel(tr("Last"));
    gridLayout->addWidget(label2, 1,0);

    m_keyChooser2 =
            CKeyChooser::createInstance(modules, historyPtr, key->copy(), this);
    gridLayout->addWidget(m_keyChooser2,1,1);

    m_moduleNameCombo = new QComboBox();
    gridLayout->addWidget(m_moduleNameCombo, 2,1);

    m_sizeToLarge = new QLabel(tr("Copy size is too large."));
    m_sizeToLarge->setVisible(false);
    hLayout->addWidget(m_sizeToLarge);

    auto const buttons =
            new QDialogButtonBox(QDialogButtonBox::Ok
                                 | QDialogButtonBox::Cancel);
    message::prepareDialogBox(buttons);
    hLayout->addWidget(buttons);
    m_okButton = buttons->button(QDialogButtonBox::Ok);

    { // Load selection keys:
        for (auto m : m_modules)
            m_moduleNameCombo->addItem(m->name());

        auto column = parent->getSelectedColumn();
        if (column < 0)
            column = 0;

        auto const first = parent->getFirstSelectedIndex();
        auto const last = parent->getLastSelectedIndex();
        if (first >= 0 && last >= 0) {
            m_keyChooser1->setKey(m_moduleTextModel->indexToKey(first, 0));
            m_keyChooser2->setKey(m_moduleTextModel->indexToKey(last, 0));

            auto const index =
                    m_moduleNameCombo->findText(m_modules.at(column)->name());
            if (index >= 0)
                m_moduleNameCombo->setCurrentIndex(index);
        } // else default to top of view.
    }

    auto const handleKeyChanged = [this]{
        bool const toLarge = isCopyToLarge(m_keyChooser1->key()->key(),
                                           m_keyChooser2->key()->key());
        m_sizeToLarge->setVisible(toLarge);
        m_okButton->setEnabled(!toLarge);
    };

    BT_CONNECT(m_keyChooser1, &CKeyChooser::keyChanged, handleKeyChanged);
    BT_CONNECT(m_keyChooser2, &CKeyChooser::keyChanged, handleKeyChanged);

    BT_CONNECT(buttons, &QDialogButtonBox::accepted,
               this, &BtCopyByReferencesDialog::accept);
    BT_CONNECT(buttons, &QDialogButtonBox::rejected,
               this, &BtCopyByReferencesDialog::reject);

    handleKeyChanged();
}

bool BtCopyByReferencesDialog::isCopyToLarge(QString const & ref1,
                                             QString const & ref2)
{
    { // Normalize references:
        decltype(m_refIndexes) ri;
        std::unique_ptr<CSwordKey> key(m_key->copy());
        key->setKey(ref1);
        ri.index1 = m_moduleTextModel->keyToIndex(*key);
        key->setKey(ref2);
        ri.index2 = m_moduleTextModel->keyToIndex(*key);
        ri.r1 = ref1;
        ri.r2 = ref2;
        if (ri.index1 > ri.index2) {
            ri.r1.swap(ri.r2);
            std::swap(ri.index1, ri.index2);
        }
        m_refIndexes = ri;
    }

    auto const type = m_modules.at(0)->type();
    if (type == CSwordModuleInfo::Bible || type == CSwordModuleInfo::Commentary)
        return m_refIndexes.index2 - m_refIndexes.index1 > 2700;
    return m_refIndexes.index2 - m_refIndexes.index1 > 100;
}

int BtCopyByReferencesDialog::getColumn()
{ return m_moduleNameCombo->currentIndex(); }
