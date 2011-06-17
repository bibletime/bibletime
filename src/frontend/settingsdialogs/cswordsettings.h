/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CSWORDSETTINGS_H
#define CSWORDSETTINGS_H

#include "frontend/bookshelfmanager/btconfigdialog.h"
#include <QWidget>


class CConfigurationDialog;
class StandardWorksTab;
class TextFiltersTab;

class CSwordSettingsPage: public BtConfigDialog::Page {

        Q_OBJECT

    public: /* Methods: */

        CSwordSettingsPage(CConfigurationDialog *parent = 0);

        void save();

    protected: /* Methods: */

        void retranslateUi();

    private: /* Fields: */

        QTabWidget *m_tabWidget;
        StandardWorksTab *m_worksTab;
        TextFiltersTab *m_filtersTab;

};

#endif
