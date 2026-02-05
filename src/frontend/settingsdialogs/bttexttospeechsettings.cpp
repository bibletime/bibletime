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

#include "bttexttospeechsettings.h"

#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QLocale>
#include <QVoice>
#include "../../backend/config/btconfig.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../bibletime.h"
#include "cconfigurationdialog.h"

namespace {

QString formatLocale(const QLocale& locale) {
    return QObject::tr("%1 (%2)", "Locale language and territory")
        .arg(QLocale::languageToString(locale.language()),
             QLocale::territoryToString(locale.territory()));
}

} // anonymous namespace

BtTextToSpeechSettingsPage::BtTextToSpeechSettingsPage(CConfigurationDialog * parent)
        : BtConfigDialog::Page(CResMgr::settings::speaker::icon(), parent) {

    auto * const mainLayout = new QVBoxLayout(this);
    auto * const formLayout = new QFormLayout();

    auto const availableEngines = QTextToSpeech::availableEngines();
    if (availableEngines.isEmpty()) {
        m_infoNoPluginsLabel = new QLabel(this);
        formLayout->addWidget(m_infoNoPluginsLabel);
    }

    m_ttsEngineLabel = new QLabel(this);
    m_ttsEngineComboBox = new QComboBox(this);
    m_ttsEngineLabel->setBuddy(m_ttsEngineComboBox);
    formLayout->addRow(m_ttsEngineLabel, m_ttsEngineComboBox);

    m_ttsLocaleLabel = new QLabel(this);
    m_ttsLocaleComboBox = new QComboBox(this);
    m_ttsLocaleComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_ttsLocaleLabel->setBuddy(m_ttsLocaleComboBox);
    formLayout->addRow(m_ttsLocaleLabel, m_ttsLocaleComboBox);

    m_ttsVoiceLabel = new QLabel(this);
    m_ttsVoiceComboBox = new QComboBox(this);
    m_ttsVoiceComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_ttsVoiceLabel->setBuddy(m_ttsVoiceComboBox);
    formLayout->addRow(m_ttsVoiceLabel, m_ttsVoiceComboBox);

    m_tts = BibleTime::createTextToSpeechInstance();

    m_ttsEngineComboBox->addItems(availableEngines);
    m_ttsEngineComboBox->setCurrentText(m_tts->engine());

    updateLocales();

    BT_CONNECT(m_ttsEngineComboBox, &QComboBox::currentIndexChanged,
               this, &BtTextToSpeechSettingsPage::ttsEngineChanged);

    BT_CONNECT(m_ttsLocaleComboBox, &QComboBox::currentIndexChanged,
               this, &BtTextToSpeechSettingsPage::ttsLocaleChanged);

    mainLayout->addLayout(formLayout);

    retranslateUi();
}

void BtTextToSpeechSettingsPage::ttsEngineChanged() {
    m_tts = std::make_unique<QTextToSpeech>(m_ttsEngineComboBox->currentText());
    updateLocales();
}

void BtTextToSpeechSettingsPage::updateLocales() {
    m_ttsLocaleComboBox->clear();

    struct LocaleInfo {
        QLocale locale;
        QString displayText;
    };
    QList<LocaleInfo> localeInfos;
    for (auto const & locale : m_tts->availableLocales())
        localeInfos.push_back({locale, formatLocale(locale)});

    std::sort(localeInfos.begin(), localeInfos.end(),
              [](auto const & locale1, auto const & locale2) {
                return QString::localeAwareCompare(locale1.displayText,
                                                   locale2.displayText) < 0;
              });
    for (auto const & localeInfo : std::as_const(localeInfos)) {
        m_ttsLocaleComboBox->addItem(localeInfo.displayText, localeInfo.locale);
        if (localeInfo.locale == m_tts->locale())
            m_ttsLocaleComboBox->setCurrentIndex(m_ttsLocaleComboBox->count() - 1);
    }

    ttsLocaleChanged();
}

void BtTextToSpeechSettingsPage::retranslateUi() {
    setHeaderText(tr("Text-to-speech"));
    if (m_infoNoPluginsLabel)
        m_infoNoPluginsLabel->setText(
            tr("No Qt text-to-speech plug-ins were found. Please install a "
               "plugin and restart the application."));
    m_ttsEngineLabel->setText(tr("Text-to-speech engine:"));
    m_ttsLocaleLabel->setText(tr("Text-to-speech language:"));
    m_ttsVoiceLabel->setText(tr("Text-to-speech voice:"));
}

void BtTextToSpeechSettingsPage::ttsLocaleChanged() {
    m_ttsVoiceComboBox->clear();

    auto const userData = m_ttsLocaleComboBox->currentData();
    if (!userData.canConvert<QLocale>())
        return;

    m_tts->setLocale(userData.toLocale());

    for (auto const & voice : m_tts->availableVoices())
        m_ttsVoiceComboBox->addItem(voice.name());

    m_ttsVoiceComboBox->setCurrentText(m_tts->voice().name());
}

void BtTextToSpeechSettingsPage::save() const {
    btConfig().setValue(QStringLiteral("GUI/ttsEngine"),
                        m_ttsEngineComboBox->currentText());
    btConfig().setValue(QStringLiteral("GUI/ttsLocale"),
                        m_ttsLocaleComboBox->currentData().toLocale());
    btConfig().setValue(QStringLiteral("GUI/ttsVoice"),
                        m_ttsVoiceComboBox->currentText());
}

#endif
