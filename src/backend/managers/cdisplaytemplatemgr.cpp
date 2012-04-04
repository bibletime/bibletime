/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/managers/cdisplaytemplatemgr.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QTextStream>
#include "backend/config/btconfig.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/clanguagemgr.h"
#include "util/directory.h"


CDisplayTemplateMgr *CDisplayTemplateMgr::m_instance = 0;

CDisplayTemplateMgr::CDisplayTemplateMgr(QString &errorMessage) {
    Q_ASSERT(m_instance == 0);

    m_instance = this;
    namespace DU = util::directory;

    QStringList filter("*.tmpl");
    QStringList cssfilter("*.css");

    // Preload global display templates from disk:
    QDir td = DU::getDisplayTemplatesDir();
    Q_FOREACH(const QString &file, td.entryList(filter, QDir::Files | QDir::Readable))
        loadTemplate(td.canonicalPath() + "/" + file);
        
    // Load app stylesheets
    Q_FOREACH(const QString &file, td.entryList(cssfilter, QDir::Files | QDir::Readable))
    	loadCSSTemplate(td.canonicalPath() + "/" + file);

    /*
      Preload user display templates from disk, overriding any global templates
      with the same file name:
    */
    QDir utd = DU::getUserDisplayTemplatesDir();
    Q_FOREACH(const QString &file, utd.entryList(filter, QDir::Files | QDir::Readable))
        loadTemplate(utd.canonicalPath() + "/" + file);

    if (m_cssMap.contains(defaultTemplateName())) {
        errorMessage = QString::null;
    } else {
        errorMessage = QObject::tr("Default template \"%1\" not found!")
                       .arg(defaultTemplateName());
    }
}

QString CDisplayTemplateMgr::fillTemplate(const QString &name,
                                          const QString &content,
                                          const Settings &settings)
{
    const QString templateName = m_cssMap.contains(name) ? name : defaultTemplateName();

    QString displayTypeString;
    QString moduleName;

    if (!settings.pageCSS_ID.isEmpty()) {
        displayTypeString = settings.pageCSS_ID;
        moduleName = "";
    }
    else {
        if (settings.modules.count()) {
            switch (settings.modules.first()->type()) {

                case CSwordModuleInfo::Bible:
                    displayTypeString = "bible";
                    break;

                case CSwordModuleInfo::GenericBook:
                    displayTypeString = "book";
                    break;

                case CSwordModuleInfo::Commentary:
                case CSwordModuleInfo::Lexicon:
                default:
                    displayTypeString = "singleentry";
                    break;
            };
            moduleName = settings.modules.first()->name();
        }
        else { //use bible as default type if no modules are set
            displayTypeString = "bible";
            moduleName = "";
        };
    }

    QString newContent = content;
    const int moduleCount = settings.modules.count();

    if (moduleCount >= 2) {
        //create header for the modules
        qDebug() << "There were more than 1 module, create headers";
        QString header;

        Q_FOREACH(const CSwordModuleInfo *mi, settings.modules) {
            header.append("<th style=\"width:")
            .append(QString::number(int( 100.0 / (float)moduleCount )))
            .append("%;\">")
            .append(mi->name())
            .append("</th>");
        }

        newContent = QString("<table><tr>")
                     .append(header)
                     .append("</tr>")
                     .append(content)
                     .append("</table>");
    }

    QString langCSS;
    const CLanguageMgr::LangMap & langMap = CLanguageMgr::instance()->availableLanguages();

    static const QString langStyleString = "*[lang=%1]{font-family:%2;"
                                           "font-size:%3pt;font-weight:%4;"
                                           "font-style:%5}";
    Q_FOREACH (const CLanguageMgr::Language * const lang, langMap) {
        if (lang->abbrev().isEmpty())
            continue;

        BtConfig::FontSettingsPair fp = btConfig().getFontForLanguage(*lang);
        if (!fp.first)
            continue;

        const QFont & f = fp.second;

        langCSS.append(langStyleString.arg(lang->abbrev(),
                                           f.family(),
                                           QString::number(f.pointSize()),
                                           f.bold() ? "bold" : "normal",
                                           f.italic() ? "italic" : "normal"));
    }
    const QFont & defaultFont = btConfig().getDefaultFont();
    static const QString contentStyleString = "#content{font-family:%1;"
                                              "font-size:%2pt;font-weight:%3;"
                                              "font-style:%4;}";
    langCSS.prepend(contentStyleString.arg(
            defaultFont.family(),
            QString::number(defaultFont.pointSize()),
            defaultFont.bold() ? "bold" : "normal",
            defaultFont.italic() ? "italic" : "normal"));

    // Template stylesheet


//     qWarning("Outputing unformated text");
    const QString t = QString(m_templateMap[ "Basic.tmpl" ]) //don't change the map's content directly, use  a copy
                      .replace("#TITLE#", settings.title)
                      .replace("#LANG_ABBREV#", settings.langAbbrev.isEmpty() ? QString("en") : settings.langAbbrev)
                      .replace("#DISPLAYTYPE#", displayTypeString)
                      .replace("#LANG_CSS#", langCSS)
                      .replace("#PAGE_DIRECTION#", settings.pageDirection)
                      .replace("#CONTENT#", newContent)
                      .replace("#THEME_STYLE#", m_cssMap[ templateName ])
                      .replace("#MODTYPE#", displayTypeString)
                      .replace("#MODNAME#", moduleName)
                      .replace("#MODULE_STYLESHEET#", QString(""));	// Let's fix this!

    return t;
}

QString CDisplayTemplateMgr::activeTemplateName() {
    const QString tn = btConfig().value<QString>("GUI/activeTemplateName", QString());
    if (tn.isEmpty())
        return defaultTemplateName();

    return tn;
}

void CDisplayTemplateMgr::loadTemplate(const QString &filename) {
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly)) {
        QString fileContent = QTextStream(&f).readAll();

        if (!fileContent.isEmpty()) {
            m_templateMap[QFileInfo(f).fileName()] = fileContent;
        }
    }
}

void CDisplayTemplateMgr::loadCSSTemplate(const QString &filename) {
    QFile f(filename);
    m_cssMap[QFileInfo(f).fileName()] = QString("file://") + filename;
}
