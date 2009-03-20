/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "util/ctoolclass.h"
#include "clistwidget.h"
#include "clistwidget.moc"

CListWidget::CListWidget(QWidget* parent)
 : QListWidget(parent)
{
}

CListWidget::~CListWidget() 
{
}

QSize CListWidget::sizeHint () const
{
	return QSize(100,120);
}

void CListWidget::setCharWidth(int width)
{
	setMaximumWidth(CToolClass::mWidth(this, width));		
}
