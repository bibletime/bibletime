/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cprinter.h"

//BibleTime includes
#include "backend/cdisplaytemplatemgr.h"
#include "backend/cswordversekey.h"

//KDE includes
#include <khtml_part.h>
#include <khtmlview.h>

/* Class: CPrinter */

namespace Printing {

	CPrinter::CPrinter(QObject *, CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions)
:  QObject(0),
	CDisplayRendering(displayOptions, filterOptions),
	m_htmlPart(new KHTMLPart(0, 0, this)) {

		//override the filteroptions set in the c-tor of CDisplayRendering
		m_filterOptions.footnotes = false;
		m_filterOptions.scriptureReferences = false;
		m_filterOptions.strongNumbers = false;
		m_filterOptions.morphTags = false;
		m_filterOptions.headings = false;

		m_htmlPart->setJScriptEnabled(false);
		m_htmlPart->setJavaEnabled(false);
		m_htmlPart->setMetaRefreshEnabled(false);
		m_htmlPart->setPluginsEnabled(false);
		m_htmlPart->view()->resize(500,500);
		m_htmlPart->view()->hide();
	}

	CPrinter::~CPrinter() {
		delete m_htmlPart;
		m_htmlPart = 0;
	}

	void CPrinter::printKeyTree( KeyTree& tree ) {
		m_htmlPart->begin();
		m_htmlPart->write(renderKeyTree(tree));
		m_htmlPart->end();

		m_htmlPart->view()->layout();
		m_htmlPart->view()->print();
	}

	const QString CPrinter::entryLink(const KeyTreeItem& item, CSwordModuleInfo* module) {
		Q_ASSERT(module);

		if (module->type() == CSwordModuleInfo::Bible) {
			CSwordVerseKey vk(module);
			vk = item.key();

			switch (item.settings().keyRenderingFace) {
				case KeyTreeItem::Settings::CompleteShort:
					return QString::fromUtf8(vk.getShortText());

				case KeyTreeItem::Settings::CompleteLong:
					return vk.key();

				case KeyTreeItem::Settings::NoKey:
					return QString::null;

				case KeyTreeItem::Settings::SimpleKey: //fall through
				default:
					return QString::number(vk.Verse());
			}

		}

		return item.key();
	}

	const QString CPrinter::renderEntry( const KeyTreeItem& i, CSwordKey* ) {
		const CPrinter::KeyTreeItem* printItem = dynamic_cast<const CPrinter::KeyTreeItem*>(&i);
		Q_ASSERT(printItem);

		if (printItem && printItem->hasAlternativeContent()) {
			QString ret = QString::fromLatin1("<div class=\"entry\"><div class=\"rangeheading\">%1</div>").arg(printItem->getAlternativeContent());

			if (i.hasChildItems()) {
				KeyTree const * tree = i.childList();

				for ( KeyTreeItem* c = tree->first(); c; c = tree->next() ) {
					ret.append( CDisplayRendering::renderEntry( *c ) );
				}
			}

			ret.append("</div>");
			return ret;
		}

		return CDisplayRendering::renderEntry(i);
	}

	const QString CPrinter::finishText(const QString& text, KeyTree& tree) {
		ListCSwordModuleInfo modules = tree.collectModules();
		Q_ASSERT(modules.count() > 0);

		const CLanguageMgr::Language* const lang = modules.first()->language();
		Q_ASSERT(lang);

		CDisplayTemplateMgr::Settings settings;
		//settings.modules = modules;
		settings.pageCSS_ID = "printer";
		settings.langAbbrev = (lang && (modules.count() == 1) && lang->isValid())
							  ? lang->abbrev()
							  : "unknown";
		settings.pageDirection = (modules.count() == 1)
				 ? ((modules.first()->textDirection() == CSwordModuleInfo::LeftToRight) ? "ltr"  : "rtl")
				 : QString::null;

		CDisplayTemplateMgr* tMgr = CPointers::displayTemplateManager();
		return tMgr->fillTemplate(CBTConfig::get(CBTConfig::displayStyle), text, settings);
	}

} //end of namespace
