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


class BtModuleTextModel;
class CDisplayWindow;
class CKeyChooser;
class CSwordKey;
class CSwordModuleInfo;
class QComboBox;
class QLabel;
class QPushButton;

class BtCopyByReferencesDialog : public QDialog {

    Q_OBJECT

public: /* Types: */

    struct Result {
        CSwordModuleInfo const * module;
        QString reference1;
        QString reference2;
        int index1;
        int index2;
    };

public: /* Methods: */

    BtCopyByReferencesDialog(BtModuleTextModel const * model,
                             CDisplayWindow * parent);

    Result const & result() const noexcept { return m_result; }

private: /* Fields: */

    BtConstModuleList const m_modules;
    CSwordKey * m_key;
    CKeyChooser * m_keyChooser1;
    CKeyChooser * m_keyChooser2;
    QComboBox * m_moduleNameCombo;
    BtModuleTextModel const * m_moduleTextModel;
    QLabel * m_sizeTooLargeLabel;
    QPushButton * m_okButton;

    Result m_result;

}; /* class BtCopyByReferencesDialog */
