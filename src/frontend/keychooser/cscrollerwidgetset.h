/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
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
 */
class CScrollerWidgetSet: public QWidget {

    Q_OBJECT

public: /* Methods: */

    CScrollerWidgetSet(QWidget * parent = nullptr);

    /**
    * Sets the tooltips for the given entries using the parameters as text.
    */
    void setToolTips(const QString & nextEntry,
                     const QString & scrollButton,
                     const QString & previousEntry);

signals:

    /**
    * Is emitted to proceed to some other entry relative to the
    * current, indicated by the int value.
    * \param count offset to change to
    */
    void change(int count);

    void scroller_pressed();

    void scroller_released();

protected: /* Methods: */

    void wheelEvent(QWheelEvent * e) override;

protected slots:

    void slotLock();
    void slotUnlock();
    void slotUpClick();
    void slotDownClick();
    void slotScroller(int);

private: /* Fields: */

    QVBoxLayout * m_layout;
    QToolButton * m_buttonUp;
    QToolButton * m_buttonDown;
    CScrollButton * m_scrollButton;

};

#endif
