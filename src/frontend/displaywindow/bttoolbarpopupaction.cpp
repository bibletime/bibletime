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

#include "bttoolbarpopupaction.h"

#include <QEvent>
#include <QMenu>
#include <QToolButton>
#include "../../util/btconnect.h"


namespace {

class BtToolButton: public QToolButton {
    public:
        BtToolButton(QWidget *parent = nullptr)
            : QToolButton(parent) {}
    private:
        void nextCheckState() override {}
};

} // anonymous namespace


// This class provides a toolbar widget that has a icon plus a right side down arrow
// The icon is typically set to a back or forward arrow and the down arrow has a popup
// menu when clicked. The menu is typicallly populated with history actions.
BtToolBarPopupAction::BtToolBarPopupAction(QIcon const & icon,
                                           QString const & text,
                                           QObject * parent)
    : QWidgetAction(parent)
    , m_menu(std::make_unique<QMenu>())
    , m_icon(icon)
    , m_text(text)
{ setText(text); }

BtToolBarPopupAction::~BtToolBarPopupAction() = default;

// return the QMenu object so a popup menu can be constructed
QMenu * BtToolBarPopupAction::popupMenu() const { return m_menu.get(); }

QWidget* BtToolBarPopupAction::createWidget(QWidget* parent) {
    auto * const button = new BtToolButton(parent);
    setIcon(m_icon);
    setToolTip(m_text);
    button->setDefaultAction(this);
    button->setPopupMode(QToolButton::MenuButtonPopup);
    button->setMenu(m_menu.get());
    BT_CONNECT(button, &BtToolButton::pressed,
               this /* Meeded */, [this] { Q_EMIT triggered(); });
    return button;
}

// Function to catch the Shortcut event and emit the triggered signal
bool BtToolBarPopupAction::event(QEvent *event) {
    if (event->type() == QEvent::Shortcut) {
        Q_EMIT triggered();
        return true;
    }
    return QWidgetAction::event(event);
}
