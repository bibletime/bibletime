/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 2024 by the BibleTime developers.
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


BtTextToSpeechSettingsPage::BtTextToSpeechSettingsPage(CConfigurationDialog *parent)
        : BtConfigDialog::Page(CResMgr::settings::audio::icon(), parent) {
    setHeaderText(tr("Text-to-speech"));

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout();

    const QStringList availableEngines = QTextToSpeech::availableEngines();
    if (availableEngines.isEmpty()) {
        formLayout->addWidget(new QLabel(tr("No Qt text-to-speech plug-ins were found. Please install a plugin and restart the application."), this));
    }

    QLabel* ttsEngineLabel = new QLabel(tr("Text-to-speech engine:"), this);
    m_ttsEngineComboBox = new QComboBox(this);
    ttsEngineLabel->setBuddy(m_ttsEngineComboBox);
    formLayout->addRow(ttsEngineLabel, m_ttsEngineComboBox);

    QLabel* ttsLocaleLabel = new QLabel(tr("Text-to-speech language:"), this);
    m_ttsLocaleComboBox = new QComboBox(this);
    ttsLocaleLabel->setBuddy(m_ttsLocaleComboBox);
    formLayout->addRow(ttsLocaleLabel, m_ttsLocaleComboBox);

    QLabel* ttsVoiceLabel = new QLabel(tr("Text-to-speech voice:"), this);
    m_ttsVoiceComboBox = new QComboBox(this);
    ttsVoiceLabel->setBuddy(m_ttsVoiceComboBox);
    formLayout->addRow(ttsVoiceLabel, m_ttsVoiceComboBox);

    m_tts = BibleTime::createTextToSpeechInstance();

    m_ttsEngineComboBox->addItems(availableEngines);
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
    m_ttsEngineComboBox->setCurrentText(m_tts->engine());
#else
    // QTextToSpeech::engine() only available in Qt >= 6.4, identify
    // current engine by available locales and voices
    if (availableEngines.size() > 1) {
        for (const QString& engineName : availableEngines) {
            QTextToSpeech candidate = QTextToSpeech(engineName);
            if (candidate.availableLocales() == m_tts->availableLocales()) {
                candidate.setLocale(m_tts->locale());
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                if (candidate.availableVoices() == m_tts->availableVoices()) {
#else
                // in Qt 5, QTextToSpeech::availableVoices returns a QVector, and QVoice::operator==
                // doesn't fulfill the requirements to use QVector::operator==, so convert to QList
                if (candidate.availableVoices().toList() == m_tts->availableVoices().toList()) {
#endif
                    m_ttsEngineComboBox->setCurrentText(engineName);
                    break;
                }
            }
        }
    }
#endif

    updateLocales();

    BT_CONNECT(m_ttsEngineComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
               this, &BtTextToSpeechSettingsPage::ttsEngineChanged);

    BT_CONNECT(m_ttsLocaleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
               this, &BtTextToSpeechSettingsPage::ttsLocaleChanged);

    mainLayout->addLayout(formLayout);
}

void BtTextToSpeechSettingsPage::ttsEngineChanged()
{
    m_tts = std::make_unique<QTextToSpeech>(m_ttsEngineComboBox->currentText());
    updateLocales();
}

void BtTextToSpeechSettingsPage::updateLocales()
{
    m_ttsLocaleComboBox->clear();
    QVector<QLocale> locales = m_tts->availableLocales();
    std::sort(locales.begin(), locales.end(),
              [](const QLocale& locale1, const QLocale& locale2) {
                return formatLocale(locale1) < formatLocale(locale2);
              });
    for (const QLocale& locale : std::as_const(locales)) {
        m_ttsLocaleComboBox->addItem(formatLocale(locale), locale);
        if (locale == m_tts->locale()) {
            m_ttsLocaleComboBox->setCurrentIndex(m_ttsLocaleComboBox->count() - 1);
        }
    }

    ttsLocaleChanged();
}

void BtTextToSpeechSettingsPage::ttsLocaleChanged()
{
    m_ttsVoiceComboBox->clear();

    const QVariant userData = m_ttsLocaleComboBox->currentData();
    if (!userData.canConvert<QLocale>()) {
        return;
    }

    m_tts->setLocale(userData.toLocale());

    for (const QVoice& voice : m_tts->availableVoices()) {
        m_ttsVoiceComboBox->addItem(voice.name());
    }

    m_ttsVoiceComboBox->setCurrentText(m_tts->voice().name());
}


QString BtTextToSpeechSettingsPage::formatLocale(const QLocale& locale)
{
    return QLocale::languageToString(locale.language()) + " (" + QLocale::countryToString(locale.country()) + ")";
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
