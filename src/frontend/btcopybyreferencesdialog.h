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

#pragma once

#include <QDialog>

#include "../backend/drivers/btmodulelist.h"


class BTHistory;
class BtModuleTextModel;
class CDisplayWindow;
class CKeyChooser;
class CSwordKey;
class QComboBox;
class QLabel;
class QPushButton;

class BtCopyByReferencesDialog : public QDialog {

    Q_OBJECT

public: /* Methods: */

    BtCopyByReferencesDialog(BtConstModuleList const & modules,
                             BTHistory * historyPtr,
                             CSwordKey * key,
                             BtModuleTextModel const * model,
                             CDisplayWindow * parent = nullptr);

    int getIndex1() const noexcept { return m_result.index1; }
    int getIndex2() const noexcept { return m_result.index2; }
    int getColumn();
    QString const & getReference1() const noexcept { return m_result.reference1; }
    QString const & getReference2() const noexcept { return m_result.reference2; }

private: /* Fields: */

    BtConstModuleList const m_modules;
    CSwordKey * m_key;
    CKeyChooser * m_keyChooser1;
    CKeyChooser * m_keyChooser2;
    QComboBox * m_moduleNameCombo;
    BtModuleTextModel const * m_moduleTextModel;
    QLabel * m_sizeToLarge;
    QPushButton * m_okButton;

    struct {
        QString reference1;
        QString reference2;
        int index1;
        int index2;
    } m_result;

}; /* class BtCopyByReferencesDialog */
