/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
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
* Creates the file filename and put the text of parameter "text" into the file.
* @return True if saving was sucessful, otherwise false
*/
bool savePlainFile( const QString& filename, const QString& text, const bool& forceOverwrite = false, QTextCodec* fileCodec = QTextCodec::codecForLocale());

/**
  \param[in] module the module whose icon to return.
  \returns the icon used for the a module.
*/
QIcon getIconForModule(const CSwordModuleInfo *module);

/**
  \param[in] module the module whose icon name to return.
  \returns the icon name used for the a module.
*/
QString getIconNameForModule(const CSwordModuleInfo *module);

/**
  Returns a new QLabel initialized by initExplanationLabel().
*/
QLabel *explanationLabel(QWidget *parent, const QString &heading, const QString &text);

/**
  Initializes a QLabel to explain difficult things of dialogs. The label should be used to
  explain difficult things of the GUI, e.g. in the options dialog pages.
  \param[in] label The label to initialize
  \param[in] heading The heading for the label.
  \param[in] text The text for the label.
*/
void initExplanationLabel(QLabel *label, const QString &heading, const QString &text);

/**
* Returns true if the character at position "pos" of text is inside an HTML tag. Returns false if it's not inside an HTML tag.
*/
bool inHTMLTag(int pos, QString & text);

/** Return the module's tooltip text for a remote module
* @param module The module required for the tooltip
* @return The tooltip text for the passed module
*/
QString remoteModuleToolTip(const CSwordModuleInfo &module,
                            const QString &localVer);

/**
* Returns the width in pixels for a string which has mCount 'M' letters, using the specified widget's font.
* This can be used when setting the size for a widget. It may be better to roughly calculate the size based on some text width rather than use pixels directly.
*/
int mWidth(const QWidget* widget, int mCount);
}
}

#endif
