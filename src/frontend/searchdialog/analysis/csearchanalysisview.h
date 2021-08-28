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

#include <QGraphicsView>


class QResizeEvent;
class QWidget;

namespace Search {

class CSearchAnalysisScene;

class CSearchAnalysisView : public QGraphicsView {
    public:
        CSearchAnalysisView(CSearchAnalysisScene* scene, QWidget* parent);

    protected:

        void resizeEvent(QResizeEvent* e) override;

};

}
