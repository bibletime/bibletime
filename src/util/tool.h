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

#include <QString>


class CSwordModuleInfo;
class QIcon;
class QLabel;
class QTextStream;
class QWidget;

namespace util {
namespace tool {

/**
  Creates the file filename and writes to it using a callback..

  \param[in] filename the filename to save to.
  \param[in] writer the writer callback.
  \param[in] userPtr arbitrary data passed on to the callback.
  \param[in] fileCodec the codec to use to save the given string data.
  \warning if a file with the given name already exists, it is first removed.
  \returns whether the file was properly saved.
*/
bool savePlainFile(QString const & filename,
                   void (&writer)(QTextStream &, void *),
                   void * userPtr);

/**
  Creates the file filename and save the given text into the file.

  \param[in] filename the filename to save to.
  \param[in] text the string data to save.
  \param[in] fileCodec the codec to use to save the given string data.
  \warning if a file with the given name already exists, it is first removed.
  \returns whether the file was properly saved.
*/
bool savePlainFile(QString const & filename, QString const & text);

/**
  \param[in] module the module whose icon to return.
  \returns the icon used for the a module.
*/
QIcon const & getIconForModule(const CSwordModuleInfo * module);

/**
  \brief Initializes a QLabel to explain difficult things of dialogs.

  The label should be used to explain difficult things of the GUI, e.g. in the
  options dialog pages.

  \param[in] label The label to initialize
  \param[in] heading The heading for the label.
  \param[in] text The text for the label.
*/
void initExplanationLabel(QLabel * label,
                          const QString & heading,
                          const QString & text);

/**
  \returns whether the character at position "pos" of text is inside an HTML tag.
*/
bool inHTMLTag(int pos, const QString & text);

/**
  \brief Calculates a maximum rendered text width for a widget and a string with
         the a given length.

  This function can be used for setting the size for a widget. It may be better
  to roughly calculate the size based on some text width rather than use a hard-
  coded value.

  \param[in] widget the widget whose font metrics to use.
  \param[in] mCount the length of the string of 'M' characters to use for
                    calculating the width.

  \returns the width in pixels.
*/
int mWidth(QWidget const & widget, int const mCount);

/**
   \param[in] input The potentially invalid BCP 47 string from Sword to fix.
   \returns a string (hopefully) more conformant to BCP 47
 */
QString fixSwordBcp47(QString input);

} /* namespace tool { */
} /* namespace util { */
