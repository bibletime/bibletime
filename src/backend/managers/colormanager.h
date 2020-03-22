
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
#include <QObject>
#include <QString>

typedef QMap<QString, QString> ColorMap;
typedef QMap<QString, ColorMap *> ColorMaps;

class ColorManager : public QObject {

    Q_OBJECT

    public: 
        static ColorManager * instance();
        QString loadColorMaps();
        ColorMap* createColorMapWithDefaults();
        QString replaceColors(const QString& content);
        QString getBackgroundColor(const QString& style = QString());
        QString getForegroundColor(const QString& style = QString());
        QString getCrossRefColor(const QString& style = QString());

    private: 
        ColorManager(QObject * parent = nullptr);
        bool darkMode() const;
        QString getColorByPattern(const QString& pattern, const QString& style = QString());
        void loadColorMap(const QString & filename);

        static ColorManager * m_instance;
        ColorMaps m_colorMaps;

}; /* class ColorManager */

#endif
