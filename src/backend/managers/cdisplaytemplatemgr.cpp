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
#include "backend/config/cbtconfig.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/clanguagemgr.h"
#include "util/directory.h"


#define CSSTEMPLATEBASE "Basic.tmpl"

CDisplayTemplateMgr *CDisplayTemplateMgr::m_instance = 0;

CDisplayTemplateMgr::CDisplayTemplateMgr(QString &errorMessage) {
    Q_ASSERT(m_instance == 0);

    m_instance = this;
    namespace DU = util::directory;

    const QStringList filter("*.tmpl");
    const QStringList cssfilter("*.css");

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

    if (!m_templateMap.contains(CSSTEMPLATEBASE)) {
        errorMessage = QObject::tr("CSS base template not found!");
    } else if (!m_cssMap.contains(defaultTemplateName())) {
        errorMessage = QObject::tr("Default template \"%1\" not found!")
                       .arg(defaultTemplateName());
    } else {
        errorMessage = QString::null;
    }
}

QStringList CDisplayTemplateMgr::availableTemplates() const {
    QStringList r = m_templateMap.keys();
    const bool b = r.removeOne(CSSTEMPLATEBASE);
    Q_ASSERT(b);
    r.append(m_cssMap.keys());
    qSort(r);
    return r;
}

QString CDisplayTemplateMgr::fillTemplate(const QString &name,
                                          const QString &content,
                                          const Settings &settings)
{
    Q_ASSERT(name != CSSTEMPLATEBASE);
    const bool templateIsCss = !m_templateMap.contains(name);

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
    {
        /*
          At first append the font standard settings for all languages without
          configured font. Create a dummy language (the langmap may be empty).
        */
        const CLanguageMgr::Language lang(QString("en"), QString("English"), QString::null);
        const QFont f = CBTConfig::getDefault(&lang);
        langCSS.append("#content{font-family:").append(f.family())
               .append(";font-size:").append(QString::number(f.pointSizeF(), 'f'))
               .append("pt;font-weight:").append(f.bold() ? "bold" : "normal")
               .append(";font-style:").append(f.italic() ? "italic" : "normal")
               .append('}');
    }
    {
        const CLanguageMgr::LangMap & langMap = CLanguageMgr::instance()->availableLanguages();
        Q_FOREACH (const CLanguageMgr::Language * lang, langMap) {
            if (!lang->abbrev().isEmpty() && CBTConfig::get(lang).first) {
                const QFont f = CBTConfig::get(lang).second;

                langCSS.append("*[lang=").append(lang->abbrev()).append("]{")
                       .append("font-family:").append(f.family())
                       .append(";font-size:").append(QString::number(f.pointSizeF(), 'f'))
                       .append("pt;font-weight:").append(f.bold() ? "bold" : "normal")
                       .append(";font-style:").append(f.italic() ? "italic" : "normal")
                       .append('}');
            }
        }
    }

    namespace DU = util::directory;
    QString output(m_templateMap[templateIsCss
              ? QString(CSSTEMPLATEBASE)
              : name]); // don't change the map's content directly, use a copy
    output.replace("#TITLE#", settings.title)
          .replace("#LANG_ABBREV#", settings.langAbbrev)
          .replace("#DISPLAYTYPE#", displayTypeString)
          .replace("#LANG_CSS#", langCSS)
          .replace("#PAGE_DIRECTION#", settings.textDirectionAsHtmlDirAttr())
          .replace("#CONTENT#", newContent)
          .replace("#MODTYPE#", displayTypeString)
          .replace("#MODNAME#", moduleName)
          .replace("#DISPLAY_TEMPLATES_PATH#", DU::getDisplayTemplatesDir().absolutePath());

    if (templateIsCss)
        output.replace("#THEME_STYLE#", m_cssMap[name]);

    return output;
}

QString CDisplayTemplateMgr::activeTemplateName() {
    const QString tn = CBTConfig::get(CBTConfig::displayStyle);
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
