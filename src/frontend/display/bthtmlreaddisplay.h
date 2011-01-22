/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTHTMLREADDISPLAY_H
#define BTHTMLREADDISPLAY_H

#include "frontend/display/creaddisplay.h"
#include <QWebPage>

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QPoint>
#include <QTimerEvent>
#include <QWebView>
#include <QWebFrame>
#include "frontend/display/bthtmljsobject.h"


class BtHtmlReadDisplayView;
class QScrollArea;
class QWidget;
class QString;
class BtHtmlReadDisplay;
class QEvent;

/** The implementation for the HTML read display.
  * @author The BibleTime team
  */
class BtHtmlReadDisplay : public QWebPage, public CReadDisplay {
        Q_OBJECT

        friend class BtHtmlReadDisplayView;

    public:
        //reimplemented functions from CDisplay
        // Returns the right text part in the specified format.
        virtual const QString text( const CDisplay::TextType format = CDisplay::HTMLText,
                                    const CDisplay::TextPart part = CDisplay::Document );

        // Sets the new text for this display widget.
        virtual void setText( const QString& newText );
        // Get the current source
        virtual QString getCurrentSource();

        virtual bool hasSelection();

        // Reimplementation.
        virtual void selectAll();
        virtual void moveToAnchor( const QString& anchor );
        virtual void openFindTextDialog();
        virtual QMap<CDisplay::NodeInfoType, QString> getCurrentNodeInfo() {
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
        struct DNDData {
            bool mousePressed;
            bool isDragging;
            QString selection;
            QPoint startPos;
            enum DragType {
                Link,
                Text
            } dragType;
        }
        m_dndData;

        QString currentSource;

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


class BtHtmlReadDisplayView : public QWebView {
        Q_OBJECT
    protected:
        friend class BtHtmlReadDisplay;
        void contextMenuEvent(QContextMenuEvent* event);
        BtHtmlReadDisplayView(BtHtmlReadDisplay* display, QWidget* parent, CReadWindow* readWindow);
        ~BtHtmlReadDisplayView();
        bool event(QEvent* e);

    private:
        BtHtmlReadDisplay* m_display;
        CReadWindow* m_readWindow;
        void dropEvent( QDropEvent* e );
        void dragEnterEvent( QDragEnterEvent* e );
        void dragMoveEvent( QDragMoveEvent* e );
};

#endif
