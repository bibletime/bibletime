/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CPRINTER_H
#define CPRINTER_H

#include "backend/managers/cswordbackend.h"
#include "backend/rendering/cdisplayrendering.h"
#include "frontend/cbtconfig.h"

#include <QObject>

class KHTMLPart;

namespace Printing {

	/**
	 * The CPrinter class manages the print item queue and the printing of them to the printer.
	 *
	 * @author The BibleTime team
	*/
class CPrinter : public QObject, public Rendering::CDisplayRendering {
		Q_OBJECT
public:
		CPrinter(QObject *parent, CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions);
		virtual ~CPrinter();
		void printKeyTree( KeyTree& );

protected:
		virtual const QString entryLink(const KeyTreeItem& item, CSwordModuleInfo* const module);
		virtual const QString renderEntry( const KeyTreeItem&, CSwordKey* = 0 );
		virtual const QString finishText(const QString& arg1, KeyTree& tree);

private:
		KHTMLPart* m_htmlPart;
};

} //namespace Printing

#endif
