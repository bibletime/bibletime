/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CMODULEINDEXDIALOG_H
#define CMODULEINDEXDIALOG_H

#include <QObject>


class CSwordModuleInfo;
class QProgressDialog;

/**
 * This dialog is used to index a list of modules and to show progress for that.\
 * While the indexing is in progress it creates a blocking, top level dialog which shows the progress
 * while the indexing is done.
 *
 * @author The BibleTime team <info@bibletime.info>
 \todo Rename to CModuleIndexer, since this is not a dialog!
*/
class CModuleIndexDialog : public QObject {
        Q_OBJECT
    public:
        /** Get the singleton instance.
         *
         */
        static CModuleIndexDialog* getInstance();

        /**
          Starts the actual indexing. It shows the dialog with progress
          information.
          \returns whether the indexing was successful.
        */
        bool indexAllModules(const QList<const CSwordModuleInfo*> &modules);

        /**
          Indexes all modules in the list which don't have an index yet.
          \returns whether the indexing was successful.
        */
        bool indexUnindexedModules(const QList<const CSwordModuleInfo*> &modules);

    signals:
        /** Indexing is cancelled programmatically. */
        void sigCancel();

    private:
        QProgressDialog* m_progress;
        int m_currentModuleIndex;

    protected slots:
        void slotModuleProgress( int percentage );
        void slotFinished();
        void slotSwordSetupChanged();
};

#endif
