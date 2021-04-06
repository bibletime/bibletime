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


class QComboBox;
class QPushButton;
class CDisplayWindow;
class QLabel;
class CKeyChooser;
class BTHistory;
class CSwordKey;
class BtModuleTextModel;

class BtCopyByReferencesDialog : public QDialog {
        Q_OBJECT
    public:
        BtCopyByReferencesDialog(const BtConstModuleList & modules,
                                 BTHistory * historyPtr,
                                 CSwordKey * key,
                                 BtModuleTextModel const * model,
                                 CDisplayWindow * parent = nullptr);

    int getIndex1() const noexcept { return m_refIndexes.index1; }
    int getIndex2() const noexcept { return m_refIndexes.index2; }
    int getColumn();
    QString const & getReference1() const noexcept { return m_refIndexes.r1; }
    QString const & getReference2() const noexcept { return m_refIndexes.r2; }

    private:

        bool isCopyToLarge(const QString& ref1, const QString& ref2);

        const BtConstModuleList m_modules;
        CSwordKey * m_key;
        CKeyChooser * m_keyChooser1;
        CKeyChooser * m_keyChooser2;
        QComboBox * m_moduleNameCombo;
        BtModuleTextModel const * m_moduleTextModel;
        QLabel * m_sizeToLarge;
        QPushButton * m_okButton;
        CDisplayWindow * m_displayWindow;

        struct {
            QString r1;
            QString r2;
            int index1;
            int index2;
        } m_refIndexes;

};
