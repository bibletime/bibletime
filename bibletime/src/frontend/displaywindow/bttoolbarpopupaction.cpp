/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "bttoolbarpopupaction.h"
#include <QMenu>
#include <QToolButton>
#include <QAction>
#include <QEvent>

class BtToolButton : public QToolButton
{
public:
	BtToolButton(QWidget* parent=0) : QToolButton(parent)
	{
	}
private:
	void nextCheckState()
	{
	}
};


// This class provides a toolbar widget that has a icon plus a right side down arrow
// The icon is typically set to a back or forward arrow and the down arrow has a popup 
// menu when clicked. The menu is typicallly populated with history actions. 
BtToolBarPopupAction::BtToolBarPopupAction(const QIcon& icon, const QString& text, QObject* parent)
	: QWidgetAction(parent), m_icon(icon), m_text(text)
{
	setText(text);
	m_menu = new QMenu();
}

BtToolBarPopupAction::~BtToolBarPopupAction()
{
	delete m_menu;
}

// return the QMenu object so a popup menu can be constructed
QMenu* BtToolBarPopupAction::popupMenu() const
{
	return m_menu;
}

QWidget* BtToolBarPopupAction::createWidget(QWidget* parent)
{
	m_button = new BtToolButton(parent);
	setIcon(m_icon);
	setToolTip(m_text);
	m_button->setDefaultAction(this);
	m_button->setPopupMode(QToolButton::MenuButtonPopup);
	m_button->setMenu(m_menu);
	bool ok = connect(m_button, SIGNAL(pressed()), this, SLOT(buttonPressed()));
	Q_ASSERT(ok);;
	return m_button;
}

// Slot to emit a triggered signal when the toolbar button is pressed
void BtToolBarPopupAction::buttonPressed()
{
	emit triggered();
}

// Function to catch the Shortcut event and emit the triggered signal
bool BtToolBarPopupAction::event(QEvent *event)
{
    if (event->type() == QEvent::Shortcut) 
	{
		emit triggered();
		return true;
	}
	return QWidgetAction::event(event);
}
