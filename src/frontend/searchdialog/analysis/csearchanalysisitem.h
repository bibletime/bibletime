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

#include <QGraphicsRectItem>

#include <cstddef>
#include <QPixmap>
#include <QString>
#include <QVector>
#include <memory>


class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

namespace Search {

class CSearchAnalysisItem : public QGraphicsRectItem {
    public:
        CSearchAnalysisItem(QString bookname, int numModules);

        auto const & bookName() const noexcept { return m_bookName; }
        auto & counts() noexcept { return m_counts; }
        auto const  & counts() const noexcept { return m_counts; }

        int width() const;

        void setScaleFactor(double value) noexcept { m_scaleFactor = value; }

    private:
        void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    private: // fields:
        double m_scaleFactor = 0.0;
        QString const m_bookName;
        QVector<std::size_t> m_counts;
        std::unique_ptr<QPixmap> m_bufferPixmap;

};

}
