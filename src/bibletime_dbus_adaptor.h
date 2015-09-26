/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef NO_DBUS

#ifndef BIBLETIME_DBUS_ADAPTOR_H
#define BIBLETIME_DBUS_ADAPTOR_H

#include "bibletime.h"

#ifndef NO_DBUS
#include <QDBusAbstractAdaptor>
#endif
#include <QString>
#include <QStringList>


class BibleTimeDBusAdaptor: QDBusAbstractAdaptor {

    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "info.bibletime.BibleTime")

public: /* Methods: */

    BibleTimeDBusAdaptor(BibleTime *pBibleTime);

public slots:

    /**
      Sync all open Bible windows to the key.
      \param[in] key The key which is set to all Bible windows.
    */
    void syncAllBibles(const QString &key);

    /**
      Sync all open commentary windows to the key.
      \param[in] key The key which is set to all Commentary windows.
    */
    void syncAllCommentaries(const QString &key);

    /**
      Sync all open lexicon windows to the key.
      \param[in] key The key which is set to all Lexicon windows.
    */
    void syncAllLexicons(const QString &key);

    /**
      Sync all open verse based (i.e. Bibles and commentaries) windows to the key.
      \param key The key which is set to all Bible and Commentary windows.
    */
    void syncAllVerseBasedModules(const QString &key);

    /**
      Open a new read window for the module moduleName using the given key
      \param[in] moduleName The name of the module which is opened in a new module window.
      \param[in] key The key to set to the newly opened window.
    */
    void openWindow(const QString &moduleName, const QString &key);

    /**
      Open a new read window for the default Bible module using the given key
      \param[in] key The key to set to the newly opened window.
    */
    void openDefaultBible(const QString &key);

    /**
      Close all open windows.
    */
    void closeAllModuleWindows();

    /**
      Returns the reference used in the current window.
      The format of the returned reference is
         [Module] [Type] OSIS_Reference,
      with type one of BIBLE/COMMENTARY/BOOK/LEXICON/UNSUPPORTED
      If the type is BIBLE or COMMENTARY the reference is an OSIS ref
      in the other cases it's the key name, for books /Chapter/Subsection
      for Lexicons just the plain key, e.g. "ADAM".
      e.g.
             [KJV] [BIBLE]    Gen.1.1
              [MHC] [COMMENTARY]  Gen.1.1
              [ISBE] [LEXICON]  REDEMPTION
      \returns The reference displayed in the currently active module window.
               Empty if none is active.
    */
    QString getCurrentReference();

    /**
      Search the searchText in the specified module.
      \param[in] moduleName The module to search in
      \param[in] searchText Search for this in the modules
      \returns The search result. It's in the format [modulename]
               osis_ref_of_the_found_key. For example "[KJV] Gen.1.1".
    */
    QStringList searchInModule(const QString &moduleName, const QString &searchText) ;

    /**
      Search in all open modules and return the search result.
      The result is in the same format as searchInModule
      \param[in] searchText Search for this in the modules
      \returns The search result for a searchin all opened module windows
      \see searchInModule For the search result format.
    */
    QStringList searchInOpenModules(const QString &searchText);

    /**
      Search in the default Bible module and return the search result.
      The result is in the same format as searchInModule
      \param[in] searchText Search for this in the modules
      \returns The search result for a search in the default Bible
      \see searchInModule
    */
    QStringList searchInDefaultBible(const QString &searchText);

    /**
      Return a list of modules of the given type.
      \param[in] type One of BIBLES, COMMENTARIES, LEXICONS, BOOKS
      \returns The list of modules of the given type, may be empty
    */
    QStringList getModulesOfType(const QString &type);

private: /* Fields: */

    BibleTime *m_bibletime;

}; /* class BibleTimeDBusAdaptor */

#endif /* #ifdef NO_DBUS */

#endif /* #ifdef BIBLETIME_DBUS_ADAPTOR_H */
