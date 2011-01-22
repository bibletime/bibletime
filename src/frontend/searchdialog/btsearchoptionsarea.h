/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTSEARCHOPTIONSAREA_H
#define BTSEARCHOPTIONSAREA_H

#include <QWidget>

#include "backend/keys/cswordversekey.h"
#include "frontend/searchdialog/chistorycombobox.h"


class CSwordModuleInfo;
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

        enum SearchType {AndType, OrType, FullType};

        friend class CSearchDialog;

        BtSearchOptionsArea(QWidget *parent = 0);
        ~BtSearchOptionsArea();
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

        SearchType searchType();

        QPushButton* searchButton() const;

        /**
          Returns the list of used modules.
        */
        inline QList<const CSwordModuleInfo*> modules() const {
            return m_modules;
        }

        /**
        * Sets all options back to the default.
        */
        void reset();
        /**
        * Returns the selected search scope if a search scope was selected.
        */
        sword::ListKey searchScope();

        bool hasSearchScope();


    protected:
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
        bool eventFilter(QObject* obj, QEvent* event);

    public slots:
        /**
          Sets the modules used by the search.
        */
        void setModules(const QList<const CSwordModuleInfo*> &modules);

        /** Sets the modules when user selects them from the combobox.*/
        void moduleListTextSelected(int index);

        /**
        * Reimplementation.
        */
        void aboutToShow();
        /**
        * Refreshes the list of ranges and the range combobox.
        */
        void refreshRanges();
        /**
         * Opens the modules chooser dialog.
         */
        void chooseModules();

    protected slots:
        void setupRanges();
        void syntaxHelp();
        void slotSearchTextEditReturnPressed();
        void slotValidateText(const QString& newText);

    signals:
        void sigSetSearchButtonStatus(bool);
        void sigStartSearch();

    private:
        QList<const CSwordModuleInfo*> m_modules;

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

#endif
