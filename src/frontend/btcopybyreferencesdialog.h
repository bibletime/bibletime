/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include <QDialog>

#include "../backend/drivers/btmodulelist.h"
#include "display/modelview/btqmlinterface.h"


class QComboBox;
class QDialogButtonBox;
class CDisplayWindow;
class QLabel;
class CKeyChooser;
class BTHistory;
class CSwordKey;
class BtModuleTextModel;
struct RefIndexes;


class BtCopyByReferencesDialog : public QDialog {
        Q_OBJECT
    public:
        BtCopyByReferencesDialog(const BtConstModuleList & modules,
                                 BTHistory * historyPtr,
                                 CSwordKey * key,
                                 BtModuleTextModel const * model,
                                 CDisplayWindow * parent = nullptr);

    int getIndex1();
    int getIndex2();
    int getColumn();
    QString getReference1();
    QString getReference2();

    private Q_SLOTS:

        void slotKeyChanged(CSwordKey * newKey);

    private:

        bool isCopyToLarge(const QString& ref1, const QString& ref2);
        RefIndexes normalizeReferences(const QString& ref1, const QString& ref2);
        void loadSelectionKeys();

        const BtConstModuleList m_modules;
        CSwordKey * m_key;
        CKeyChooser * m_keyChooser1;
        CKeyChooser * m_keyChooser2;
        QComboBox * m_moduleNameCombo;
        BtModuleTextModel const * m_moduleTextModel;
        QLabel * m_sizeToLarge;
        QDialogButtonBox * m_buttons;
        CDisplayWindow * m_displayWindow;

        RefIndexes m_ri;

};
