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

#include <QWidgetAction>

#include <memory>
#include <QIcon>
#include <QObject>
#include <QString>


class QEvent;
class QWidget;

class QMenu;

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

    private:
        std::unique_ptr<QMenu> const m_menu;
        QIcon m_icon;
        QString m_text;
};
