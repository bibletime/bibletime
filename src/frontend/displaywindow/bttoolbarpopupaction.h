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

#ifndef BT_TOOLBAR_POPUP_ACTION_H
#define BT_TOOLBAR_POPUP_ACTION_H

#include <QWidgetAction>


class QIcon;
class QMenu;
class QString;
class QToolButton;

// This class manages the toolbar display for going forward and backward in history.
class BtToolBarPopupAction : public QWidgetAction {
        Q_OBJECT
    public:

        BtToolBarPopupAction(const QIcon& icon, const QString& text, QObject* parent);
        ~BtToolBarPopupAction() override;

// return the QMenu object so a popup menu can be constructed
        QMenu* popupMenu() const;

        bool event(QEvent* e) override;

    protected:
        QWidget* createWidget(QWidget* parent) override;

    private slots:

// Slot to emit a triggered signal when the toolbar button is pressed
        void buttonPressed();

    private:
        QMenu* m_menu;
        QToolButton* m_button;
        QIcon m_icon;
        QString m_text;
};

#endif
