/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include <QMdiArea>

#include <QObject>
#include <QString>
#include <QtCore>
#include <QList>


class BibleTime;
class CDisplayWindow;
class QMdiSubWindow;
class QWidget;

/**
  A custom MDI area widget.
*/
class CMDIArea: public QMdiArea {

        Q_OBJECT
        Q_PROPERTY(MDIArrangementMode m_mdiArrangementMode READ getMDIArrangementMode WRITE setMDIArrangementMode)

    public: // types:

        /**
          Possible MDI subwindow arrangement modes.
        */
        enum MDIArrangementMode {
            ArrangementModeTileVertical = 1,
            ArrangementModeTileHorizontal = 2,
            ArrangementModeCascade = 3,
            ArrangementModeManual = 4,
            ArrangementModeTile = 5,
            ArrangementModeTabbed = 6
        };

    public: // methods:

        /**
          \param[in] parent BibleTime main window used for parent widget.
        */
        CMDIArea(BibleTime *parent);

        QMdiSubWindow * addDisplayWindow(CDisplayWindow * displayWindow);

        /**
          Returns the BibleTime main window
        */
        BibleTime* bibleTimeWindow() {
            return m_bibleTime;
        }

        /**
          Resets the MDI arrangement mode and arranges the windows.
          \param[in] mode new MDI arrangement mode.
         */
        void setMDIArrangementMode(const MDIArrangementMode mode);

        /**
          Returns the current MDI arrangement mode.
        */
        MDIArrangementMode getMDIArrangementMode() const
        { return m_mdiArrangementMode; }

        /**
        * Forces an update of the currently chosen window arrangement.
        */
        void triggerWindowUpdate();

        /**
          \returns a lists of all subwindows which are not minimized or hidden.
        */
        QList<QMdiSubWindow*> usableWindowList() const;

        /**
          Show or hide the sub-window min/max buttons.
        */
        void enableWindowMinMaxFlags(bool enable);

    public Q_SLOTS:

        /**
          Our own auto tile version which, if only one subwindow is left, shows it
          \note This not set an automatic arrangement mode, it just arranges the
                subwindows once. However, this method is also used when
                arranging the subwindows into a tile automatically.
        */
        void myTile();

        /**
          Our own cascade version which, if only one subwindow is left, shows it
          maximized.
          \note This not set an automatic arrangement mode, it just arranges the
                subwindows once. However, this method is also used when
                arranging the subwindows into a cascade automatically.
        */
        void myCascade();

        /**
          Our own vertical tile version which, if only one subwindow is left,
          shows it maximized.
          \note This not set an automatic arrangement mode, it just arranges the
                subwindows once. However, this method is also used when
                arranging the subwindows into a vertical tiling automatically.
        */
        void myTileVertical();

        /**
          Our own horizontal tile version which, if only one subwindow is left,
          shows it maximized.
          \note This not set an automatic arrangement mode, it just arranges the
                subwindows once. However, this method is also used when
                arranging the subwindows into a horizontal tiling automatically.
        */
        void myTileHorizontal();

        void findNextTextInActiveWindow();

        void findPreviousTextInActiveWindow();

        void highlightTextInActiveWindow(const QString& text, bool caseSensitive);

    Q_SIGNALS:

        /**
        * Emits a signal to set the caption of the toplevel widget.
        */
        void sigSetToplevelCaption(const QString&);

    protected: // methods:

        void findTextInActiveWindow(bool const backward);

        /**
          Reimplementation of QWidget::resizeEvent() to handle our automatic
          tiling properly.
        */
        void resizeEvent(QResizeEvent *e) override;

        /**
          Reimplementation of QObject::eventFilter() used to handle some MDI
          subwindow events.
        */
        bool eventFilter(QObject *o, QEvent *e) override;

        void emitWindowCaptionChanged();

    protected: // fields:

        MDIArrangementMode m_mdiArrangementMode;

    private: // fields:

        CDisplayWindow* m_activeWindow;
        BibleTime* m_bibleTime;

}; /* class CMDIArea */
