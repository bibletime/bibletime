/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTHTMLREADDISPLAY_H
#define BTHTMLREADDISPLAY_H

//BibleTime includes
#include "bthtmljsobject.h"
#include "creaddisplay.h"

//Qt includes
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QTimerEvent>
#include <QWebView>
#include <QWebPage>
#include <QWebFrame>
#include <QPoint>

class BtHtmlReadDisplayView;
class QScrollArea;
class QWidget;
class QString;
class BtHtmlReadDisplay;


/** The implementation for the HTML read display.
  * @author The BibleTime team
  */
class BtHtmlReadDisplay : public QWebPage, public CReadDisplay {
	Q_OBJECT
public:
	//reimplemented functions from CDisplay
	// Returns the right text part in the specified format.
	virtual const QString text( const CDisplay::TextType format = CDisplay::HTMLText, 
                                const CDisplay::TextPart part = CDisplay::Document );

	// Sets the new text for this display widget.
	virtual void setText( const QString& newText );
	virtual bool hasSelection();

	// Reimplementation.
	virtual void selectAll();
	virtual void moveToAnchor( const QString& anchor );
	virtual void openFindTextDialog();
	virtual QMap<CDisplay::NodeInfoType, QString> getCurrentNodeInfo() 
	{
		return m_nodeInfo;
	}
	QWidget* view();
	void setLemma(const QString& lemma);

public slots:
	void loadJSObject();
	void slotLoadFinished(bool);

signals:
	void completed();

protected:
	friend class CDisplay;
	BtHtmlReadDisplay( CReadWindow* readWindow, QWidget* parent = 0 );
	virtual ~BtHtmlReadDisplay();
	void slotGoToAnchor(const QString& anchor);
	struct DNDData 
	{
		bool mousePressed;
		bool isDragging;
		QString selection;
		QPoint startPos;
		enum DragType 
		{
			Link,
			Text
		} dragType;
	}
	m_dndData;

	QMap<NodeInfoType, QString> m_nodeInfo;
	int m_magTimerId;

// For debugging javascript - setbreakpoint in this function to catch javascript error messages
//#define DEBUG_JS
#ifdef DEBUG_JS
	void javaScriptConsoleMessage (const QString & message, int lineNumber, const QString & sourceID );
#endif

private:
	void initJavascript();
	BtHtmlReadDisplayView* m_view;
	BtHtmlJsObject* m_jsObject;
	QString m_currentAnchorCache;

};


class BtHtmlReadDisplayView : public QWebView, public CPointers 
{
	Q_OBJECT
protected:
	friend class BtHtmlReadDisplay;
	void contextMenuEvent(QContextMenuEvent* event);
	BtHtmlReadDisplayView(BtHtmlReadDisplay* display, QWidget* parent);
	~BtHtmlReadDisplayView();

private:
	BtHtmlReadDisplay* m_display;
	void dropEvent( QDropEvent* e );
	void dragEnterEvent( QDragEnterEvent* e );
	void dragMoveEvent( QDragMoveEvent* e );
};

#endif
