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

QMenu* BtToolBarPopupAction::popupMenu() const
{
	return m_menu;
}

QWidget* BtToolBarPopupAction::createWidget(QWidget* parent)
{
	m_button = new QToolButton(parent);
	setIcon(m_icon);
	setToolTip(m_text);
	m_button->setDefaultAction(this);
	m_button->setPopupMode(QToolButton::MenuButtonPopup);
	m_button->setMenu(m_menu);
	bool ok = connect(m_button, SIGNAL(pressed()), this, SLOT(buttonPressed()));
	Q_ASSERT(ok);;
	return m_button;
}

void BtToolBarPopupAction::buttonPressed()
{
	emit triggered();
}

