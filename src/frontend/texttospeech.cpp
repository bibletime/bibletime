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

#include "texttospeech.h"

#include "../backend/config/btconfig.h"


namespace {

std::unique_ptr<QTextToSpeech> textToSpeech;

} // anonymous namespace


namespace BtTextToSpeech {

void speakText(QString const & text) {
    if (!textToSpeech)
        textToSpeech = newInstance();

    textToSpeech->say(text);
}

std::unique_ptr<QTextToSpeech> newInstance() {
    std::unique_ptr<QTextToSpeech> tts;

    auto const configuredEngine =
            btConfig().value<QString>(QStringLiteral("GUI/ttsEngine"));
    if (QTextToSpeech::availableEngines().contains(configuredEngine)) {
        tts = std::make_unique<QTextToSpeech>(configuredEngine);
    } else {
        tts = std::make_unique<QTextToSpeech>();
    }

    auto const configuredLocale =
            btConfig().value<QLocale>(QStringLiteral("GUI/ttsLocale"));
    if (tts->availableLocales().contains(configuredLocale))
        tts->setLocale(configuredLocale);

    auto const configuredVoice =
            btConfig().value<QString>(QStringLiteral("GUI/ttsVoice"));
    for (auto const & voice : tts->availableVoices()) {
        if (voice.name() == configuredVoice) {
            tts->setVoice(voice);
            break;
        }
    }

    return tts;
}

void reset() { textToSpeech.reset(); }

} // namespace BtTextToSpeech

#endif