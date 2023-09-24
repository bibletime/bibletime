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

#include "cdisplaytemplatemgr.h"

#include <algorithm>
#include <memory>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFont>
#include <QIODevice>
#include <QObject>
#include <QStringList>
#include <QTextStream>
#include <set>
#include "../../util/btassert.h"
#include "../../util/directory.h"
#include "../config/btconfig.h"
#include "../drivers/btmodulelist.h"
#include "../drivers/cswordmoduleinfo.h"
#include "../language.h"
#include "cswordbackend.h"


#define CSSTEMPLATEBASE "Basic.tmpl"

namespace {

inline QString readFileToString(const QString & filename) {
    QFile f(filename);
    return f.open(QIODevice::ReadOnly) ? QTextStream(&f).readAll() : QString();
}

auto const defaultTemplateName = QStringLiteral("Blue.css");

} // anonymous namespace

CDisplayTemplateMgr * CDisplayTemplateMgr::m_instance = nullptr;

CDisplayTemplateMgr::CDisplayTemplateMgr(QString & errorMessage) :
    m_multiModuleHeaders(true) {
    BT_ASSERT(!m_instance);
    m_instance = this;

    {
        namespace DU = util::directory;
        const QDir::Filters readableFileFilter(QDir::Files | QDir::Readable);
        const QDir & td = DU::getDisplayTemplatesDir(); // Global template directory
        const QDir & utd = DU::getUserDisplayTemplatesDir(); // User template directory

        // Load regular templates:
        {
            const QStringList filter(QStringLiteral("*.tmpl"));
            // Preload global display templates from disk:
            for (auto const & file : td.entryList(filter, readableFileFilter))
                loadTemplate(td.canonicalPath() + '/' + file);
            // Preload user display templates from disk:
            for (auto const & file : utd.entryList(filter, readableFileFilter))
                loadTemplate(utd.canonicalPath() + '/' + file);
        }

        if (!m_templateMap.contains(CSSTEMPLATEBASE)) {
            errorMessage = QObject::tr("CSS base template not found!");
            return;
        }

        // Load CSS templates:
        {
            const QStringList cssfilter(QStringLiteral("*.css"));
            // Load global app stylesheets
            for (auto const & file : td.entryList(cssfilter, readableFileFilter))
                loadCSSTemplate(td.canonicalPath() + '/' + file);
            // Load user app stylesheets
            for (auto const & file : utd.entryList(cssfilter, readableFileFilter))
                loadCSSTemplate(utd.canonicalPath() + '/' + file);
        }
    }

    if (!m_cssMap.contains(defaultTemplateName)) {
        errorMessage = QObject::tr("Default template \"%1\" not found!")
                       .arg(defaultTemplateName);
        return;
    }

    // Create template names cache:
    m_availableTemplateNamesCache = m_templateMap.keys();
    const bool b = m_availableTemplateNamesCache.removeOne(CSSTEMPLATEBASE);
    BT_ASSERT(b);
    m_availableTemplateNamesCache.append(m_cssMap.keys());
    std::sort(m_availableTemplateNamesCache.begin(),
              m_availableTemplateNamesCache.end());

    errorMessage = QString();
}

