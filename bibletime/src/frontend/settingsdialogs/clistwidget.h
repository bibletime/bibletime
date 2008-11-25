//
// C++ Interface: CListWidget
//
// Description: BibleTime font chooser
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 1999-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CLISTWIDGET_H
#define CLISTWIDGET_H

#include <QListWidget>

class CListWidget : public QListWidget {
	Q_OBJECT

public:
	CListWidget(QWidget* parent = 0);
	~CListWidget();
	virtual QSize sizeHint () const;
	void setCharWidth(int width);
};

#endif
