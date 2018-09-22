/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTHTMLREADDISPLAY_H
#define BTHTMLREADDISPLAY_H

#include "frontend/btwebengineview.h"
#include "frontend/btwebenginepage.h"
#include "frontend/display/creaddisplay.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QPoint>
#include <QTimerEvent>
#include "frontend/display/bthtmljsobject.h"


class BtHtmlReadDisplayView;

/** The implementation for the HTML read display.
  * @author The BibleTime team
  */
class BtHtmlReadDisplay : public BtWebEnginePage, public CReadDisplay {
        Q_OBJECT

        friend class BtHtmlReadDisplayView;

    public:

        BtHtmlReadDisplay( CReadWindow* readWindow, QWidget* parent = nullptr );
        ~BtHtmlReadDisplay() override;

        //reimplemented functions from CDisplay
        // Returns the right text part in the specified format.
        const QString text(const CDisplay::TextType format = CDisplay::HTMLText,
                           const CDisplay::TextPart part = CDisplay::Document)
                override;

        void setText( const QString& newText ) override;

        QString getCurrentSource();

        bool hasSelection() const override;

        void selectAll() override;
        void moveToAnchor( const QString& anchor ) override;
        void openFindTextDialog() override;
        inline QString getCurrentNodeInfo() const override {
            return m_nodeInfo;
        }
        QWidget* view() override;
        void setLemma(const QString& lemma);

    public slots:
        void loadJSObject();
        void slotLoadFinished(bool);

    signals:
        void completed();

    protected:

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

        QString m_nodeInfo;
        int m_magTimerId;

// For debugging javascript - setbreakpoint in this function to catch javascript error messages
//#define DEBUG_JS
#ifdef DEBUG_JS
        void javaScriptConsoleMessage (const QString & message, int lineNumber, const QString & sourceID );
#endif

    private slots:
        void slotDelayedMoveToAnchor();

    private:
        void initJavascript();
        void loadScripts();
        QString readJavascript(const QString& jsFileName);

        BtHtmlReadDisplayView* m_view;
        BtHtmlJsObject* m_jsObject;
        QString m_currentAnchorCache;

};


class BtHtmlReadDisplayView : public BtWebEngineView {
        Q_OBJECT
    protected:
        friend class BtHtmlReadDisplay;
        void contextMenuEvent(QContextMenuEvent* event) override;
        BtHtmlReadDisplayView(BtHtmlReadDisplay* display, QWidget* parent, CReadWindow* readWindow);
        ~BtHtmlReadDisplayView();
        bool event(QEvent* e) override;

    private:
        BtHtmlReadDisplay* m_display;
        CReadWindow* m_readWindow;
        void dropEvent( QDropEvent* e ) override;
        void dragEnterEvent( QDragEnterEvent* e ) override;
        void dragMoveEvent( QDragMoveEvent* e ) override;
};

#endif
