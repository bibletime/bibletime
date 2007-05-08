/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef BIBLETIME_INTERFACE_H
#define BIBLETIME_INTERFACE_H

//Qt includes
#include <qstring.h>
#include <qstringlist.h>

//KDE includes
#include <dcopobject.h>
#include <kmainwindowiface.h>

/** DCOP interface definition for BibleTime.
* This is the interface definition for BibleTime's DCOP client interface. The main window class "BibleTime"
* inherits from this class and re-implements these pure virtual functions.
* It uses virtual inheritance so that a widget which inherits twice from DCOPObject does not get mixed up.
* This file is turned into bibletimeinterface_skel.cpp by the KDE automake system.
*
* @author Joachim Ansorg
*/
class BibleTimeInterface : virtual public DCOPObject {
	//This K_DCOP line is required so that the k_dcop: and void are processed.
	K_DCOP

public:
BibleTimeInterface(QCString id) : DCOPObject(id) {}

k_dcop:
	/** Sync all open Bible windows to the key.
	* @param key The key which is set to all Bible windows.
	*/
	virtual void syncAllBibles(const QString& key) = 0;
	/** Sync all open commentary windows to the key.
	* @param key The key which is set to all Commentary windows.
	*/
	virtual void syncAllCommentaries(const QString& key) = 0;
	/** Sync all open lexicon windows to the key.
	* @param key The key which is set to all Lexicon windows.
	*/
	virtual void syncAllLexicons(const QString& key) = 0;
	/** Sync all open verse based (i.e. Bibles and commentaries) windows to the key.
	* @param key The key which is set to all Bible and Commentary windows.
	*/
	virtual void syncAllVerseBasedModules(const QString& key) = 0;
	/** Reload all modules
	*/
	virtual void reloadModules() = 0;
	/** Open a new read window for the module moduleName using the given key
	* @param moduleName The name of the module which is opened in a new module window.
	* @param key The key to set to the newly opened window.
	*/
	virtual void openWindow(const QString& moduleName, const QString& key) = 0;
	/** Open a new read window for the default Bible module using the given key
	* @param key The key to set to the newly opened window.
	*/
	virtual void openDefaultBible(const QString& key) = 0;
	/** Close all open windows.
	*/
	virtual void closeAllModuleWindows() = 0;
	/** Returns the reference used in the current window.
	* The format of the returned reference is
	*	[Module] [Type] OSIS_Reference,
	* wtih type one of BIBLE/COMMENTARY/BOOK/LEXICON/UNSUPPORTED
	* If the type is BIBLE or COMMENTARY the reference is an OSIS ref
	* in the other cases it's the key name, for books /Chapter/Subsection
	* for Lexicons just the plain key, e.g. "ADAM".
	* e.g.
	*		[KJV] [BIBLE]	Gen.1.1
	* 		[MHC] [COMMENTARY]  Gen.1.1
	* 		[ISBE] [LEXICON]  REDEMPTION
	* @return The reference displayed in the currently active module window. Empty if none is active.
	*/
	virtual QString getCurrentReference() = 0;
	/** Seach the searchText in the specified module.
	* @param moduleName The module to search in
	* @param searchText Search for this in the modules
	* @return The search result. It's in the format [modulename] osis_ref_of_the_found_key. For example "[KJV] Gen.1.1".
	*/
	virtual QStringList searchInModule(const QString& moduleName, const QString& searchText) = 0;
	/** Search in all open modules and return the search result.
	* The result is in the same format as searchInModule
	* @param searchText Search for this in the modules
	* @return The search result for a searchin all opened module windows
	* @see searchInModule For the search result format.
	*/
	virtual QStringList searchInOpenModules(const QString& searchText) = 0;
	/** Search in the default Bible module and return the search result.
	* The result is in the same format as searchInModule
	* @param searchText Search for this in the modules
	* @return The search result for a search in the default Bible
	* @see searchInModule
	*/
	virtual QStringList searchInDefaultBible(const QString& searchText) = 0;
	/** Return a list of modules of the given type.
	* @param type One of BIBLES, COMMENTARIES, LEXICONS, BOOKS
	* @return The list of modules of the given type, may be empty
	*/
	virtual QStringList getModulesOfType(const QString& type) = 0;
};

#endif
