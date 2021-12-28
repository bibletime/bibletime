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

#include <QWidget>

#include <QObject>
#include <QString>


class CScrollButton;
class QToolButton;
class QVBoxLayout;
class QWheelEvent;

/**
 * This class implements the Scroller Widget-set, which
 * consists of two normal ref @QToolButton and a enhanced @ref CScrollButton
 */
class CScrollerWidgetSet: public QWidget {

    Q_OBJECT

public: // methods:

    CScrollerWidgetSet(QWidget * parent = nullptr);

    /**
    * Sets the tooltips for the given entries using the parameters as text.
    */
    void setToolTips(const QString & nextEntry,
                     const QString & scrollButton,
                     const QString & previousEntry);

Q_SIGNALS:

    /**
    * Is emitted to proceed to some other entry relative to the
    * current, indicated by the int value.
    * \param count offset to change to
    */
    void change(int count);

    void scroller_pressed();

    void scroller_released();

protected: // methods:

    void wheelEvent(QWheelEvent * e) override;

private: // fields:

    QVBoxLayout * m_layout;
    QToolButton * m_buttonUp;
    QToolButton * m_buttonDown;
    CScrollButton * m_scrollButton;

};
