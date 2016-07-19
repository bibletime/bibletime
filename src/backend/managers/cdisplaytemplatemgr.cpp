/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "cdisplaytemplatemgr.h"

#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QTextStream>
#include "../../util/directory.h"
#include "../config/btconfig.h"
#include "../drivers/cswordmoduleinfo.h"
#include "clanguagemgr.h"


#define CSSTEMPLATEBASE "Basic.tmpl"

namespace {

inline QString readFileToString(const QString & filename) {
    QFile f(filename);
    return f.open(QIODevice::ReadOnly) ? QTextStream(&f).readAll() : QString();
}

} // anonymous namespace

CDisplayTemplateMgr * CDisplayTemplateMgr::m_instance = nullptr;

CDisplayTemplateMgr::CDisplayTemplateMgr(QString & errorMessage) {
    BT_ASSERT(!m_instance);
    m_instance = this;

    {
        namespace DU = util::directory;
        const QDir::Filters readableFileFilter(QDir::Files | QDir::Readable);
        const QDir & td = DU::getDisplayTemplatesDir(); // Global template directory
        const QDir & utd = DU::getUserDisplayTemplatesDir(); // User template directory

        // Load regular templates:
        {
            const QStringList filter("*.tmpl");
            // Preload global display templates from disk:
            Q_FOREACH(const QString & file, td.entryList(filter, readableFileFilter))
                loadTemplate(td.canonicalPath() + "/" + file);
            // Preload user display templates from disk:
            Q_FOREACH(const QString & file, utd.entryList(filter, readableFileFilter))
                loadTemplate(utd.canonicalPath() + "/" + file);
        }

        if (!m_templateMap.contains(CSSTEMPLATEBASE)) {
            errorMessage = QObject::tr("CSS base template not found!");
            return;
        }

        // Load CSS templates:
        {
            const QStringList cssfilter("*.css");
            // Load global app stylesheets
            Q_FOREACH(const QString & file, td.entryList(cssfilter, readableFileFilter))
                loadCSSTemplate(td.canonicalPath() + "/" + file);
            // Load user app stylesheets
            Q_FOREACH(const QString & file, utd.entryList(cssfilter, readableFileFilter))
                loadCSSTemplate(utd.canonicalPath() + "/" + file);
        }
    }

    if (!m_cssMap.contains(defaultTemplateName())) {
        errorMessage = QObject::tr("Default template \"%1\" not found!")
                       .arg(defaultTemplateName());
        return;
    }

    // Create template names cache:
    m_availableTemplateNamesCache = m_templateMap.keys();
    const bool b = m_availableTemplateNamesCache.removeOne(CSSTEMPLATEBASE);
    BT_ASSERT(b);
    m_availableTemplateNamesCache.append(m_cssMap.keys());
    qSort(m_availableTemplateNamesCache);

    errorMessage = QString::null;
}

QString CDisplayTemplateMgr::fillTemplate(const QString & name,
                                          const QString & content,
                                          const Settings & settings) const
{
    BT_ASSERT(name != CSSTEMPLATEBASE);
    BT_ASSERT(name.endsWith(".css") || name.endsWith(".tmpl"));
    BT_ASSERT(!name.endsWith(".css") || m_cssMap.contains(name));
    BT_ASSERT(!name.endsWith(".tmpl") || m_templateMap.contains(name));
    const bool templateIsCss = name.endsWith(".css");

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
        // qDebug() << "There were more than 1 module, create headers";
        QString header;

        Q_FOREACH(const CSwordModuleInfo * const mi, settings.modules) {
            header.append("<th style=\"width:")
            .append(QString::number(static_cast<int>(100.0 / moduleCount)))
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
        const QFont & f = btConfig().getDefaultFont();
        langCSS.append("#content{font-family:").append(f.family())
               .append(";font-size:").append(QString::number(f.pointSizeF(), 'f'))
               .append("pt;font-weight:").append(f.bold() ? "bold" : "normal")
               .append(";font-style:").append(f.italic() ? "italic" : "normal")
               .append('}');
    }
    {
        const CLanguageMgr::LangMap & langMap = CLanguageMgr::instance()->availableLanguages();
        Q_FOREACH(const CLanguageMgr::Language * const lang, langMap) {
            if (lang->abbrev().isEmpty())
                continue;

            BtConfig::FontSettingsPair fp = btConfig().getFontForLanguage(*lang);
            if (fp.first) {
                const QFont & f = fp.second;

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
        output.replace("#THEME_STYLE#", readFileToString(m_cssMap[name]));

    return output;
}

QString CDisplayTemplateMgr::activeTemplateName() {
    const QString tn = btConfig().value<QString>("GUI/activeTemplateName",
                                                 QString());
    return (tn.isEmpty()
            || !instance()->m_availableTemplateNamesCache.contains(tn))
           ? defaultTemplateName()
           : tn;
}

void CDisplayTemplateMgr::loadTemplate(const QString & filename) {
    BT_ASSERT(filename.endsWith(".tmpl"));
    BT_ASSERT(QFileInfo(filename).isFile());
    const QString templateString = readFileToString(filename);
    if (!templateString.isEmpty())
        m_templateMap.insert(QFileInfo(filename).fileName(), templateString);
}

void CDisplayTemplateMgr::loadCSSTemplate(const QString & filename) {
    BT_ASSERT(filename.endsWith(".css"));
    const QFileInfo fi(filename);
    BT_ASSERT(fi.isFile());
    if (fi.isReadable())
        m_cssMap.insert(fi.fileName(), filename);
}
