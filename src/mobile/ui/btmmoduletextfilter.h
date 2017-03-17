/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTM_MODULE_TEXT_FILTER_H
#define BTM_MODULE_TEXT_FILTER_H

#include <QString>
#include <QDomNodeList>
#include "backend/models/btmoduletextmodel.h"

namespace btm {

class BtmModuleTextFilter: public BtModuleTextFilter {

public:
    BtmModuleTextFilter();
    QString processText(const QString& text);
    void setShowReferences(bool on);

private:
    void convertFootNoteToLink(QDomElement& element);
    void convertLemmaAndMorphToLink(QDomElement& element);
    QString getAttributeValue(const QDomElement& element, const QString& attrName) const;
    bool elementHasAttribute(const QDomElement& element, const QString& attrName) const;
    bool isFootnote(const QDomElement& element) const;
    bool isLemmaOrMorph(const QDomElement& element) const;
    void traverse(const QDomNodeList& nodeList);

    bool m_showReferences;
    QDomDocument m_doc;
};

} // end namespace

#endif
