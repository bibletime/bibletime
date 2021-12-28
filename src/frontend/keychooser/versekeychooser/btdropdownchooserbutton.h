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


class BtBibleKeyWidget;

/**
* Base class for book/ch/v dropdown list chooser buttons.
*/
class BtDropdownChooserButton : public QToolButton {
        Q_OBJECT
    public:
        BtDropdownChooserButton(void (BtBibleKeyWidget::*populateMenu)(QMenu &),
                                BtBibleKeyWidget & parent);

        /** The item list is constructed here just before the menu is shown.*/
        void mousePressEvent(QMouseEvent* event) override;

    protected: // methods:

        void wheelEvent(QWheelEvent* event) override;

    Q_SIGNALS:

        void stepItem(int step);

    private: // fields:

        void (BtBibleKeyWidget::*m_populateMenu)(QMenu &);
        BtBibleKeyWidget & m_parent;

};
