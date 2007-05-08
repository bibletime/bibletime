/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CPROFILEMGR_H
#define CPROFILEMGR_H

#include "cprofile.h"

//Qt includes
#include <qstring.h>

namespace Profile {
/** The manager for profiles.
 * Provides functions to create, delete, save and load profiles.
 * @author The BibleTime team
 */
class CProfileMgr {
public:
	CProfileMgr();
	~CProfileMgr();

	/** Creates a new profile with the name "name" (first parameter).
	* @return The profile object
	*/
	CProfile* create( const QString name );
	/**
	* @return a list of available profiles
	*/
	const QPtrList<CProfile>& profiles();
	/**
	* Removes the profile from the list and from the directory containg the profile files.
	*/
	const bool remove
		( CProfile* p );
	/**
	* Removes the profile from the list and from the directory containg the profile files.
	*/
	const bool remove
		( const QString& );
	/**
	* Returns the profile with the desired name. If there's no such profile 0 is returned.
	*/
	CProfile* profile(const QString&);
	/**
	* Returns the startup profile if it exists, otherwise return 0.
	*/
	CProfile* startupProfile();
	/**
	* Refreshes the profiles available on disk. Use this function to update the list of profiles after another instance of CProfileMgr created a new profile.
	*/
	void refresh();

protected:
	QPtrList<CProfile> m_profiles;
	QString m_profilePath;
	CProfile* m_startupProfile;
};

} //end of namespace Profile

#endif
