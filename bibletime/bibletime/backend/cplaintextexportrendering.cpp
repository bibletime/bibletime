//
// C++ Implementation: cplaintextexportrendering
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
#include "cplaintextexportrendering.h"
#include "cswordkey.h"

//Util
#include "util/scoped_resource.h"



namespace Rendering {

	CPlainTextExportRendering::CPlainTextExportRendering(const CPlainTextExportRendering::Settings& settings, CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions)
: CHTMLExportRendering(settings, displayOptions, filterOptions) {}

	CPlainTextExportRendering::~CPlainTextExportRendering() {}

	const QString CPlainTextExportRendering::renderEntry( const KeyTreeItem& i, CSwordKey*  ) {
		if (!m_settings.addText) {
			return QString(i.key()).append("\n");
		}

		ListCSwordModuleInfo modules = i.modules();
		util::scoped_ptr<CSwordKey> key( CSwordKey::createInstance(modules.first()) );
		QString renderedText = QString(i.key()).append(":\n");

		QString entry;
		//   for (CSwordModuleInfo* m = modules.first(); m; m = modules.next()) {
		ListCSwordModuleInfo::iterator end_it = modules.end();

		for (ListCSwordModuleInfo::iterator it(modules.begin()); it != end_it; ++it) {
			key->module(*it);
			key->key( i.key() );

			//ToDo: Check this code
			entry.append(key->strippedText()).append("\n");
			renderedText.append( entry );
		}

		return renderedText;
	}

	const QString CPlainTextExportRendering::finishText( const QString& oldText, KeyTree& ) {
		return oldText;
	}

};
