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

#include "btconfigdialog.h"

#include <QObject>
#include <QString>


class BtTextFiltersTab;
class CConfigurationDialog;
class BtStandardWorksTab;
class QTabWidget;

class CSwordSettingsPage: public BtConfigDialog::Page {

        Q_OBJECT

    public: /* Methods: */

        CSwordSettingsPage(CConfigurationDialog * parent = nullptr);

        void save() const final override;

    protected: /* Methods: */

        void retranslateUi();

    private: /* Fields: */

        QTabWidget * m_tabWidget;
        BtStandardWorksTab * m_worksTab;
        BtTextFiltersTab * m_filtersTab;

};
