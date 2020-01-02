/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef CSEARCHDIALOG_H
#define CSEARCHDIALOG_H

#include <QDialog>

#include <QString>
#include "../../backend/cswordmodulesearch.h"
#include "../../backend/managers/cswordbackend.h"
#include "btsearchoptionsarea.h"

namespace Search {
class BtSearchResultArea;
}
class QPushButton;
class QWidget;

namespace Search {

/**
  \note destroys itself on close
*/
class CSearchDialog : public QDialog {
        Q_OBJECT
    public:
        static void openDialog(const BtConstModuleList modules,
                               const QString &searchText = QString(),
                               QWidget *parentDialog = nullptr);

        static void closeDialog();

    protected:

        friend class CSearchAnalysisScene;
        friend class BtSearchResultArea;

        /**
        * Only interesting for the class members! Useful to get the searched text etc.
        */
        static CSearchDialog* getSearchDialog();

        CSearchDialog(QWidget *parent);
        ~CSearchDialog();

        /**
         * Initializes this object.
         */
        void initView();

        /**
          Starts the search with the given module list and given search text.
        */
        void startSearch(const BtConstModuleList modules,
                         const QString &searchText);

        /**
          Sets the list of modules for the search.
        */
        void setModules(const BtConstModuleList modules) {
            m_searchOptionsArea->setModules(modules);
        }

        /**
          Returns the list of used modules.
        */
        inline BtConstModuleList modules() const {
            return m_searchOptionsArea->modules();
        }

        /**
        * Sets the search text which is used for the search.
        */
        void setSearchText( const QString &searchText );

        /**
          \returns the search text which is set currently.
        */
        QString searchText() const {
            return m_searchOptionsArea->searchText();
        }

        /**
          \returns the used search scope as a list key
        */
        inline sword::ListKey searchScope() const {
            return m_searchOptionsArea->searchScope();
        }

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
        void saveDialogSettings() const;

    protected slots:
        /**
          Starts the search with the set modules and the set search text.
        */
        void startSearch();

        void showModulesSelector();
        /**
        * Initializes the signal slot connections
        */
        void initConnections();

        void manageIndexesButtonClicked();

        void closeButtonClicked();

    private:
        QPushButton* m_analyseButton;
        QPushButton* m_manageIndexes;
        QPushButton* m_closeButton;
        BtSearchResultArea* m_searchResultArea;
        BtSearchOptionsArea* m_searchOptionsArea;

        CSwordModuleSearch m_searcher;
};


} //end of namespace Search

#endif
