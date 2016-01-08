/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/config/btconfig.h"
#include "frontend/bookshelfwizard/btbookshelfworkspage.h"
#include "frontend/bookshelfwizard/btbookshelfinstallfinalpage.h"
#include "frontend/bookshelfwizard/btbookshelflanguagespage.h"
#include "frontend/bookshelfwizard/btbookshelfremovefinalpage.h"
#include "frontend/bookshelfwizard/btbookshelfsourcespage.h"
#include "frontend/bookshelfwizard/btbookshelfsourcesprogresspage.h"
#include "frontend/bookshelfwizard/btbookshelftaskpage.h"
#include "frontend/bookshelfwizard/btbookshelfwizard.h"
#include "frontend/bookshelfwizard/btbookshelfwizardenums.h"

#include <QApplication>
#include <QByteArray>

namespace {
const QString GeometryKey = "GUI/BookshelfWizard/geometry";
const QString SourcesKey = "GUI/BookshelfWizard/sources";
const QString LanguagesKey = "GUI/BookshelfWizard/languages";
} // anonymous namespace

BtBookshelfWizard::BtBookshelfWizard(QWidget *parent,
                                     Qt::WindowFlags flags)
    : QWizard(parent, flags),
      m_taskPage(new BtBookshelfTaskPage),
      m_sourcesProgressPage(new BtBookshelfSourcesProgressPage),
      m_sourcesPage(new BtBookshelfSourcesPage),
      m_languagesPage(new BtBookshelfLanguagesPage),
      m_installWorksPage(new BtBookshelfWorksPage(WizardTaskType::installWorks)),
      m_updateWorksPage(new BtBookshelfWorksPage(WizardTaskType::updateWorks)),
      m_removeWorksPage(new BtBookshelfWorksPage(WizardTaskType::removeWorks)),
      m_removeFinalPage(new BtBookshelfRemoveFinalPage),
      m_installFinalPage(new BtBookshelfInstallFinalPage)  // For install and update
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
    setOption(QWizard::NoCancelButtonOnLastPage);
    setOption(QWizard::NoBackButtonOnLastPage);

    retranslateUi();
    loadWizardGeometry();
}

void BtBookshelfWizard::retranslateUi() {
    setWindowTitle(QApplication::translate("BookshelfWizard", "Bookshelf Manager", 0));
}

QStringList BtBookshelfWizard::selectedSources() const {
    return m_sourcesPage->selectedSources();
}

QStringList BtBookshelfWizard::selectedLanguages() const {
    return m_languagesPage->selectedLanguages();
}

void BtBookshelfWizard::accept() {
    if (currentPage() == m_installFinalPage)
        saveSettings();

    saveWizardGeometry();
    QDialog::accept();
}

void BtBookshelfWizard::loadWizardGeometry() {
    restoreGeometry(btConfig().value<QByteArray>(GeometryKey, QByteArray()));
}

void BtBookshelfWizard::saveWizardGeometry() const {
    btConfig().setValue(GeometryKey, saveGeometry());
}

void BtBookshelfWizard::saveSettings() const {
    btConfig().setValue(SourcesKey,selectedSources());
    btConfig().setValue(LanguagesKey,selectedLanguages());
}

BtModuleSet BtBookshelfWizard::selectedWorks() const {
    WizardTaskType iType = m_taskPage->taskType();
    if (iType == WizardTaskType::installWorks)
        return m_installWorksPage->checkedModules();
    if (iType == WizardTaskType::updateWorks)
        return m_updateWorksPage->checkedModules();
    return m_removeWorksPage->checkedModules();
}

WizardTaskType BtBookshelfWizard::taskType() const {
    return m_taskPage->taskType();
}

QString BtBookshelfWizard::installPath() const {
    WizardTaskType iType = m_taskPage->taskType();
    if (iType == WizardTaskType::installWorks)
        return m_installWorksPage->installPath();
    if (iType == WizardTaskType::updateWorks)
        return m_updateWorksPage->installPath();
    BT_ASSERT(false);
    return QString();
}

BtBookshelfLanguagesPage * BtBookshelfWizard::languagesPage() {
    return m_languagesPage;
}
