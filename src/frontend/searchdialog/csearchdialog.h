/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CSEARCHDIALOG_H
#define CSEARCHDIALOG_H

#include <QDialog>

#include <QString>
#include "backend/cswordmodulesearch.h"
#include "backend/managers/cswordbackend.h"
#include "util/cpointers.h"


namespace Search {
class BtSearchResultArea;
class BtSearchOptionsArea;
}
class QPushButton;
class QWidget;

namespace Search {

/**
  *@author The BibleTime team
  */
class CSearchDialog : public QDialog {
        Q_OBJECT
    public:
        static void openDialog(const QList<CSwordModuleInfo*> modules, const QString& searchText = QString::null, QWidget* parentDialog = 0);
		static void closeDialog();

    protected:
        friend class CSearchAnalysisScene;
        friend class CSearchResultArea;
        friend class BtSearchResultArea;
        friend class BibleTime;

        /**
        * Only interesting for the class members! Useful to get the searched text etc.
        */
        static CSearchDialog* getSearchDialog();

        /**
        * The constructor of the dialog. It's protected because you should use the static public function openDialog.
        * The dialog destroys itself if it was closed.
        */
        CSearchDialog(QWidget *parent);
        ~CSearchDialog();

        /**
         * Initializes this object.
         */
        void initView();
        /**
        * Starts the search with the given module list and given search text.
        * Doesn't wait for the start button press, starts immediately
        */
        void startSearch( const QList<CSwordModuleInfo*> modules, const QString& searchText);
        /**Prepares the search string given by user for a specific search type */
        QString prepareSearchText(const QString& orig);
        /**
        * Sets the list of modules for the search.
        */
        void setModules( const QList<CSwordModuleInfo*> modules );
        /**
        * Returns the list of used modules.
        */
        QList<CSwordModuleInfo*> modules() const;
        /**
        * Sets the search text which is used for the search.
        */
        void setSearchText( const QString searchText );
        /**
        * Returns the search text which is set currently.
        */
        QString searchText() const;
        /**
        * Returns the used search scope as a list key
        */
        sword::ListKey searchScope();

        /**
        * Resets the parts to the default.
        */
        void reset();
        /**
        * Load the settings from the resource file
        */
        void loadDialogSettings();
        /**
        * Save the settings to the resource file
        */
        void saveDialogSettings();

    protected slots:
        /**
        * Starts the search with the set modules and the set search text.
        */
        void startSearch();
        void searchFinished();
        void showModulesSelector();
        /**
        * Initializes the signal slot connections
        */
        void initConnections();

        void closeButtonClicked();

    private:
        QPushButton* m_analyseButton;
        QPushButton* m_closeButton;
        BtSearchResultArea* m_searchResultArea;
        BtSearchOptionsArea* m_searchOptionsArea;

        CSwordModuleSearch m_searcher;
};


} //end of namespace Search

#endif
