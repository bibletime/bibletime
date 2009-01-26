/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/
#ifndef BTHTMLJSOBJECT_H
#define BTHTMLJSOBJECT_H

#include <QObject>
#include <QPoint>

class BtHtmlReadDisplay;

class BtHtmlJsObject: public QObject
{
	Q_OBJECT

public:
	BtHtmlJsObject(BtHtmlReadDisplay* display);
	~BtHtmlJsObject(){}
	void moveToAnchor(const QString& anchor);

public slots: void mouseMoveEvent(const QString& attributes, const int& x, const int& y, const bool& shiftKey);
	void mouseClick(const QString& url);
	void mouseDownLeft(const QString& url, const int& X, const int& Y);
	void mouseDownRight(const QString& url, const QString& lemma);
	void timeOutEvent(const QString& attributes);

signals:
	void startTimer(int time);
	void mouseMoveAttribute(const QString& attrName, const QString& attrValue);
	void gotoAnchor(const QString& anchor);

private:
	int m_int;
	BtHtmlReadDisplay* m_display;

	struct DNDData 
	{
		bool mousePressed;
		bool isDragging;
		QPoint startPos;
		QString url;
		enum DragType 
		{
			Link,
			Text
		} dragType;
	}		m_dndData;

};

#endif
