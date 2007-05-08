/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cprofilewindow.h"

namespace Profile {

CProfileWindow::CProfileWindow(CSwordModuleInfo::ModuleType type)
: m_type(type),
m_windowGeometry(),
m_moduleList(),
m_key(QString::null),
m_maximized(false),
m_hasFocus(false),
m_windowSettings(0),
m_writeWindowType(0) {
	m_scrollbarPos.horizontal = 0;
	m_scrollbarPos.vertical = 0;
}

CProfileWindow::~CProfileWindow() {}

/** Returns the size of the window including the x,y coordinates. */
const QRect& CProfileWindow::geometry() const {
	return m_windowGeometry;
}

/** Sets the size of the window. */
void CProfileWindow::setGeometry( const QRect& rect ) {
	m_windowGeometry = rect;
}

/** Returns the type of the managed window (bible window, commentary window or lexicon window). */
const CSwordModuleInfo::ModuleType CProfileWindow::type() const {
	if (m_type != CSwordModuleInfo::Unknown)
	return m_type;
	return CSwordModuleInfo::Unknown;
}

/** Sets the modules. */
void CProfileWindow::setModules( const QStringList& modules ) {
	m_moduleList = modules; //copy module pointers into our own list
}

/** Returns a list of module names which are chosen in the managed window profile. */
const QStringList& CProfileWindow::modules() const {
	return m_moduleList;
}

/** Returns the current key set in the modules. */
const QString& CProfileWindow::key() const {
	return m_key;
}

/** Set the key used in the modules. */
void CProfileWindow::setKey( const QString& key ) {
	m_key = key;
}

/** Sets the current position of the scrollbars. */
void CProfileWindow::setScrollbarPositions(const int& horizontal, const int& vertical) {
	// m_scrollbarPos = {x,y};
	m_scrollbarPos.horizontal = horizontal;
	m_scrollbarPos.vertical = vertical;
}

/** Returns tghe position of the scrollbars */
const CProfileWindow::ScrollbarPos& CProfileWindow::scrollbarPositions() const {
	return m_scrollbarPos;
}

/** Sets the type of the used modules. */
void CProfileWindow::setType(const CSwordModuleInfo::ModuleType& type) {
	m_type = type;
}

/** Returns true if the window is maximized. */
const bool& CProfileWindow::maximized() const {
	return m_maximized;
}

/** Sets the windows maximized state to true or false. */
void CProfileWindow::setMaximized( const bool& maximized ) {
	m_maximized = maximized;
}

/** Returns true if the window is maximized. */
const bool& CProfileWindow::hasFocus() const {
	return m_hasFocus;
}

/** Sets the windows maximized state to true or false. */
void CProfileWindow::setFocus( const bool& hasFocus ) {
	m_hasFocus = hasFocus;
}

/** Returns an integer with the window specific settings */
const int& CProfileWindow::windowSettings() const {
	return m_windowSettings;
}

/** Sets the window specific settings. */
void CProfileWindow::setWindowSettings( const int& settings ) {
	m_windowSettings = settings;
}

/** Tells this profile window to represent a write window. */
void CProfileWindow::setWriteWindowType( const int& writeWindowType ) {
	m_writeWindowType = writeWindowType;
}

/** Returns whether this profile window represents a write window. */
const int& CProfileWindow::writeWindowType() const {
	return m_writeWindowType;
}

} //end of namespace Profile
