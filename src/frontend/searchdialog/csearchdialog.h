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

#include <QDialog>

#include <QString>
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
class CSearchDialog final: public QDialog {
        Q_OBJECT
    public:
        static void openDialog(BtConstModuleList modules,
                               QString const & searchText = QString(),
                               QWidget * parentDialog = nullptr);

        static void closeDialog();

    private:

        CSearchDialog(QWidget *parent);
        ~CSearchDialog();

        /**
        * Resets the parts to the default.
        */
        void reset(BtConstModuleList modules, QString const & searchText);

    private Q_SLOTS:
        /**
          Starts the search with the set modules and the set search text.
        */
        void startSearch();

    private:
        QPushButton* m_analyseButton;
        QPushButton* m_manageIndexes;
        QPushButton* m_closeButton;
        BtSearchResultArea* m_searchResultArea;
        BtSearchOptionsArea* m_searchOptionsArea;
};


} //end of namespace Search
