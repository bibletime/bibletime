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

#ifndef TOOL_H
#define TOOL_H

#include <QIcon>
#include <QString>
#include <QTextCodec>


class CSwordModuleInfo;
class QLabel;
class QWidget;

namespace util {
namespace tool {

/**
  Creates the file filename and save the given text into the file.

  \param[in] filename the filename to save to.
  \param[in] text the string data to save.
  \param[in] forceOverwrite whether to force the overwrite.
  \param[in] fileCodec the codec to use to save the given string data.
  \note if the file exists, and forceOverwrite is false, a confirmation dialog
        is shown to ask the user whether to overwrite the existing file.
  \returns whether the file was properly saved.
*/
bool savePlainFile(const QString & filename,
                   const QString & text,
                   QTextCodec * fileCodec = QTextCodec::codecForLocale());

/**
  \param[in] module the module whose icon to return.
  \returns the icon used for the a module.
*/
QIcon const & getIconForModule(const CSwordModuleInfo * module);

/**
  Creates a new explanation label.

  \param[in] parent the parent widget.
  \param[in] heading the heading of the label.
  \param[in] text the text of the label.
  \returns a new QLabel initialized by initExplanationLabel().
*/
QLabel * explanationLabel(QWidget * parent,
                          const QString & heading,
                          const QString & text);

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
  \param[in] module The module required for the tooltip
  \returns the remote module's tooltip text.
*/
QString remoteModuleToolTip(const CSwordModuleInfo & module,
                            const QString & localVer);

/**
  \brief Calculates a maximum rendered text width for a widget and a string with
         the a given length.

  This function can be used for setting the size for a widget. It may be better
  to roughly calculate the size based on some text width rather than use a hard-
  coded value.

  \param[in] widget the widget whose font metrics to use. If 0, then the font
                    metrics of the application are used.
  \param[in] mCount the length of the string of 'M' characters to use for
                    calculating the width.

  \returns the width in pixels.
*/
int mWidth(const QWidget * widget, int mCount);

} /* namespace tool { */
} /* namespace util { */

#endif
