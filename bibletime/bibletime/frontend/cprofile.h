/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CPROFILE_H
#define CPROFILE_H

#include "cprofilewindow.h"

//Qt includes
#include <qptrlist.h>

namespace Profile {

/** Manages one profile file. Provides functions to save and restore settings of the available display windows.
  * @author The BibleTime team
  */
class CProfile {
public:
	CProfile(const QString& fileName, const QString& name = QString::null);
	~CProfile();

	/**
	* Saves the profile to the file given in the constructor.
	* @param windows The list of windows available in the profile.
	*/
	const bool save( QPtrList<CProfileWindow> windows );
	/**
	* Saves the profile to the file given in the constructor.
	*/
	const bool save();
	/**
	* Loads the profile from the file given in the constructor.
	* @return The list of profiled window which exist in the profile.
	*/
	QPtrList<CProfileWindow> load();
	/**
	* Returns the name of this profile.
	*/
	const QString& name();
	/**
	* Returns the filename used for this profile.
	*/
	const QString& filename();
	/**
	* Initializes the XML for the first time (use to create a new profile)
	*/
	void init(const QString);
	/**
	* Chnages the name of this profile.
	*/
	void setName( const QString& );
	/**
	* Returns true if the main window was in fullscreen mode as the profile was saved.
	*/
	const bool fullscreen() const;
	/**
	* Set the parameter to true if the main window coveres the full screen size.
	*/
	void setFullscreen( const bool fullscreen );
	/**
	* Returns true if the main window was maximized as the profile was saved.
	*/
	const bool maximized() const;
	/**
	* Set the parameter to true if the main window is maximized.
	*/
	void setMaximized( const bool maximized );
	/**
	* Sets the geoemtry of the main window
	*/
	void setGeometry( const QRect rect );
	/**
	* Returns the geometry of the main window
	*/
	const QRect geometry();

private:
	/**
	* Loads the basic settings requires for proper operation.
	*/
	void loadBasics();
	void saveBasics();

	QPtrList<CProfileWindow> m_profileWindows;
	QString m_name;
	QString m_filename;
	bool m_fullscreen;
	bool m_maximized;
	QRect m_geometry;
};

} //end of namespace Profile

#endif
