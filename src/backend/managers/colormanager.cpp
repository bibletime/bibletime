
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

#include "colormanager.h"

#include <QApplication>
#include <QDir>
#include <QPalette>
#include <QSettings>
#include <utility>
#include "../../util/directory.h"
#include "../../backend/managers/cdisplaytemplatemgr.h"


ColorManager & ColorManager::instance() {
    static ColorManager r;
    return r;
}

ColorManager::ColorManager(QObject * parent)
    : QObject(parent) {
}

bool ColorManager::darkMode() const {
    QPalette p = qApp->palette();
    QColor b = p.color(QPalette::Base);
    if (b.value() < 128)
        return true;
    return false;
}

QString ColorManager::loadColorMaps() {
    QString errorMessage;
    namespace DU = util::directory;
    const QDir & td = DU::getDisplayTemplatesDir(); // Global template directory
    const QDir & utd = DU::getUserDisplayTemplatesDir(); // User template directory
    const QDir::Filters readableFileFilter(QDir::Files | QDir::Readable);
    const QStringList cssFilter("*.css");

    // Load global app stylesheets
    Q_FOREACH(const QString & file, td.entryList(cssFilter, readableFileFilter))
        loadColorMap(td.canonicalPath() + "/" + file);

    // Load user app stylesheets
    Q_FOREACH(const QString & file, utd.entryList(cssFilter, readableFileFilter))
        loadColorMap(utd.canonicalPath() + "/" + file);

    return errorMessage;
}

ColorManager::ColorMap ColorManager::createColorMapWithDefaults(){
    ColorMap colorDefs;
    if (darkMode()) {
        colorDefs.insert("FOREGROUND_COLOR", qApp->palette().color(QPalette::WindowText).name());
        colorDefs.insert("BACKGROUND_COLOR", qApp->palette().color(QPalette::Base).name());
        colorDefs.insert("HIGHLIGHT_COLOR",  QColor("#ffff00").name());
        colorDefs.insert("CROSSREF_COLOR",  QColor("#aac2ff").name());
        colorDefs.insert("JESUS_WORDS_COLOR",QColor("#ff0000").name());
    } else {
        colorDefs.insert("FOREGROUND_COLOR", qApp->palette().color(QPalette::WindowText).name());
        colorDefs.insert("BACKGROUND_COLOR", qApp->palette().color(QPalette::Base).name());
        colorDefs.insert("HIGHLIGHT_COLOR",  QColor("#ffff00").name());
        colorDefs.insert("CROSSREF_COLOR",  QColor("#1414ff").name());
        colorDefs.insert("JESUS_WORDS_COLOR",QColor("#ff0000").name());
    }
    return colorDefs;
}

void ColorManager::loadColorMap(const QString & filePath) {
    QFileInfo cssInfo(filePath);
    QString cMapPath = cssInfo.path() + "/" + cssInfo.completeBaseName() + ".cmap";
    QFileInfo cMapInfo(cMapPath);
    QString fileName = cssInfo.fileName();
    auto colorMap(createColorMapWithDefaults());
    if (cMapInfo.exists()) {
        QString group = darkMode()? "dark": "light";
        QSettings cMapSettings(cMapPath, QSettings::IniFormat);
        cMapSettings.beginGroup(group);
        QStringList colorKeys = cMapSettings.childKeys();
        for (QString colorKey:colorKeys) {
            QString value = cMapSettings.value(colorKey).toString();
            colorMap.insert(colorKey,value);
        }
    }
    m_colorMaps[fileName] = std::move(colorMap);
}

QString ColorManager::replaceColors(const QString& content) {
    QString activeTemplate = CDisplayTemplateMgr::activeTemplateName();
    ColorMap const & colorMap = m_colorMaps[activeTemplate];
    QString newContent = content;
    for (auto const & key : colorMap.keys()) {
        QString pattern = "#" + key + "#";
        auto value(colorMap.value(key));
        newContent = newContent.replace(pattern, value);
    }
    return newContent;
}

QString ColorManager::getColorByPattern(const QString& pattern, const QString& style) {
    QString activeTemplate;
    if (style.isEmpty())
        activeTemplate = CDisplayTemplateMgr::activeTemplateName();
    auto const it(m_colorMaps.find(activeTemplate));
    BT_ASSERT(it != m_colorMaps.end());
    ColorMap const & colorMap = *it;
    auto value(colorMap.value(pattern));
    BT_ASSERT(!value.isEmpty());
    return value;
}

QString ColorManager::getBackgroundColor(const QString& style) {
    return getColorByPattern("BACKGROUND_COLOR", style);
}

QString ColorManager::getForegroundColor(const QString& style) {
    return getColorByPattern("FOREGROUND_COLOR", style);
}

QString ColorManager::getCrossRefColor(const QString& style) {
    return getColorByPattern("CROSSREF_COLOR", style);
}


