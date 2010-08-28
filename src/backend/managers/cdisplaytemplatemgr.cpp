/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
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


CDisplayTemplateMgr *CDisplayTemplateMgr::m_instance = 0;

CDisplayTemplateMgr::CDisplayTemplateMgr() {
    namespace DU = util::directory;

    QStringList filter("*.tmpl");

    // Preload global display templates from disk:
    QDir td = DU::getDisplayTemplatesDir();
    Q_FOREACH(QString file, td.entryList(filter, QDir::Files | QDir::Readable))
        loadTemplate(td.canonicalPath() + "/" + file);

    /*
      Preload user display templates from disk, overriding any global templates
      with the same file name:
    */
    QDir utd = DU::getUserDisplayTemplatesDir();
    Q_FOREACH(QString file, utd.entryList(filter, QDir::Files | QDir::Readable))
        loadTemplate(utd.canonicalPath() + "/" + file);
}

const QString CDisplayTemplateMgr::fillTemplate( const QString& name, const QString& content, Settings& settings ) {
    qDebug() << "CDisplayTemplateMgr::fillTemplate";

    const QString templateName = m_templateMap.contains(name) ? name : defaultTemplate();

    QString displayTypeString;

    if (!settings.pageCSS_ID.isEmpty()) {
        displayTypeString = settings.pageCSS_ID;
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
        }
        else { //use bible as default type if no modules are set
            displayTypeString = "bible";
        };
    }

    QString newContent = content;
    const int moduleCount = settings.modules.count();

    if (moduleCount >= 2) {
        //create header for the modules
        qDebug() << "There were more than 1 module, create headers";
        QString header;

        QList<const CSwordModuleInfo*>::iterator end_it = settings.modules.end();

        for (QList<const CSwordModuleInfo*>::iterator it(settings.modules.begin()); it != end_it; ++it) {
            header.append("<th style=\"width:")
            .append(QString::number(int( 100.0 / (float)moduleCount )))
            .append("%;\">")
            .append((*it)->name())
            .append("</th>");
        }

        newContent = QString("<table><tr>")
                     .append(header)
                     .append("</tr>")
                     .append(content)
                     .append("</table>");
    }

    QString langCSS;
    CLanguageMgr::LangMap langMap = CLanguageMgr::instance()->availableLanguages();

    qDebug() << "langMap length:" << langMap.count();
    qDebug() << "loop through langMap";
    foreach(const CLanguageMgr::Language* lang, langMap) {
        //const CLanguageMgr::Language* lang = *it;
        //qDebug() << "foreach, lang: ";
        //qDebug() << lang;

        //if (lang->isValid() && CBTConfig::get(lang).first) {
        if (!lang->abbrev().isEmpty() && CBTConfig::get(lang).first) {
            const QFont f = CBTConfig::get(lang).second;

            //don't use important, because it would reset the title formatting, etc. to the setup font
            QString css("{ ");
            css.append("font-family:").append(f.family())/*.append(" !important")*/;
            css.append("; font-size:").append(QString::number(f.pointSize())).append("pt /*!important*/");
            css.append("; font-weight:").append(f.bold() ? "bold" : "normal /*!important*/");
            css.append("; font-style:").append(f.italic() ? "italic" : "normal /*!important*/");
            css.append("; }\n");

            langCSS +=
                QString("\n*[lang=%1] %2")
                .arg(lang->abbrev())
                .arg(css);
        }
    }

    //at first append the font standard settings for all languages without configured font
    // Create a dummy language (the langmap may be empty)
    CLanguageMgr::Language lang_v(QString("en"), QString("English"), QString::null);
    CLanguageMgr::Language* lang = &lang_v;

    if (lang && !lang->abbrev().isEmpty()/*&& lang->isValid()*/) {
        const QFont standardFont = CBTConfig::getDefault(lang); //we just need a dummy lang param
        langCSS.prepend(
            QString("\n#content {font-family:%1; font-size:%2pt; font-weight:%3; font-style: %4;}\n")
            .arg(standardFont.family())
            .arg(standardFont.pointSize())
            .arg(standardFont.bold() ? "bold" : "normal")
            .arg(standardFont.italic() ? "italic" : "normal")
        );
    }

//     qWarning("Outputing unformated text");
    const QString t = QString(m_templateMap[ templateName ]) //don't change the map's content directly, use  a copy
                      .replace("#TITLE#", settings.title)
                      .replace("#LANG_ABBREV#", settings.langAbbrev.isEmpty() ? QString("en") : settings.langAbbrev)
                      .replace("#DISPLAYTYPE#", displayTypeString)
                      .replace("#LANG_CSS#", langCSS)
                      .replace("#PAGE_DIRECTION#", settings.pageDirection)
                      .replace("#CONTENT#", newContent);

    return t;
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
