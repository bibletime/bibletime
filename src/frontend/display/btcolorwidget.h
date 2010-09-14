//
// C++ Interface: BtColorWidget
//
// Description: A color choosing widget for the toolbar
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 1999-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef BTCOLORWIDGET_H
#define BTCOLORWIDGET_H

#include <QFrame>
class QPaintEvent;

class BtColorWidget : public QFrame
{
	Q_OBJECT

public:
	BtColorWidget(QWidget* parent=0);
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


