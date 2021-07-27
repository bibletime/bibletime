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

#include <map>
#include <QString>


class ColorManager {

public: /* Methods: */

    static ColorManager & instance();

    void loadColorMaps();
    QString getBackgroundColor(QString const & style = QString());
    QString getBackgroundHighlightColor(QString const & style = QString());
    QString getForegroundColor(QString const & style = QString());
    QString getCrossRefColor(QString const & style = QString());
    QString replaceColors(QString content);
    QString replaceDarkWithLight(const QString & content);

private: /* Methods: */

    ColorManager();

private: /* Fields: */

    std::map<QString, std::map<QString, QString> > m_colorMaps;

}; /* class ColorManager */
