//
// C++ Implementation: cdisplaytemplatemgr
//
// Description:
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

//BibleTime includes
#include "cdisplaytemplatemgr.h"
#include "cswordmoduleinfo.h"
#include "clanguagemgr.h"

#include "frontend/cbtconfig.h"

#include "util/cpointers.h"

#include "config.h"

//Qt includes
#include <qstringlist.h>
#include <qfile.h>
#include <qfileinfo.h>

//KDE includes
#include <klocale.h>
#include <kstandarddirs.h>

#include <iostream>

CDisplayTemplateMgr::CDisplayTemplateMgr() {
	init();
	loadUserTemplates();
}

CDisplayTemplateMgr::~CDisplayTemplateMgr() {}


/*!
    \fn CDisplayTemplateMgr::fillTemplate( const QString& name, const QString& title, const QString& content )
 */
const QString CDisplayTemplateMgr::fillTemplate( const QString& name, const QString& content, Settings& settings ) {
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
		QString header;

		ListCSwordModuleInfo::iterator end_it = settings.modules.end();

		for (ListCSwordModuleInfo::iterator it(settings.modules.begin()); it != end_it; ++it) {
			header.append("<th style=\"width:")
			.append(QString::number(int( 100.0 / (float)moduleCount )))
			.append("%;\">")
			.append((*it)->name())
			.append("</th>");
		}

		newContent.setLatin1("<table><tr>")
		.append(header)
		.append("</tr>")
		.append(content)
		.append("</table>");
	}

	QString langCSS;
	CLanguageMgr::LangMap langMap = CPointers::languageMgr()->availableLanguages();

	for ( CLanguageMgr::LangMapIterator it( langMap ); it.current(); ++it ) {
		const CLanguageMgr::Language* lang = it.current();


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
	CLanguageMgr::LangMapIterator it( langMap );

	const CLanguageMgr::Language* lang = it.current();

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

// 	qWarning("Outputing unformated text");
	const QString t = QString(m_templateMap[ templateName ]) //don't change the map's content directly, use  a copy
	 		.replace("#TITLE#", settings.title)
	 		.replace("#LANG_ABBREV#", settings.langAbbrev.isEmpty() ? QString("en") : settings.langAbbrev)
	 		.replace("#DISPLAYTYPE#", displayTypeString)
	 		.replace("#LANG_CSS#", langCSS)
	 		.replace("#PAGE_DIRECTION#", settings.pageDirection)
	 		.replace("#CONTENT#", newContent);
 	 //printf("%s\n\n", t.latin1());

	return t;
/*			QString(m_templateMap[ templateName ]) //don't change the map's content directly, use  a copy
		   .replace("#TITLE#", settings.title)
		   .replace("#LANG_ABBREV#", settings.langAbbrev.isEmpty() ? QString("en") : settings.langAbbrev)
		   .replace("#DISPLAYTYPE#", displayTypeString)
		   .replace("#LANG_CSS#", langCSS)
		   .replace("#PAGE_DIRECTION#", settings.pageDirection)
		   .replace("#CONTENT#", newContent);*/
}


/*!
    \fn CDisplayTemplateMgr::loadUserTemplates
 */
void CDisplayTemplateMgr::loadUserTemplates() {
	qDebug("Loading user templates");
	QStringList files = KGlobal::dirs()->findAllResources("BT_DisplayTemplates");

	for ( QStringList::iterator it( files.begin() ); it != files.end(); ++it) {
		qDebug("Found user template %s", (*it).latin1());

		QFile f( *it );
		Q_ASSERT( f.exists() );

		if (f.open( IO_ReadOnly )) {
			QString fileContent = QTextStream( &f ).read();

			if (!fileContent.isEmpty()) {
				m_templateMap[ QFileInfo(*it).fileName() + QString(" ") + i18n("(user template)")] = fileContent;
			}
		}
	}
}

//Include the HTML templates which were put into a cpp file by a Perl script
#include "../display-templates/template-init.cpp"
