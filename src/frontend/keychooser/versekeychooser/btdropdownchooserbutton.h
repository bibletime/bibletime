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

#ifndef BTDROPDOWNCHOOSERBUTTON_H
#define BTDROPDOWNCHOOSERBUTTON_H

#include <QToolButton>


class BtBibleKeyWidget;

/**
* Base class for book/ch/v dropdown list chooser buttons.
*/
class BtDropdownChooserButton : public QToolButton {
        Q_OBJECT
    public:
        BtDropdownChooserButton(BtBibleKeyWidget* ref);

        /** The item list is constructed here just before the menu is shown.*/
        void mousePressEvent(QMouseEvent* event) override;
        /** Recreates the menu list.*/
        virtual void newList() = 0;
        /** Returns the verse reference widget which this button belongs to.*/
        BtBibleKeyWidget* ref() {
            return m_ref;
        }
    public slots:
        /** When a menu item is selected the key will be changed.*/
        virtual void slotMenuTriggered(QAction* action) = 0;
    protected:
        BtBibleKeyWidget* m_ref;
        void wheelEvent(QWheelEvent* event) override;
    signals:
        void stepItem(int step);
};

/** See BtDropdownChooserButton.*/
class BtBookDropdownChooserButton : public BtDropdownChooserButton {
        Q_OBJECT
    public:
        BtBookDropdownChooserButton(BtBibleKeyWidget* ref);
        void newList() override;
    public slots:
        void slotMenuTriggered(QAction* action) override;
};

/** See BtDropdownChooserButton.*/
class BtChapterDropdownChooserButton : public BtDropdownChooserButton {
        Q_OBJECT
    public:
        BtChapterDropdownChooserButton(BtBibleKeyWidget* ref);
        void newList() override;
    public slots:
        void slotMenuTriggered(QAction* action) override;
};

/** See BtDropdownChooserButton.*/
class BtVerseDropdownChooserButton : public BtDropdownChooserButton {
        Q_OBJECT
    public:
        BtVerseDropdownChooserButton(BtBibleKeyWidget* ref);
        void newList() override;
    public slots:
        void slotMenuTriggered(QAction* action) override;
};
#endif
