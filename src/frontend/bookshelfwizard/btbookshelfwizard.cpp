/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfwizard/btbookshelfwizard.h"

#include <QApplication>
#include <QByteArray>
#include "backend/config/btconfig.h"
#include "frontend/bookshelfwizard/btbookshelfinstallfinalpage.h"
#include "frontend/bookshelfwizard/btbookshelflanguagespage.h"
#include "frontend/bookshelfwizard/btbookshelfremovefinalpage.h"
#include "frontend/bookshelfwizard/btbookshelfsourcespage.h"
#include "frontend/bookshelfwizard/btbookshelfsourcesprogresspage.h"
#include "frontend/bookshelfwizard/btbookshelftaskpage.h"
#include "frontend/bookshelfwizard/btbookshelfwizardenums.h"
#include "frontend/bookshelfwizard/btbookshelfworkspage.h"


namespace {
QString const GeometryKey = "GUI/BookshelfWizard/geometry";
QString const SourcesKey = "GUI/BookshelfWizard/sources";
QString const LanguagesKey = "GUI/BookshelfWizard/languages";
} // anonymous namespace

BtBookshelfWizard::BtBookshelfWizard(QWidget * parent, Qt::WindowFlags flags)
    : QWizard(parent, flags)
    , m_taskPage(new BtBookshelfTaskPage)
    , m_sourcesProgressPage(new BtBookshelfSourcesProgressPage)
    , m_sourcesPage(new BtBookshelfSourcesPage)
    , m_languagesPage(new BtBookshelfLanguagesPage)
    , m_installWorksPage(new BtBookshelfWorksPage(WizardTaskType::installWorks))
    , m_updateWorksPage(new BtBookshelfWorksPage(WizardTaskType::updateWorks))
    , m_removeWorksPage(new BtBookshelfWorksPage(WizardTaskType::removeWorks))
    , m_removeFinalPage(new BtBookshelfRemoveFinalPage)
    , m_installFinalPage(new BtBookshelfInstallFinalPage)  // For install and update
{
    addPage(m_taskPage);
    addPage(m_sourcesProgressPage);
    addPage(m_sourcesPage);
    addPage(m_languagesPage);
    addPage(m_installWorksPage);
    addPage(m_updateWorksPage);
    addPage(m_removeWorksPage);
    addPage(m_removeFinalPage);
    addPage(m_installFinalPage);

    QRect rect = geometry();
    rect.setWidth(780);
    rect.setHeight(600);
    setGeometry(rect);
    setOption(QWizard::NoBackButtonOnLastPage);

    retranslateUi();

    // Load wizard geometry:
    restoreGeometry(btConfig().value<QByteArray>(GeometryKey, QByteArray()));
}

void BtBookshelfWizard::retranslateUi() {
    setWindowTitle(QApplication::translate("BookshelfWizard",
                                           "Bookshelf Manager"));
}

QStringList BtBookshelfWizard::selectedSources() const
{ return m_sourcesPage->selectedSources(); }

QStringList BtBookshelfWizard::selectedLanguages() const
{ return m_languagesPage->selectedLanguages(); }

void BtBookshelfWizard::accept() {
    if (currentPage() == m_installFinalPage) {
        // Save settings:
        btConfig().setValue(SourcesKey, selectedSources());
        btConfig().setValue(LanguagesKey, selectedLanguages());
    }

    btConfig().setValue(GeometryKey, saveGeometry()); // Save wizard geometry
    QDialog::accept();
}

BtModuleSet BtBookshelfWizard::selectedWorks() const {
    WizardTaskType const iType = m_taskPage->taskType();
    if (iType == WizardTaskType::installWorks)
        return m_installWorksPage->checkedModules();
    if (iType == WizardTaskType::updateWorks)
        return m_updateWorksPage->checkedModules();
    BT_ASSERT(iType == WizardTaskType::removeWorks);
    return m_removeWorksPage->checkedModules();
}

WizardTaskType BtBookshelfWizard::taskType() const
{ return m_taskPage->taskType(); }

QString BtBookshelfWizard::installPath() const {
    WizardTaskType const iType = m_taskPage->taskType();
    if (iType == WizardTaskType::installWorks)
        return m_installWorksPage->installPath();
    BT_ASSERT(iType == WizardTaskType::updateWorks);
    return m_updateWorksPage->installPath();
}
