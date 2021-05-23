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

#ifndef BTTEXTBROWSER_H
#define BTTEXTBROWSER_H

#include <QPoint>
#include <QTextBrowser>

/**
* This class is a subclass of QTextBrowser. It adds the
* ability to drag a reference so it can be dropped on
* another widget
*/

class BtTextBrowser: public QTextBrowser {

    Q_OBJECT

public:
    BtTextBrowser(QWidget *parent);

protected:
    void keyPressEvent(QKeyEvent * event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QString m_hoveredUrl;
    QString m_dragUrl;
    QPoint m_startPos;
    bool m_isDragging = false;
    bool m_mousePressed = false;
};

#endif
