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

#include <QListWidget>

#include <QObject>
#include <QSize>
#include <QString>


class QWidget;

class CListWidget : public QListWidget {
        Q_OBJECT
    public:
        CListWidget(QWidget* parent = nullptr);

        QSize sizeHint() const override;
        void setCharWidth(int width);
};
