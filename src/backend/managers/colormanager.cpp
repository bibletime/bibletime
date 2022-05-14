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

#include "colormanager.h"

#include <QApplication>
#include <QColor>
#include <QDir>
#include <QFileInfo>
#include <QList>
#include <QPalette>
#include <QSettings>
#include <QStringList>
#include <QVariant>
#include <utility>
#include "../../util/btassert.h"
#include "../../util/directory.h"
#include "../../backend/managers/cdisplaytemplatemgr.h"
#include "../../backend/config/btconfig.h"


namespace {

inline bool darkMode()
{ return qApp->palette().color(QPalette::Base).value() < 128; }

QString getColorByPattern(
        std::map<QString, std::map<QString, QString> > const & maps,
        QString const & pattern,
        QString const & style)
{
    auto const activeTemplate =
            style.isEmpty() ? CDisplayTemplateMgr::activeTemplateName() : style;
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
    for (auto const & [key, value] : m_colorMaps[activeTemplate])
        content.replace(pattern.arg(key), value);
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

void ColorManager::initLightDarkPalette() {
    int lightDarkMode = btConfig().value<int>("GUI/lightDarkMode", 0);
    if (lightDarkMode == LightDarkMode::systemDefault)
        return;
    QPalette p;
    if (lightDarkMode == LightDarkMode::dark) {
        p.setColor(QPalette::WindowText,QColor(0xfc, 0xfc, 0xfc));
        p.setColor(QPalette::Button,QColor(0x31, 0x36, 0x3b));
        p.setColor(QPalette::Light,QColor(0x18, 0x1b, 0x1d));
        p.setColor(QPalette::Midlight,QColor(0x25, 0x29, 0x2c));
        p.setColor(QPalette::Dark,QColor(0x62, 0x6c, 0x76));
        p.setColor(QPalette::Mid,QColor(0x41, 0x48, 0x4e));
        p.setColor(QPalette::Text,QColor(0xfc, 0xfc, 0xfc));
        p.setColor(QPalette::BrightText,QColor(0xff, 0xff, 0xff));
        p.setColor(QPalette::ButtonText,QColor(0xfc, 0xfc, 0xfc));
        p.setColor(QPalette::Base,QColor(0x1b, 0x1e, 0x20));
        p.setColor(QPalette::Window,QColor(0x2a, 0x2e, 0x32));
        p.setColor(QPalette::Shadow,QColor(0x76, 0x76, 0x76));
        p.setColor(QPalette::Highlight,QColor(0x3d, 0xae, 0xe9));
        p.setColor(QPalette::HighlightedText,QColor(0xfc, 0xfc, 0xfc));
        p.setColor(QPalette::Link,QColor(0x1d, 0x99, 0xf3));
        p.setColor(QPalette::LinkVisited,QColor(0x9b, 0x59, 0xb6));
    } else {
        p.setColor(QPalette::WindowText,QColor(0x23, 0x26, 0x29));
        p.setColor(QPalette::Button,QColor(0xf7, 0xf7, 0xf7));
        p.setColor(QPalette::Light,QColor(0x0, 0x0, 0x0));
        p.setColor(QPalette::Midlight,QColor(0x0, 0x0, 0x0));
        p.setColor(QPalette::Dark,QColor(0x7b, 0x7b, 0x7b));
        p.setColor(QPalette::Mid,QColor(0xa5, 0xa5, 0xa5));
        p.setColor(QPalette::Text,QColor(0x23, 0x26, 0x29));
        p.setColor(QPalette::BrightText,QColor(0xff, 0xff, 0xff));
        p.setColor(QPalette::ButtonText,QColor(0x23, 0x26, 0x29));
        p.setColor(QPalette::Base,QColor(0xff, 0xff, 0xff));
        p.setColor(QPalette::Window,QColor(0xef, 0xf0, 0xf1));
        p.setColor(QPalette::Shadow,QColor(0x76, 0x76, 0x76));
        p.setColor(QPalette::Highlight,QColor(0x3d, 0xae, 0xe9));
        p.setColor(QPalette::HighlightedText,QColor(0xff, 0xff, 0xff));
        p.setColor(QPalette::Link,QColor(0x29, 0x80, 0xb9));
        p.setColor(QPalette::LinkVisited,QColor(0x9b, 0x59, 0xb6));
    }

    qApp->setPalette(p);
}
