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

#include <QWidget>

#include "../../backend/cswordmodulesearch.h"
#include "../../backend/drivers/btmodulelist.h"
#include "chistorycombobox.h"


class QComboBox;
class QEvent;
class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QObject;
class QPushButton;
class QRadioButton;

namespace Search {

class BtSearchOptionsArea : public QWidget {
        Q_OBJECT
    public:

        BtSearchOptionsArea(QWidget *parent = nullptr);
        ~BtSearchOptionsArea() override;
        /*
        * Add text to search combox box history
        */
        void addToHistory(const QString& text);
        /**
        * Sets the search text used in the page.
        */
        void setSearchText(const QString& text);
        /**
        * Returns the search text set in this page.
        */
        QString searchText() const;

        CSwordModuleSearch::SearchType searchType();

        /**
          Returns the list of used modules.
        */
        BtConstModuleList const & modules() const { return m_modules; }

        /**
        * Sets all options back to the default.
        */
        void reset();
        /**
        * Returns the selected search scope if a search scope was selected.
        */
        sword::ListKey searchScope();

        /**
         * Opens the modules chooser dialog.
         */
        void chooseModules();

        /**
          Sets the modules used by the search.
        */
        void setModules(const BtConstModuleList &modules);

    private: // methods:
        /**
        * Initializes this page.
        */
        void initView();
        void initConnections();
        /**
        * Reads the settings of the last searchdialog session.
        */
        void readSettings();
        /**
        * Reads the settings for the searchdialog from disk.
        */
        void saveSettings();
        bool eventFilter(QObject* obj, QEvent* event) override;

        /**
        * Refreshes the list of ranges and the range combobox.
        */
        void refreshRanges();

        /**
          Get unique works list from the Works combobox
        */
        QStringList getUniqueWorksList();

    protected Q_SLOTS:

        /// \todo Implement validation
        #if 0
        void slotValidateText(const QString& newText);
        #endif

    Q_SIGNALS:
        void sigStartSearch();

    private:
        BtConstModuleList m_modules;

        QHBoxLayout *hboxLayout;
        QGroupBox *searchGroupBox;
        QGridLayout *gridLayout;
        QLabel *m_searchTextLabel;
        QPushButton* m_searchButton;
        QLabel* m_helpLabel;
        QRadioButton* m_typeAndButton;
        QRadioButton* m_typeOrButton;
        QRadioButton* m_typeFreeButton;
        QPushButton *m_chooseModulesButton;
        QPushButton *m_chooseRangeButton;
        QLabel *m_searchScopeLabel;
        QComboBox *m_rangeChooserCombo;
        CHistoryComboBox *m_searchTextCombo;
        QLabel *m_modulesLabel;
        QComboBox* m_modulesCombo;

};

}
