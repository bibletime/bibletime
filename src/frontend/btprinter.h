/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTPRINTER_H
#define BTPRINTER_H

#include <QObject>
#include "../backend/rendering/cdisplayrendering.h"

#include "../backend/managers/cswordbackend.h"


/** \brief Manages the print item queue and printing. */
class BtPrinter final: public QObject, public Rendering::CDisplayRendering {

    Q_OBJECT

public: /* Methods: */

    BtPrinter(DisplayOptions const & displayOptions,
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

#endif /* BTPRINTER_H */
