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

#include <QToolButton>

#include <QObject>
#include <QString>


class QMouseEvent;
class QWidget;

/**
* This Class implements the direct chooser button used in the KeyChooser Widget.
* \author The BibleTime team.
*/
class CScrollButton: public QToolButton {
        Q_OBJECT
    public:
        CScrollButton(QWidget *parent = nullptr);

    Q_SIGNALS:
        /**
        * \brief The lock() signal is emitted when the button grabs the mouse
        *        and enters the locked state.
        */
        void lock();

        /**
        * \brief The unlock() signal is emitted when the button releases the
        *        leaves the locked state.
        */
        void unlock();

        /**
        * \brief Indicates a change the user made by moving the mouse.
        *
        * \param count the number of items to be changed in the KeyChooser
        *              ComboBox.
        */
        void change_requested(int count);

    protected:
        /**
        * \brief Grabs the mouse on left button click and emits lock().
        */
        void mousePressEvent(QMouseEvent *e) override;

        /**
        * \brief If the mouse is grabbed and we release the left mouse button,
        *        releases the mouse and emits unlock().
        */
        void mouseReleaseEvent(QMouseEvent *e) override;

        void mouseMoveEvent(QMouseEvent *e) override;

    protected:
        /**
        * \brief Indicates whether the button is in locked state or not.
        *
        * If the button is in the locked state, this means the mouse is grabbed
        * and any mouse move events invoke calculation about whether to emit the
        * change_requested() signal.
        */
        bool m_isLocked;

        /**
         * \brief The amount the mouse moved.
         *
         * This is saved so slow movements of mice can still be tracked and yet not
         * emitting a change for every smallest movement of the mouse.
         */
        float m_movement;
};
