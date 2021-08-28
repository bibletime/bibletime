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

#include "cswordsettings.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include "../../util/cresmgr.h"
#include "btstandardworkstab.h"
#include "bttextfilterstab.h"
#include "cconfigurationdialog.h"


CSwordSettingsPage::CSwordSettingsPage(CConfigurationDialog * parent)
        : BtConfigDialog::Page(CResMgr::settings::sword::icon(), parent)
{
    static const QString nullString;

    m_tabWidget = new QTabWidget(this);
        m_worksTab = new BtStandardWorksTab(this);
        m_tabWidget->addTab(m_worksTab, nullString);

        m_filtersTab = new BtTextFiltersTab(this);
        m_tabWidget->addTab(m_filtersTab, nullString);


    QVBoxLayout * mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_tabWidget);

    retranslateUi();
}

void CSwordSettingsPage::retranslateUi() {
    setHeaderText(tr("Desk"));

    m_tabWidget->setTabText(m_tabWidget->indexOf(m_worksTab), tr("Standard works"));
    m_tabWidget->setTabText(m_tabWidget->indexOf(m_filtersTab), tr("Text filters"));
}

void CSwordSettingsPage::save() const {
    m_worksTab->save();
    m_filtersTab->save();
}
