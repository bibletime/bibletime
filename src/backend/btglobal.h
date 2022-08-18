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

#include <QMetaType>


class QDataStream;


/**
  Filter options to control the text display of modules. Uses int and not bool
  because not all options have just two toggle values.
*/
struct FilterOptions {
    int footnotes; /**< 0 for disabled, 1 for enabled */
    int strongNumbers; /**< 0 for disabled, 1 for enabled */
    int headings; /**< 0 for disabled, 1 for enabled */
    int morphTags; /**< 0 for disabled, 1 for enabled */
    int lemmas; /**< 0 for disabled, 1 for enabled */
    int hebrewPoints; /**< 0 for disabled, 1 for enabled */
    int hebrewCantillation; /**< 0 for disabled, 1 for enabled */
    int greekAccents; /**< 0 for disabled, 1 for enabled */
    int textualVariants; /**< Number n to enabled the n-th variant */
    int redLetterWords; /**< 0 for disabled, 1 for enabled */
    int scriptureReferences; /**< 0 for disabled, 1 for enabled */
    int morphSegmentation; /**< 0 for disabled, 1 for enabled */
    FilterOptions();
    bool filterOptionsAreEqual(FilterOptions const & opts) const noexcept;
};
Q_DECLARE_METATYPE(FilterOptions)

/**
  Controls the display of a text.
*/
struct DisplayOptions {
    int lineBreaks;
    int verseNumbers;
    bool displayOptionsAreEqual(DisplayOptions const & opts) const noexcept;

/**
  Work around for Windows compiler bug in Visual Studio 2008 & 2010. The Crash
  occurs at the return statement of the not anymore existing
  CBTConfig::getDisplayOptionDefaults and is caused by a bad calling sequence
  when called from CDisplayWindow::init. The bug might still occur but now
  caused by BtConfig::getDisplayOptions.
  \todo Properly identify this bug and remove the #ifdef when fix is available.
*/
#ifdef Q_OS_WIN
    int notUsed;
#endif

};
Q_DECLARE_METATYPE(DisplayOptions)

/*!
 * Enumeration indicating the alignment mode
 * used for child windows.
 */
enum alignmentMode { /* Values provided for serialization */
    autoTileVertical = 0,
    autoTileHorizontal = 1,
    autoTile = 2,
    autoTabbed = 3,
    autoCascade = 4,
    manual = 5
};
QDataStream &operator<<(QDataStream &out, const alignmentMode &mode);
QDataStream &operator>>(QDataStream &in, alignmentMode &mode);
Q_DECLARE_METATYPE(alignmentMode)
