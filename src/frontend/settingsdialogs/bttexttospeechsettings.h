/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2026 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifdef BUILD_TEXT_TO_SPEECH

#pragma once

#include "btconfigdialog.h"

#include <QObject>
#include <QTextToSpeech>


class CConfigurationDialog;
class QComboBox;
class QLabel;

class BtTextToSpeechSettingsPage: public BtConfigDialog::Page {

    Q_OBJECT

public: // methods

    BtTextToSpeechSettingsPage(CConfigurationDialog * parent = nullptr);
    void save() const final override;

private Q_SLOTS:

    void ttsEngineChanged();
    void ttsLocaleChanged();

private: // methods
    void updateLocales();
    void retranslateUi();

private: // fields

    std::unique_ptr<QTextToSpeech> m_tts;

    QLabel * m_infoNoPluginsLabel = nullptr;
    QLabel * m_ttsEngineLabel;
    QLabel * m_ttsLocaleLabel;
    QLabel * m_ttsVoiceLabel;
    QComboBox * m_ttsEngineComboBox;
    QComboBox * m_ttsLocaleComboBox;
    QComboBox * m_ttsVoiceComboBox;
};

#endif
