
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


namespace {

inline bool darkMode()
{ return qApp->palette().color(QPalette::Base).value() < 128; }

QString getColorByPattern(
        std::map<QString, std::map<QString, QString> > const & maps,
        QString const & pattern,
        QString const & style)
{
    QString activeTemplate;
    if (style.isEmpty())
        activeTemplate = CDisplayTemplateMgr::activeTemplateName();
    auto const mapIt(maps.find(activeTemplate));
    BT_ASSERT(mapIt != maps.end());
    auto const valueIt(mapIt->second.find(pattern));
    BT_ASSERT(valueIt != mapIt->second.end());
    BT_ASSERT(!valueIt->second.isEmpty());
    return valueIt->second;
}

} // anonymous namespace

ColorManager & ColorManager::instance() {
    static ColorManager r;
    return r;
}

ColorManager::ColorManager() = default;


void ColorManager::loadColorMaps() {
    namespace DU = util::directory;
    QDir::Filters const readableFileFilter(QDir::Files | QDir::Readable);
    QStringList const cssFilter("*.css");

    auto const loadColorMap =
            [this](QString const & filePath) {
                QFileInfo const cssInfo(filePath);
                static QString const cMapPathTemplate("%1/%2.cmap");
                auto cMapPath(cMapPathTemplate.arg(cssInfo.path())
                                              .arg(cssInfo.completeBaseName()));
                QFileInfo const cMapInfo(cMapPath);
                auto fileName(cssInfo.fileName());

                // Start with default color map:
                std::map<QString, QString> colorMap;
                auto const p(qApp->palette());
                if (darkMode()) {
                    colorMap.emplace("FOREGROUND_COLOR",
                                     p.color(QPalette::WindowText).name());
                    colorMap.emplace("BACKGROUND_COLOR",
                                     p.color(QPalette::Base).name());
                    colorMap.emplace("HIGHLIGHT",
                                     QColor("#ffff00").name());
                    colorMap.emplace("BACKGROUND_HIGHLIGHT",
                                     QColor("#444466").name());
                    colorMap.emplace("CROSSREF_COLOR",
                                     QColor("#aac2ff").name());
                    colorMap.emplace("JESUS_WORDS_COLOR",
                                     QColor("#ff0000").name());
                } else {
                    colorMap.emplace("FOREGROUND_COLOR",
                                     p.color(QPalette::WindowText).name());
                    colorMap.emplace("BACKGROUND_COLOR",
                                     p.color(QPalette::Base).name());
                    colorMap.emplace("HIGHLIGHT",
                                     QColor("#ffff00").name());
                    colorMap.emplace("BACKGROUND_HIGHLIGHT",
                                     QColor("#ddddff").name());
                    colorMap.emplace("CROSSREF_COLOR",
                                     QColor("#1414ff").name());
                    colorMap.emplace("JESUS_WORDS_COLOR",
                                     QColor("#ff0000").name());
                }

                if (cMapInfo.exists()) {
                    QSettings cMapSettings(cMapPath, QSettings::IniFormat);
                    static QString const dark("dark");
                    static QString const light("light");
                    cMapSettings.beginGroup(darkMode() ? dark : light);
                    for (auto const & colorKey : cMapSettings.childKeys())
                        colorMap[colorKey] = cMapSettings.value(colorKey).toString();
                }
                m_colorMaps[std::move(fileName)] = std::move(colorMap);
            };

    // Load global app stylesheets
    auto const & td = DU::getDisplayTemplatesDir();
    for (auto const & file : td.entryList(cssFilter, readableFileFilter))
        loadColorMap(td.canonicalPath() + "/" + file);

    // Load user app stylesheets
    auto const & utd = DU::getUserDisplayTemplatesDir();
    for (auto const & file : utd.entryList(cssFilter, readableFileFilter))
        loadColorMap(utd.canonicalPath() + "/" + file);
}

QString ColorManager::replaceColors(QString content) {
    auto const activeTemplate(CDisplayTemplateMgr::activeTemplateName());
    static QString const pattern("#%1#");
    for (auto const & cp : m_colorMaps[activeTemplate])
        content.replace(pattern.arg(cp.first), cp.second);
    return content;
}

QString ColorManager::getBackgroundColor(QString const & style)
{ return getColorByPattern(m_colorMaps, "BACKGROUND_COLOR", style); }

QString ColorManager::getBackgroundHighlightColor(QString const & style)
{ return getColorByPattern(m_colorMaps, "BACKGROUND_HIGHLIGHT", style); }

QString ColorManager::getForegroundColor(QString const & style)
{ return getColorByPattern(m_colorMaps, "FOREGROUND_COLOR", style); }

QString ColorManager::getCrossRefColor(QString const & style)
{ return getColorByPattern(m_colorMaps, "CROSSREF_COLOR", style); }
