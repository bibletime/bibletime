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


namespace ColorManager {

QString replaceColors(QString content);
QString replaceColors(QString content, QString const & templateName);
QString getBackgroundColor();
QString getBackgroundColor(QString const & templateName);
QString getBackgroundHighlightColor();
QString getBackgroundHighlightColor(QString const & templateName);
QString getForegroundColor();
QString getForegroundColor(QString const & templateName);
QString getCrossRefColor();
QString getCrossRefColor(QString const & templateName);

} /* namespace ColorManager */
