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
#include <QString>
#include <vector>
#include "../../backend/config/btconfig.h"


class CConfigurationDialog;
class Language;
class QCheckBox;
class QComboBox;
class QFontDialog;
class QGroupBox;
class QLabel;

class BtFontSettingsPage: public BtConfigDialog::Page {

        Q_OBJECT

    private: // types:

        struct WorkSetting {
            Language const & language;
            BtConfig::FontSettingsPair settings;
        };

    public: // methods:

        BtFontSettingsPage(CConfigurationDialog *parent = nullptr);

        void save() const final override;

    protected Q_SLOTS:

        // This slot is called when the "Use own font for language" button was clicked.
        void useOwnFontClicked(bool);

    private: // methods:

        void retranslateUi();

    private: // fields:

        QGroupBox *m_fontsGroupBox;
        QLabel *m_languageLabel;
        QComboBox *m_languageComboBox;
        QCheckBox *m_languageCheckBox;
        QFontDialog * m_fontChooser;

        std::vector<WorkSetting> m_workSettings;

};
