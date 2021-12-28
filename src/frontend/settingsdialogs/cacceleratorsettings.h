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

#include "btconfigdialog.h"

#include <QObject>
#include <QPointer>
#include <QString>


class BtActionCollection;
class BtShortcutsEditor;
class CConfigurationDialog;
class QComboBox;
class QLabel;
class QStackedWidget;

/**
 * @brief The shortcut settings page.
 */
class CAcceleratorSettingsPage: public BtConfigDialog::Page {

        Q_OBJECT

    public: // methods:

        CAcceleratorSettingsPage(CConfigurationDialog *parent = nullptr);

        void save() const final override;

    protected: // methods:

        void retranslateUi();

    private:

        struct WindowType {
            QPointer<BtShortcutsEditor> keyChooser;
            BtActionCollection * actionCollection = nullptr;
            QString title;
        };

        WindowType m_application;
        WindowType m_general;
        WindowType m_bible;
        WindowType m_commentary;
        WindowType m_lexicon;
        WindowType m_book;

        QLabel *m_actionGroupLabel;
        QComboBox* m_typeChooser;
        QStackedWidget* m_keyChooserStack;

};
