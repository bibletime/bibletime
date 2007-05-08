/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef KSTARTUPLOGO_H
#define KSTARTUPLOGO_H

#include <qwidget.h>

//class forward declarations
class QLabel;

/**
 * This class provides a startuplogo.
 *
 * @author The team of BibleTime
 */
class KStartupLogo : public QWidget {
public:
	/**
	* Create the splash screen and show it on the screen.
	* Use hideSplash, raiseSplash and lowerSplash() to control the visibility on screen.
	*/
	static void createSplash();
	/**
	* Show the splash screen on the users desktop.
	*/
	static void showSplash();
	/**
	* Hides the splash screen.
	*/
	static void hideSplash();
	/**
	* Delete the splash screen if it exists.
	*/
	static void deleteSplash();
	/**
	* Sets the text status message in the splash screen.
	*/
	static void setStatusMessage(const QString& message);
	/**
	* Makes the splashscreen the toplevel window.
	*/
	static void raiseSplash();
	/**
	* Lowers the splash screen one window down,
	* so it's possible to make it not to hide tip windows or other startup stuff.
	*/
	static void lowerSplash();

private:
	KStartupLogo();
	void setText(const QString text);
	QLabel* textLabel;

	static KStartupLogo* startupLogo;
};

#endif
