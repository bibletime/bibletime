/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include <QProgressDialog>


class CSwordModuleInfo;

/**
  This dialog is used to index a list of modules and to show progress for that.
  While the indexing is in progress it creates a blocking, top level dialog
  which shows the progress while the indexing is done.
*/
class BtModuleIndexDialog: public QProgressDialog {

    Q_OBJECT

public: // methods:

    /**
      Creates and shows the indexing progress dialog and starts the actual
      indexing. It shows the dialog with progress information. In case indexing
      some module is unsuccessful or cancelled, any indices that were created
      for other given modules are deleted. After indexing, the dialog is closed.
      \param[in] modules The list of modules to index.
      \pre all given modules are unindexed
      \returns whether the indexing was finished successfully.
    */
    static bool indexAllModules(QList<CSwordModuleInfo *> const & modules);

private: // methods:

    BtModuleIndexDialog(int numModules);

    /**
      Shows the indexing progress dialog and starts the actual indexing. It
      shows the dialog with progress information. In case indexing some module
      is unsuccessful or cancelled, any indices that were created for other
      given modules are deleted. After indexing, the dialog is closed.
      \param[in] modules The list of modules to index.
      \pre all given modules are unindexed
      \returns whether the indexing was finished successfully.
    */
    bool indexAllModulesPrivate(QList<CSwordModuleInfo *> const & modules);

private: // fields:

    int m_currentModuleIndex;

};
