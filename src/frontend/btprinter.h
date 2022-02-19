/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include <QObject>
#include "../backend/rendering/cdisplayrendering.h"

#include "../backend/managers/cswordbackend.h"


/** \brief Manages the print item queue and printing. */
class BtPrinter final: public QObject, public Rendering::CDisplayRendering {

    Q_OBJECT

public: // methods:

    BtPrinter(DisplayOptions const & displayOptions,
              FilterOptions const & filterOptions,
              QObject * const parent = nullptr);

    void printKeyTree(KeyTree const &);

private: // methods:

    QString entryLink(KeyTreeItem const & item,
                      const CSwordKey * key) override;

    QString renderEntry(KeyTreeItem const & item) override;
    QString finishText(QString const & text, KeyTree const & tree) override;

};
