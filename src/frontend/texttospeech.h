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

#pragma once

#ifdef BUILD_TEXT_TO_SPEECH

#include <memory>
#include <QString>
#include <QTextToSpeech>


namespace BtTextToSpeech {

/**
  \brief Speaks the given text.
  \param[in] text The text to speak.
*/
void speakText(QString const & text);

/**
  \brief Creates a QTextToSpeech instance based on settings.
  \returns The QTextToSpeech instance.
*/
std::unique_ptr<QTextToSpeech> newInstance();

/**
  \brief Resets the active text-to-speech instance.
  \note This is needed in case the settings change.
*/
void reset();

} // namespace BtTextToSpeech

#endif
