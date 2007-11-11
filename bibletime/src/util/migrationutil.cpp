/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "migrationutil.h"
#include "config.h"
#include "directoryutil.h"

#include "frontend/cbtconfig.h"

#include "kglobal.h"
#include "klocale.h"

#include <QMessageBox>

using namespace util::filesystem;
namespace util{

bool MigrationUtil::checkMigration(){
	return CBTConfig::get(CBTConfig::bibletimeVersion) != BT_VERSION;
}

void MigrationUtil::doMigration(){
	if (CBTConfig::get(CBTConfig::bibletimeVersion) == "NOT YET INSTALLED"){
		//No old version found, so we try our migration from KDE 3.
		fromKDE3Migration();
	}
}

void MigrationUtil::fromKDE3Migration(){
	//Migration code for KDE 4 port
	//List of potential old KDE directories to load data from.
	QString searchDirs[8] = {"/.kde", "/.kde3", "/.kde3.5",
								"/.kde3.4", "/.kde3.3", "/.kde3.2",
								"/.kde3.1", "/.kde3.0"};
	for (int i = 0; i < 8; i++){
		QString currSearch = QDir::homePath() + searchDirs[i];
		QDir searchHome(currSearch);
		QFile oldRc(currSearch + "/share/config/bibletimerc");
		if (oldRc.exists()){
			QMessageBox msg (QMessageBox::Question, i18n("Settings"
				" Migration"),
				i18n("It"
				" appears you have a BibleTime configuration from KDE"
				" 3 stored in %1, and you have not migrated it to"
				" this version.  Would you like to import it?",
				currSearch), QMessageBox::Yes | QMessageBox::No);
			int result = msg.exec();
			if (result != QMessageBox::Yes){
				break;
			}
			//Copy our old bibletimerc into the new KDE4 directory.
			QString newRcLoc = DirectoryUtil::getUserBaseDir().absolutePath() + "/bibletimerc";
			QFile newRc(newRcLoc);
			newRc.remove();
			oldRc.copy(newRcLoc);
			QFile oldBookmarks(currSearch +
					"/share/apps/bibletime/bookmarks.xml");
			if (oldBookmarks.exists()){
				QString newBookmarksLoc = DirectoryUtil::getUserBaseDir().absolutePath() + "/" + "bookmarks.xml";
				QFile newBookmarks(newBookmarksLoc);
				newBookmarks.remove();
				oldBookmarks.copy(newBookmarksLoc);
			}
			QDir sessionDir(currSearch +
					"/share/apps/bibletime/sessions");
			if (sessionDir.exists()){
				DirectoryUtil::copyRecursive(
				sessionDir.absolutePath(),
				DirectoryUtil::getUserSessionsDir().absolutePath());
			}
			else{
				QDir oldSessionDir(currSearch + "/share/apps/bibletime/profiles");
				if (oldSessionDir.exists()){
					DirectoryUtil::copyRecursive(
							oldSessionDir.absolutePath(),
							DirectoryUtil::getUserSessionsDir().absolutePath());
				}
			}
			//We found at least a config file, so we are done
			//searching for migration data.
			break;
		}
	}
	CBTConfig::getConfig()->reparseConfiguration();
}

}

