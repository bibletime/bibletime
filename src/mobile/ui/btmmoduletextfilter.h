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
/**
 * \brief This filter provides a method for modifying text generated
 * by BtModuleTextModel.
 *
 * The "rich text" used by QML does not support
 * attributes within a href tag. These attributes are added to the url
 * so they can be obtained later for use of the MAG window.
 * It also does some other miscellaneous processing.
 */


class BtmModuleTextFilter: public BtModuleTextFilter {

public:
    BtmModuleTextFilter();
    ~BtmModuleTextFilter();

    QString processText(const QString& text);
    void setShowReferences(bool on);

private:
    void fixDoubleBR();
    QString fixNonRichText(const QString& text);
    int rewriteFootnoteAsLink(int i, const QString& part);
    int rewriteHref(int i, const QString& part);
    int rewriteLemmaOrMorphAsLink(int i, const QString& part);
    void splitText(const QString& text);

    bool m_showReferences;
    int m_spanDepth;

    QStringList m_parts;
};

} // end namespace

#endif
