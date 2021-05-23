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

#pragma once

#include <QTextBrowser>

#include <QPoint>

/**
* This class is a subclass of QTextBrowser. It adds the
* ability to drag a reference so it can be dropped on
* another widget
*/

class BtTextBrowser: public QTextBrowser {

    Q_OBJECT

public:
    BtTextBrowser(QWidget * parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent * event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QString m_hoveredUrl;
    QPoint m_startPos;
    bool m_readyToStartDrag = false;
};

