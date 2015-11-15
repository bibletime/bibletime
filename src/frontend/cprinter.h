/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
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
        virtual QString entryLink(const KeyTreeItem &item,
                                  const CSwordModuleInfo * module) override;

        virtual QString renderEntry(const KeyTreeItem &item, CSwordKey * key = nullptr) override;
        virtual QString finishText(const QString &text, const KeyTree &tree) override;

    private:
        QWebPage* m_htmlPage;
};

} //namespace Printing

#endif
