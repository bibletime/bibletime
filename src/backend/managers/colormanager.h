
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

private: /* Types: */

    using ColorMap = QMap<QString, QString>;

public: /* Methods: */

    static ColorManager & instance();

    void loadColorMaps();
    QString replaceColors(QString content);
    QString getBackgroundColor(QString const & style = QString());
    QString getForegroundColor(QString const & style = QString());
    QString getCrossRefColor(QString const & style = QString());

private: /* Methods: */

    ColorManager();
    ColorMap createColorMapWithDefaults();
    bool darkMode() const;
    QString getColorByPattern(QString const & pattern,
                              QString const & style = QString());
    void loadColorMap(QString const & filename);

private: /* Fields: */

    QMap<QString, ColorMap> m_colorMaps;

}; /* class ColorManager */

#endif
