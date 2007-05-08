//
// C++ Implementation: cdisplayrendering
//
// Description:
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

//Backend includes
#include "cdisplayrendering.h"

#include "cdisplaytemplatemgr.h"
#include "creferencemanager.h"
#include "cswordkey.h"
#include "cswordversekey.h"

//Qt includes
#include <qstring.h>
#include <qregexp.h>

namespace Rendering {

	CDisplayRendering::CDisplayRendering(CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions)
: CHTMLExportRendering(CHTMLExportRendering::Settings(true), displayOptions, filterOptions) {}

	const QString CDisplayRendering::entryLink( const KeyTreeItem& item, CSwordModuleInfo*  module ) {
		QString linkText;

		const bool isBible = module && (module->type() == CSwordModuleInfo::Bible);
		CSwordVerseKey vk(module); //only valid for bible modules, i.e. isBible == true
		vk.Headings(true);

		if (isBible) {
			vk = item.key();
		}

		if (isBible && (vk.Verse() == 0)) {
			return QString::null; //Warning: return already here
		}

		switch (item.settings().keyRenderingFace) {

			case KeyTreeItem::Settings::NoKey: {
				linkText = QString::null;
				break; //no key is valid for all modules
			}

			case KeyTreeItem::Settings::CompleteShort: {
				if (isBible) {
					linkText = QString::fromUtf8(vk.getShortText());
					break;
				}

				//fall through for non-Bible modules
			}

			case KeyTreeItem::Settings::CompleteLong: {
				if (isBible) {
					linkText = vk.key();
					break;
				}

				//fall through for non-Bible modules
			}

			case KeyTreeItem::Settings::SimpleKey: {
				if (isBible) {
					linkText = QString::number(vk.Verse());
					break;
				}

				//fall through for non-Bible modules
			}

			default: { //default behaviour to return the passed key
				linkText = item.key();
				break;
			}
		}

		if (linkText.isEmpty()) {
			return QString("<a name=\"").append(keyToHTMLAnchor(item.key())).append("\" />");
		}
		else {
			return QString("<a name=\"").append(keyToHTMLAnchor(item.key())).append("\" ")
				   .append("href=\"")
				   .append(CReferenceManager::encodeHyperlink(
							   module->name(), item.key(), CReferenceManager::typeFromModule(module->type()))
						  )
				   .append("\">").append(linkText).append("</a>\n");
		}

		return QString::null;
	}

	const QString CDisplayRendering::keyToHTMLAnchor(const QString& key) {
		QString ret = key;
		ret = ret.stripWhiteSpace().remove(QRegExp("[^A-Za-z0-9]+"));
		ret = ret.remove(QRegExp("^\\d+|"));

		return ret;
	}

	const QString CDisplayRendering::finishText( const QString& oldText, KeyTree& tree ) {
		ListCSwordModuleInfo modules = tree.collectModules();


		//marking words is very slow, we have to find a better solution

		/*
		 //mark all words by spans

		 QString text = oldText;

		 QRegExp re("(\\b)(?=\\w)"); //word begin marker
		 int pos = text.find(re, 0);

		 while (pos != -1) { //word begin found
		  //qWarning("found word at %i in %i", pos, text.length());
		  int endPos = pos + 1;
		  if (!CToolClass::inHTMLTag(pos+1, text)) { //the re has a positive look ahead which matches one char before the word start
		   //qWarning("matched %s", text.mid(pos+1, 4).latin1());

		   //find end of word and put a marker around it
		   endPos = text.find(QRegExp("\\b|[,.:]"), pos+1);
		   if ((endPos != -1) && !CToolClass::inHTMLTag(endPos, text) && (endPos - pos >= 3)) { //reuire wordslonger than 3 chars
		    text.insert(endPos, "</span>");
		    text.insert(pos, "<span class=\"word\">");

		    endPos += 26;
		   }
		  }
		  pos = text.find(re, endPos);
		 }
		*/
		const CLanguageMgr::Language* const lang =
			(modules.count() >= 1)
			? modules.first()->language()
			: CPointers::languageMgr()->defaultLanguage();

		CDisplayTemplateMgr* tMgr = CPointers::displayTemplateManager();

		Q_ASSERT(modules.count() >= 1);

		CDisplayTemplateMgr::Settings settings;
		settings.modules = modules;
		settings.langAbbrev = ((modules.count() == 1) && lang->isValid())
							  ? lang->abbrev()
							  : QString::null;
		settings.pageDirection = (modules.count() == 1)
								 ? ((modules.first()->textDirection() == CSwordModuleInfo::LeftToRight) ? "ltr"  : "rtl")
						 : QString::null;

		return tMgr->fillTemplate(CBTConfig::get
									  (CBTConfig::displayStyle), oldText, settings);


	}
}
