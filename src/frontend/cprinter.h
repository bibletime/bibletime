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


namespace Printing {

/** \brief Manages the print item queue and printing. */
class CPrinter final: public QObject, public Rendering::CDisplayRendering {

    Q_OBJECT

public: /* Methods: */

    CPrinter(DisplayOptions const & displayOptions,
             FilterOptions const & filterOptions,
             QObject * const parent = nullptr);

    void printKeyTree(KeyTree const &);

private: /* Methods: */

    QString entryLink(KeyTreeItem const & item,
                      CSwordModuleInfo const * module) override;

    QString renderEntry(KeyTreeItem const & item,
                        CSwordKey * const key = nullptr) override;
    QString finishText(QString const & text, KeyTree const & tree) override;

};

} //namespace Printing

#endif
