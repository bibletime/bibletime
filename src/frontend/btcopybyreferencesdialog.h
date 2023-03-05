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

#include <optional>
#include <QDialog>
#include "display/modelview/btqmlinterface.h"


class BtModuleTextModel;
class CDisplayWindow;
class CKeyChooser;
class CSwordKey;
class CSwordModuleInfo;
class QComboBox;
class QDialogButtonBox;
class QLabel;

class BtCopyByReferencesDialog : public QDialog {

    Q_OBJECT

public: // types:

    struct Result {
        CSwordModuleInfo const * module;
        CSwordKey const * key1;
        CSwordKey const * key2;
        int index1;
        int index2;
    };

public: // methods:

    BtCopyByReferencesDialog(
            BtModuleTextModel const * model,
            std::optional<BtQmlInterface::Selection> const & selection,
            CDisplayWindow * parent);

    Result const & result() const noexcept { return m_result; }

private: // Methods:

    void retranslateUi();
    void resetThreshold();

private: // fields:

    QLabel * const m_workLabel;
    QComboBox * const m_workCombo;
    QLabel * const m_firstKeyLabel;
    CKeyChooser * m_firstKeyChooser;
    QLabel * const m_lastKeyLabel;
    CKeyChooser * m_lastKeyChooser;
    QLabel * const m_sizeTooLargeLabel;
    QDialogButtonBox * const m_buttonBox;

    Result m_result;

}; /* class BtCopyByReferencesDialog */
