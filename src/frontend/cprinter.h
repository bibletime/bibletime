/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CPRINTER_H
#define CPRINTER_H

#include <QObject>
#include "backend/rendering/cdisplayrendering.h"

#include "backend/managers/cswordbackend.h"


class QWebPage;

namespace Printing {

// The CPrinter class manages the print item queue and the printing of them to the printer.

class CPrinter : public QObject, public Rendering::CDisplayRendering {
        Q_OBJECT
    public:
        CPrinter(QObject *parent,
                 const DisplayOptions &displayOptions,
                 const FilterOptions &filterOptions);

        virtual ~CPrinter();
        void printKeyTree( KeyTree& );

    protected:
        virtual const QString entryLink(const KeyTreeItem &item,
                                        const CSwordModuleInfo *module);

        virtual const QString renderEntry( const KeyTreeItem&, CSwordKey* = 0 );
        virtual const QString finishText(const QString& arg1, KeyTree& tree);

    private:
        QWebPage* m_htmlPage;
};

} //namespace Printing

#endif
