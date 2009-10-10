/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CSCROLLERWIDGETSET_H
#define CSCROLLERWIDGETSET_H

#include <QWidget>


class CScrollButton;
class QString;
class QToolButton;
class QVBoxLayout;

/**
 * This class implements the Scroller Widget-set, which
 * consists of two normal ref @QToolButton and a enhanced @ref CScrollButton
 *
  * @author The BibleTime team
  */
class CScrollerWidgetSet : public QWidget {
    Q_OBJECT
    public:
        /**
        * the constructor
        */
        CScrollerWidgetSet(QWidget *parent = 0);
        /**
        * Sets the tooltips for the given entries using the parameters as text.
        */
        void setToolTips( const QString nextEntry, const QString scrollButton, const QString previousEntry);

    signals:
        /**
        * is emitted to proceed to some other entry relative to the
        * current, indicated by the int value
        */
        void change(int count);

        /**
        * These emit when the scroll button is pressed or released
        */
        void scroller_pressed();
        void scroller_released();

    protected:

        virtual void wheelEvent( QWheelEvent* e );

        QToolButton* btn_up;
        QToolButton* btn_down;
        CScrollButton* btn_fx;

    protected slots:
        void slotLock();
        void slotUnlock();
        void slotUpClick();
        void slotDownClick();
        void slotScroller(int);

    private:
        QVBoxLayout *m_layout;

};

#endif
