/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/profile/cprofilemgr.h"

#include <QDir>
#include <QFile>
#include <QList>
#include "util/directory.h"


namespace Profile {

CProfileMgr::CProfileMgr() : m_startupProfile(0) {
    namespace DU = util::directory;

    //m_profiles.setAutoDelete(true);

    m_profilePath = DU::getUserSessionsDir().absolutePath() + "/";

    QDir d( m_profilePath );
    QStringList files = d.entryList(QStringList("*.xml"));
    for ( QStringList::Iterator it = files.begin(); it != files.end(); ++it ) {
        if ((*it) != "_startup_.xml") {
            m_profiles.append(new CProfile(m_profilePath + *it));
        }
        else {
            m_startupProfile = new CProfile(m_profilePath + *it);
        }
    }
}

CProfileMgr::~CProfileMgr() {
    qDeleteAll(m_profiles);
    m_profiles.clear();
    delete m_startupProfile;
}

/** Returns a list of available profiles. */
const QList<CProfile*>& CProfileMgr::profiles() {
    return m_profiles;
}

/** Creates a new profile with the name "name" (first parameter).  @return The profile object */
CProfile *CProfileMgr::create(const QString &name) {
    CProfile* p = new CProfile(QString::null, name);
    m_profiles.append(p);

    return p;
}

/** Removes the profile from the list and from the directory containg the profile files. */
bool CProfileMgr::remove( CProfile* p ) {
    bool ret = false;
    QFile f( p->filename() );
    if (f.exists())
        f.remove();

    int i = m_profiles.indexOf(p);
    if (i != -1)
        delete m_profiles.takeAt(i);

    ret = true;
    return ret;
}

bool CProfileMgr::remove( const QString& profile) {
    bool ret = false;
    QListIterator<CProfile*> it(m_profiles);
    while (it.hasNext()) {
        CProfile* p = it.next();
        if (p->name() == profile) {
            remove(p);
            ret =  true;
        }
    }
    return ret;
}

/** Returns the profile with the desired name. If there's no such profile 0 is returned. */
CProfile* CProfileMgr::profile(const QString& name) {
    QListIterator<CProfile*> it(m_profiles);
    while (it.hasNext()) {
        CProfile* p = it.next();
        if (p && p->name() == name) {
            return p;
        }
    }

    return 0;
}

/** Returns the startup profile if it exists, otherwise return 0. */
CProfile* CProfileMgr::startupProfile() {
    if (!m_startupProfile) {
        m_startupProfile = new CProfile(QString::null, "_startup_");
    }

    return m_startupProfile;
}

/** Refreshes the profiles available on disk. Use this function to update the list of profiles after another instance of CProfileMgr created a new profile. */
void CProfileMgr::refresh() {
    qDeleteAll(m_profiles);
    m_profiles.clear(); //delete all profiles
    QDir d( m_profilePath );
    QStringList files = d.entryList(QStringList("*.xml"));
    for ( QStringList::Iterator it = files.begin(); it != files.end(); ++it ) {
        CProfile p(m_profilePath + *it);
        if (p.name() == "_startup_") { //new startup profile
            if  (!m_startupProfile) { //don't put this in the if clause above,it doesn't work!
                m_startupProfile = new CProfile(m_profilePath + *it);
            }
        }
        else if (!profile(p.name())) { //don't have it already
            m_profiles.append(new CProfile(m_profilePath + *it));
        }
    }
}

} //end of namespace Profile
