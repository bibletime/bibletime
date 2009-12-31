/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/rendering/cplaintextexportrendering.h"

#include <boost/scoped_ptr.hpp>
#include "backend/keys/cswordkey.h"


namespace Rendering {

CPlainTextExportRendering::CPlainTextExportRendering(const CPlainTextExportRendering::Settings& settings, CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions)
        : CHTMLExportRendering(settings, displayOptions, filterOptions) {}

CPlainTextExportRendering::~CPlainTextExportRendering() {}

const QString CPlainTextExportRendering::renderEntry( const KeyTreeItem& i, CSwordKey*  ) {
    if (!m_settings.addText) {
        return QString(i.key()).append("\n");
    }

    QList<CSwordModuleInfo*> modules = i.modules();
    boost::scoped_ptr<CSwordKey> key( CSwordKey::createInstance(modules.first()) );
    QString renderedText = QString(i.key()).append(":\n");

    QString entry;
    //   for (CSwordModuleInfo* m = modules.first(); m; m = modules.next()) {
    QList<CSwordModuleInfo*>::iterator end_it = modules.end();

    for (QList<CSwordModuleInfo*>::iterator it(modules.begin()); it != end_it; ++it) {
        key->module(*it);
        key->key( i.key() );

        /// \todo Check this code
        entry.append(key->strippedText()).append("\n");
        renderedText.append( entry );
    }

    return renderedText;
}

const QString CPlainTextExportRendering::finishText( const QString& oldText, KeyTree& ) {
    return oldText;
}

}
