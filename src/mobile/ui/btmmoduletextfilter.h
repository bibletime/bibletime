/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTM_MODULE_TEXT_FILTER_H
#define BTM_MODULE_TEXT_FILTER_H

#include <QString>
#include "backend/models/btmoduletextmodel.h"

namespace btm {

class BtmModuleTextFilter: public BtModuleTextFilter {

public:
    BtmModuleTextFilter();
    QString processText(const QString& text);
    void setShowReferences(bool on);

private:
    void fixDoubleBR();
    QString fixNonRichText(const QString& text);
    int partsContains(const QString& text, int start);
    void rewriteFootnoteAsLink();
    void rewriteLemmaMorphAsLink();
    void splitText(const QString& text);

    bool m_showReferences;

    QStringList m_parts;
};

} // end namespace

#endif
