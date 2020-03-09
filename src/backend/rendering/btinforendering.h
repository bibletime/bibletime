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

#ifndef BTINFORENDERING_H
#define BTINFORENDERING_H


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
QString formatInfo(const ListInfoData & info,  BtConstModuleList const & modules = BtConstModuleList());
QString formatInfo(QString const & info, QString const & lang = QString());

QString decodeAbbreviation(QString const & data);

/**
*   \param modules render references to each Bible module in the list, if empty use standard Bible.
*       If module was provided in reference (KJV:John.1.1) this parameter has no effect.
*/
QString decodeCrossReference(QString const & data, BtConstModuleList const & modules = BtConstModuleList());
QString decodeFootnote(QString const & data);
QString decodeStrongs(QString const & data);
QString decodeMorph(QString const & data);
QString decodeSwordReference(QString const & data);
QString getWordTranslation(QString const & data);

} /* namespace Rendering { */

#endif // BTINFORENDERING_H
