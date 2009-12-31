/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTCOLORWIDGET_H
#define BTCOLORWIDGET_H

#include <QFrame>


class QPaintEvent;

class BtColorWidget : public QFrame {
        Q_OBJECT

    public:
        BtColorWidget(QWidget* parent = 0);
        ~BtColorWidget();
        QSize sizeHint() const;
        void setColor(const QColor& color);

    protected:
//	void paintEvent( QPaintEvent* );
        void mouseReleaseEvent(QMouseEvent* event);

    private:
        void showColorDialog();

        QColor m_color;

    signals:
        void changed(const QColor& color);
};

#endif


