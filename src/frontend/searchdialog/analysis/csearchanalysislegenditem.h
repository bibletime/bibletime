/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef SEARCHCSEARCHANALYSISLEGENDITEM_H
#define SEARCHCSEARCHANALYSISLEGENDITEM_H

#include <QGraphicsRectItem>


class CSwordModuleInfo;

namespace Search {

class CSearchAnalysisLegendItem : public QGraphicsRectItem {
    public: /* Methods: */
        inline CSearchAnalysisLegendItem(const QList<const CSwordModuleInfo*> &modules)
            : m_moduleList(modules) {}

    private: /* Methods: */
        /** Reimplementation of QGraphicsItem::paint. */
        virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*);

    private: /* Fields: */
        QList<const CSwordModuleInfo*> m_moduleList;

};

} // namespace Search

#endif
