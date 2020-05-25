
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

ColorManager::ColorManager() = default;

bool ColorManager::darkMode() const
{ return qApp->palette().color(QPalette::Base).value() < 128; }

void ColorManager::loadColorMaps() {
    namespace DU = util::directory;
    QDir::Filters const readableFileFilter(QDir::Files | QDir::Readable);
    QStringList const cssFilter("*.css");

    // Load global app stylesheets
    auto const & td = DU::getDisplayTemplatesDir();
    for (auto const & file : td.entryList(cssFilter, readableFileFilter))
        loadColorMap(td.canonicalPath() + "/" + file);

    // Load user app stylesheets
    auto const & utd = DU::getUserDisplayTemplatesDir();
    for (auto const & file : utd.entryList(cssFilter, readableFileFilter))
        loadColorMap(utd.canonicalPath() + "/" + file);
}

ColorManager::ColorMap ColorManager::createColorMapWithDefaults(){
    ColorMap colorDefs;
    if (darkMode()) {
        colorDefs.emplace("FOREGROUND_COLOR", qApp->palette().color(QPalette::WindowText).name());
        colorDefs.emplace("BACKGROUND_COLOR", qApp->palette().color(QPalette::Base).name());
        colorDefs.emplace("HIGHLIGHT_COLOR",  QColor("#ffff00").name());
        colorDefs.emplace("CROSSREF_COLOR",  QColor("#aac2ff").name());
        colorDefs.emplace("JESUS_WORDS_COLOR",QColor("#ff0000").name());
    } else {
        colorDefs.emplace("FOREGROUND_COLOR", qApp->palette().color(QPalette::WindowText).name());
        colorDefs.emplace("BACKGROUND_COLOR", qApp->palette().color(QPalette::Base).name());
        colorDefs.emplace("HIGHLIGHT_COLOR",  QColor("#ffff00").name());
        colorDefs.emplace("CROSSREF_COLOR",  QColor("#1414ff").name());
        colorDefs.emplace("JESUS_WORDS_COLOR",QColor("#ff0000").name());
    }
    return colorDefs;
}

void ColorManager::loadColorMap(QString const & filePath) {
    QFileInfo const cssInfo(filePath);
    auto cMapPath(cssInfo.path() + "/" + cssInfo.completeBaseName() + ".cmap");
    QFileInfo const cMapInfo(cMapPath);
    auto fileName(cssInfo.fileName());
    auto colorMap(createColorMapWithDefaults());
    if (cMapInfo.exists()) {
        QSettings cMapSettings(cMapPath, QSettings::IniFormat);
        static QString const dark("dark");
        static QString const light("light");
        cMapSettings.beginGroup(darkMode() ? dark : light);
        for (auto const & colorKey : cMapSettings.childKeys())
            colorMap.emplace(colorKey, cMapSettings.value(colorKey).toString());
    }
    m_colorMaps[std::move(fileName)] = std::move(colorMap);
}

QString ColorManager::replaceColors(QString content) {
    auto const activeTemplate(CDisplayTemplateMgr::activeTemplateName());
    static QString const pattern("#%1#");
    for (auto const & cp : m_colorMaps[activeTemplate])
        content.replace(pattern.arg(cp.first), cp.second);
    return content;
}

QString ColorManager::getColorByPattern(QString const & pattern,
                                        QString const & style)
{
    QString activeTemplate;
    if (style.isEmpty())
        activeTemplate = CDisplayTemplateMgr::activeTemplateName();
    auto const mapIt(m_colorMaps.find(activeTemplate));
    BT_ASSERT(mapIt != m_colorMaps.end());
    auto const valueIt(mapIt->second.find(pattern));
    BT_ASSERT(valueIt != mapIt->second.end());
    BT_ASSERT(!valueIt->second.isEmpty());
    return valueIt->second;
}

QString ColorManager::getBackgroundColor(QString const & style)
{ return getColorByPattern("BACKGROUND_COLOR", style); }

QString ColorManager::getForegroundColor(QString const & style)
{ return getColorByPattern("FOREGROUND_COLOR", style); }

QString ColorManager::getCrossRefColor(QString const & style)
{ return getColorByPattern("CROSSREF_COLOR", style); }