QString CDisplayTemplateMgr::fillTemplate(const QString & name,
                                          const QString & content,
                                          const Settings & settings) const
{
    BT_ASSERT(name != CSSTEMPLATEBASE);
    BT_ASSERT(name.endsWith(QStringLiteral(".css"))
              || name.endsWith(QStringLiteral(".tmpl")));
    BT_ASSERT(!name.endsWith(QStringLiteral(".css"))
              || m_cssMap.contains(name));
    BT_ASSERT(!name.endsWith(QStringLiteral(".tmpl"))
              || m_templateMap.contains(name));
    const bool templateIsCss = name.endsWith(QStringLiteral(".css"));

    QString displayTypeString;
    QString moduleName;

    if (!settings.pageCSS_ID.isEmpty()) {
        displayTypeString = settings.pageCSS_ID;
    }
    else {
        if (settings.modules.count()) {
            switch (settings.modules.first()->type()) {

                case CSwordModuleInfo::Bible:
                    displayTypeString = QStringLiteral("bible");
                    break;

                case CSwordModuleInfo::GenericBook:
                    displayTypeString = QStringLiteral("book");
                    break;

                case CSwordModuleInfo::Commentary:
                case CSwordModuleInfo::Lexicon:
                default:
                    displayTypeString = QStringLiteral("singleentry");
                    break;
            }
            moduleName = settings.modules.first()->name();
        }
        else { //use bible as default type if no modules are set
            displayTypeString = QStringLiteral("bible");
        }
    }

    QString newContent = content;
    const int moduleCount = settings.modules.count();

    if (moduleCount >= 2 && m_multiModuleHeaders) {
        //create header for the modules
        // qDebug() << "There were more than 1 module, create headers";
        QString header;

        for (auto const * const mi : settings.modules)
            header.append(
                        QStringLiteral("<th style=\"width:%1%;\">%2</th>")
                        .arg(QString::number(static_cast<int>(100.0 / moduleCount)),
                             mi->name()));

        newContent = QStringLiteral("<table><tr>%1</tr>%2</table>")
                     .arg(header, content);
    }

    QString langCSS;
    {
        const QFont & f = btConfig().getDefaultFont();
        langCSS.append(
                    QStringLiteral("#content{font-family:%1;font-size:%2pt;"
                                   "font-weight:%3;font-style:%4}")
                    .arg(f.family(),
                         QString::number(f.pointSizeF(), 'f'),
                         f.bold()
                         ? QStringLiteral("bold")
                         : QStringLiteral("normal"),
                         f.italic()
                         ? QStringLiteral("italic")
                         : QStringLiteral("normal")));
    }
    {
        auto const availableLanguages =
                CSwordBackend::instance().availableLanguages();
        BT_ASSERT(availableLanguages);
        for (auto const & lang : *availableLanguages) {
            if (lang->abbrev().isEmpty())
                continue;

            BtConfig::FontSettingsPair fp = btConfig().getFontForLanguage(*lang);
            if (fp.first) {
                const QFont & f = fp.second;

                /* QFont::weight() returns an int in the range [0, 99] but CSS
                   requires a real number in the range [1, 1000]. No extra
                   checks are needed for floating point precision in the result
                   range. */
                auto const fontWeight = 1.0 + (f.weight() * 999.0) / 99.0;

                auto const fontStyleString =
                        [&f]() {
                            switch ((int) f.style()) {
                            case QFont::StyleItalic:
                                return QStringLiteral("italic");
                            case QFont::StyleOblique:
                                return QStringLiteral("oblique");
                            case QFont::StyleNormal:
                            default:
                                return QStringLiteral("normal");
                            }
                        }();

                auto const textDecorationString =
                        f.underline()
                        ? (f.strikeOut()
                           ? QStringLiteral("underline line-through")
                           : QStringLiteral("underline"))
                        : (f.strikeOut()
                           ? QStringLiteral("line-through")
                           : QStringLiteral("none"));

                /// \todo Add support translating more QFont properties to CSS.

                langCSS.append(
                            QStringLiteral("*[lang=%1]{font-family:%2;"
                                           "font-size:%3pt;font-weight:%4;"
                                           "font-style:%5;text-decoration:%6}")
                            .arg(lang->abbrev(),
                                 f.family(),
                                 QString::number(f.pointSizeF(), 'f'),
                                 QString::number(fontWeight),
                                 fontStyleString,
                                 textDecorationString));
            }
        }
    }

    namespace DU = util::directory;
    QString output(m_templateMap[templateIsCss
                                 ? QString(CSSTEMPLATEBASE)
                                 : name]); // don't change the map's content directly, use a copy
    output.replace(QStringLiteral("#TITLE#"), settings.title)
          .replace(QStringLiteral("#LANG_ABBREV#"), settings.langAbbrev)
          .replace(QStringLiteral("#DISPLAYTYPE#"), displayTypeString)
          .replace(QStringLiteral("#LANG_CSS#"), langCSS)
          .replace(QStringLiteral("#PAGE_DIRECTION#"),
                   settings.textDirectionAsHtmlDirAttr())
          .replace(QStringLiteral("#CONTENT#"), newContent)
          .replace(QStringLiteral("#BODY_CLASSES#"),
                   QStringLiteral("%1 %1_%2").arg(displayTypeString,
                                                  moduleName))
          .replace(QStringLiteral("#DISPLAY_TEMPLATES_PATH#"),
                   DU::getDisplayTemplatesDir().absolutePath());

    if (templateIsCss)
        output.replace(QStringLiteral("#THEME_STYLE#"), m_cssMap[name]);

    return output;
}

QString CDisplayTemplateMgr::activeTemplateName() {
    auto const tn =
            btConfig().value<QString>(QStringLiteral("GUI/activeTemplateName"),
                                      QString());
    return (tn.isEmpty()
            || !instance()->m_availableTemplateNamesCache.contains(tn))
           ? defaultTemplateName
           : tn;
}

void CDisplayTemplateMgr::loadTemplate(const QString & filename) {
    BT_ASSERT(filename.endsWith(QStringLiteral(".tmpl")));
    BT_ASSERT(QFileInfo(filename).isFile());
    const QString templateString = readFileToString(filename);
    if (!templateString.isEmpty())
        m_templateMap.insert(QFileInfo(filename).fileName(), templateString);
}

void CDisplayTemplateMgr::loadCSSTemplate(const QString & filename) {
    BT_ASSERT(filename.endsWith(QStringLiteral(".css")));
    const QFileInfo fi(filename);
    BT_ASSERT(fi.isFile());
    if (fi.isReadable())
        m_cssMap.insert(fi.fileName(), readFileToString(filename));
}

void CDisplayTemplateMgr::setMultiModuleHeadersVisible(bool visible) {
    m_multiModuleHeaders = visible;
}
