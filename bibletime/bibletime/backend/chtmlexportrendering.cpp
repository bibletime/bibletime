//
// C++ Implementation: chtmlexportrendering
//
// Description:
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

//Backend
#include "chtmlexportrendering.h"

#include "cdisplaytemplatemgr.h"
#include "clanguagemgr.h"
#include "cswordkey.h"
#include "cswordversekey.h"
#include "cswordmoduleinfo.h"

//Util
#include "util/cpointers.h"
#include "util/scoped_resource.h"

//KDE includes
#include <klocale.h>

namespace Rendering {

	CHTMLExportRendering::CHTMLExportRendering(const CHTMLExportRendering::Settings& settings, CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions)
: m_displayOptions(displayOptions),
	m_filterOptions(filterOptions),
	m_settings(settings) {}

	CHTMLExportRendering::~CHTMLExportRendering() {}

	const QString CHTMLExportRendering::renderEntry( const KeyTreeItem& i, CSwordKey* k) {
		//  qDebug("CHTMLExportRendering::renderEntry");

		if (i.hasAlternativeContent()) {
			QString ret;
			ret.setLatin1(i.settings().highlight ? "<div class=\"currententry\">" : "<div class=\"entry\">");
			ret.append(i.getAlternativeContent());

			//   Q_ASSERT(i.hasChildItems());

			if (i.hasChildItems()) {
				KeyTree const * tree = i.childList();

				const ListCSwordModuleInfo& modules( tree->collectModules() );

				if (modules.count() == 1) { //insert the direction into the sorrounding div
					ret.insert( 5, QString("dir=\"%1\" ").arg((modules.first()->textDirection() == CSwordModuleInfo::LeftToRight) ? "ltr" : "rtl" ));
				}

				for ( KeyTreeItem* c = tree->first(); c; c = tree->next() ) {
					ret.append( renderEntry( *c ) );
				}
			}

			ret.append("</div>");
			return ret; //WARNING: Return already here!
		}


		const ListCSwordModuleInfo& modules( i.modules() );

		Q_ASSERT(modules.count() >= 1);

		util::scoped_ptr<CSwordKey> scoped_key( !k ? CSwordKey::createInstance(modules.first()) : 0 );

		CSwordKey* key = k ? k : scoped_key;

		Q_ASSERT(key);

		CSwordVerseKey* myVK = dynamic_cast<CSwordVerseKey*>(key);

		if ( myVK  ) {
			myVK->Headings(1);
		}

		QString renderedText( (modules.count() > 1) ? "<tr>" : "" );

		if (modules.count() == 0) {
			return QString(""); //no module present for rendering
		}

		// Only insert the table stuff if we are displaying parallel.
		// Otherwise, strip out he table stuff -> the whole chapter will be rendered in one cell!

		//declarations out of the loop for optimization
		QString entry;

		QString keyText;

		bool isRTL;

		//taken out of the loop for optimization
		QString preverseHeading;

		QString langAttr;

		ListCSwordModuleInfo::const_iterator end_modItr = modules.end();

		for (ListCSwordModuleInfo::const_iterator mod_Itr(modules.begin()); mod_Itr != end_modItr; ++mod_Itr) {
			key->module(*mod_Itr);
			key->key( i.key() );

			keyText = key->key();
			isRTL = ((*mod_Itr)->textDirection() == CSwordModuleInfo::RightToLeft);
			entry = QString::null;

			if ((*mod_Itr)->language()->isValid()) {
				langAttr.setLatin1("xml:lang=\"")
				.append((*mod_Itr)->language()->abbrev())
				.append("\" lang=\"")
				.append((*mod_Itr)->language()->abbrev())
				.append("\"");
			}
			else {
				langAttr.setLatin1("xml:lang=\"")
				.append((*mod_Itr)->module()->Lang())
				.append("\" lang=\"")
				.append((*mod_Itr)->module()->Lang())
				.append("\"");
			}

			const QString key_renderedText = key->renderedText();

			//   qWarning(key_renderedText.latin1());

			if (m_filterOptions.headings) {
				AttributeValue::const_iterator it =
					(*mod_Itr)->module()->getEntryAttributes()["Heading"]["Preverse"].begin();
				const AttributeValue::const_iterator end =
					(*mod_Itr)->module()->getEntryAttributes()["Heading"]["Preverse"].end();

				for (; it != end; ++it) {
					preverseHeading = QString::fromUtf8(it->second.c_str());

					//TODO: Take care of the heading type!

					if (!preverseHeading.isEmpty()) {
						entry.append("<div ")
						.append(langAttr)
						.append(" class=\"sectiontitle\">")
						.append(preverseHeading)
						.append("</div>");
					}
				}
			}

			entry.append(m_displayOptions.lineBreaks  ? "<div "  : "<span ");

			if (modules.count() == 1) { //insert only the class if we're not in a td
				entry.append( i.settings().highlight  ? "class=\"currententry\" " : "class=\"entry\" " );
			}

			entry.append(langAttr).append(isRTL ? " dir=\"rtl\"" : " dir=\"ltr\"").append(">");

			//keys should normally be left-to-right, but this doesn't apply in all cases
			entry.append("<span class=\"entryname\" dir=\"ltr\">").append(entryLink(i, *mod_Itr)).append("</span>");

			if (m_settings.addText) {
				//entry.append( QString::fromLatin1("<span %1>%2</span>").arg(langAttr).arg(key_renderedText) );
				entry.append( key_renderedText );
			}

			if (i.hasChildItems()) {
				KeyTree const * tree = i.childList();

				for (KeyTreeItem* c = tree->first(); c; c = tree->next()) {
					entry.append( renderEntry(*c) );
				}
			}

			entry.append(m_displayOptions.lineBreaks ? "</div>\n"  : "</span>\n");

			if (modules.count() == 1) {
				renderedText.append( entry );
			}
			else {
				renderedText.append("<td class=\"")
				.append(i.settings().highlight ? "currententry" : "entry")
				.append("\" ")
				.append(langAttr)
				.append(" dir=\"")
				.append(isRTL ? "rtl" : "ltr")
				.append("\">")
				.append(entry)
				.append("</td>\n");
			}
		}

		if (modules.count() > 1) {
			renderedText.append("</tr>\n");
		}

		//  qDebug("CHTMLExportRendering: %s", renderedText.latin1());
		return renderedText;
	}

	void CHTMLExportRendering::initRendering() {
		CPointers::backend()->setDisplayOptions( m_displayOptions );
		CPointers::backend()->setFilterOptions( m_filterOptions );
	}

	const QString CHTMLExportRendering::finishText( const QString& text, KeyTree& tree ) {
		ListCSwordModuleInfo modules = tree.collectModules();

		const CLanguageMgr::Language* const lang = modules.first()->language();

		CDisplayTemplateMgr* tMgr = CPointers::displayTemplateManager();
		CDisplayTemplateMgr::Settings settings;
		settings.modules = modules;
		settings.langAbbrev = ((modules.count() == 1) && lang->isValid())
							  ? lang->abbrev()
							  : "unknown";
		settings.pageDirection = (modules.count() == 1)
								 ? ((modules.first()->textDirection() == CSwordModuleInfo::LeftToRight) ? "ltr"  : "rtl")
						 : QString::null;

		return tMgr->fillTemplate(i18n("Export"), text, settings);
	}

	/*!
	    \fn CHTMLExportRendering::entryLink( KeyTreeItem& item )
	 */
	const QString CHTMLExportRendering::entryLink( const KeyTreeItem& item, CSwordModuleInfo* ) {
		return item.key();
	}

}

; //end of namespace "Rendering"
