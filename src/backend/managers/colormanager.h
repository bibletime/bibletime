
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

#ifndef COLORMANAGER_H
#define COLORMANAGER_H

#include <QMap>
#include <QString>


class ColorManager {

public: /* Types: */

    typedef QMap<QString, QString> ColorMap;

public: /* Methods: */

    static ColorManager & instance();

    QString loadColorMaps();
    QString replaceColors(const QString& content);
    QString getBackgroundColor(const QString& style = QString());
    QString getForegroundColor(const QString& style = QString());
    QString getCrossRefColor(const QString& style = QString());

private: /* Fields: */

    ColorManager();
    ColorMap createColorMapWithDefaults();
    bool darkMode() const;
    QString getColorByPattern(const QString& pattern, const QString& style = QString());
    void loadColorMap(const QString & filename);

    QMap<QString, ColorMap> m_colorMaps;

}; /* class ColorManager */

#endif
