//
// C++ Interface: BtFontSizeWidget
//
// Description: A font combobox widget for the toolbar
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 1999-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef BTFONTSIZEWIDGET_H
#define BTFONTSIZEWIDGET_H

#include <QComboBox>


class BtFontSizeWidget : public QComboBox 
{
	Q_OBJECT

public:
	BtFontSizeWidget(QWidget* parent=0);
	~BtFontSizeWidget();
	void setFontSize(int size);
	int fontSize() const;
	
private slots:
	virtual void changed(const QString& text);
			
signals:
	void fontSizeChanged( int );
};

#endif
