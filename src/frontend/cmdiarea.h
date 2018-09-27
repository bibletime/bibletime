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

#ifndef CMDIAREA_H
#define CMDIAREA_H

#include <QMdiArea>

#include <QList>


class BibleTime;
class CSwordModuleInfo;
class CDisplayWindow;

/**
  A custom MDI area widget.
*/
class CMDIArea: public QMdiArea {

        Q_OBJECT
        Q_PROPERTY(MDIArrangementMode m_mdiArrangementMode READ getMDIArrangementMode WRITE setMDIArrangementMode)

    public: /* Types: */

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

    public: /* Methods: */

        /**
          \param[in] parent BibleTime main window used for parent widget.
        */
        CMDIArea(BibleTime *parent);

        /**
          Reimplementation of QMdiArea::addSubWindow().
        */
        QMdiSubWindow * addSubWindow(QWidget * widget,
                                     Qt::WindowFlags windowFlags = nullptr);

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
        inline MDIArrangementMode getMDIArrangementMode() const {
            return m_mdiArrangementMode;
        }

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

    public slots:

        /**
          Uses Qt's tileSubWindows function.
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

        void findNextTextInActiveWindow(const QString& text, bool caseSensitive);

        void findPreviousTextInActiveWindow(const QString& text, bool caseSensitive);

        void highlightTextInActiveWindow(const QString& text, bool caseSensitive);

    signals:

        /**
        * Emits a signal to set the caption of the toplevel widget.
        */
        void sigSetToplevelCaption(const QString&);

    protected: /* Methods: */

        void findTextInActiveWindow(QString const & text,
                                    bool caseSensitive,
                                    bool backward);

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

        void fixSystemMenu(QMdiSubWindow* subWindow);

    protected slots:

        /**
          Called whan a subwindow was activated.
        */
        void slotSubWindowActivated(QMdiSubWindow *client);

        /**
          Called whan a tabbed subwindow is closed from the tab
        */
        void closeTab(int i);

    protected: /* Fields: */

        MDIArrangementMode m_mdiArrangementMode;

    private: /* Fields: */

        CDisplayWindow* m_activeWindow;
        BibleTime* m_bibleTime;

}; /* class CMDIArea */

#endif
