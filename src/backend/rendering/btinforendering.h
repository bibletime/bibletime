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

#include <QList>
#include <QPair>
#include <QString>

#include "../drivers/btmodulelist.h"


namespace Rendering {

enum InfoType {
    Abbreviation,
    CrossReference,
    Footnote,
    Lemma,
    Morph,
    Text,
    Reference, // for rendering references
};

using InfoData = QPair<InfoType, QString>;
using ListInfoData = QList<InfoData>;


/** Parse string for attributes */
ListInfoData detectInfo(QString const & data);

/** Process list of InfoData and format all data into string */
QString formatInfo(ListInfoData const & info,
                   BtConstModuleList const & modules);
QString formatInfo(QString const & info, QString const & lang = QString());

} /* namespace Rendering { */
