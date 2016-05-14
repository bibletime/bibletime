/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfwizard/btbookshelfwizard.h"

#include <QApplication>
#include <QByteArray>
#include <QKeyEvent>
#include <QMessageBox>
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
    , m_downloadInProgress(false)
    , m_closeRequested(false)
    , m_closeMessageBox(new QMessageBox(this))
    , m_taskPage(new BtBookshelfTaskPage(this))
    , m_sourcesPage(new BtBookshelfSourcesPage(this))
    , m_sourcesProgressPage(new BtBookshelfSourcesProgressPage(this))
    , m_languagesPage(new BtBookshelfLanguagesPage(this))
    , m_installWorksPage(new BtBookshelfWorksPage(WizardTaskType::installWorks, this))
    , m_updateWorksPage(new BtBookshelfWorksPage(WizardTaskType::updateWorks, this))
    , m_removeWorksPage(new BtBookshelfWorksPage(WizardTaskType::removeWorks, this))
    , m_installFinalPage(new BtBookshelfInstallFinalPage(this))  // For install and update
{
    setPage(WizardPage::taskPage, m_taskPage);
    setPage(WizardPage::sourcesProgressPage,m_sourcesProgressPage);
    setPage(WizardPage::sourcesPage, m_sourcesPage);
    setPage(WizardPage::languagesPage, m_languagesPage);
    setPage(WizardPage::installWorksPage, m_installWorksPage);
    setPage(WizardPage::updateWorksPage, m_updateWorksPage);
    setPage(WizardPage::removeWorksPage, m_removeWorksPage);
    setPage(WizardPage::removeFinalPage, new BtBookshelfRemoveFinalPage(this));
    setPage(WizardPage::installFinalPage, m_installFinalPage);
    setStartId(WizardPage::taskPage);

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
    m_closeMessageBox->setWindowTitle(QApplication::translate(
                                          "BookshelfWizard",
                                          "Canceling Downloads"));
    m_closeMessageBox->setText(QApplication::translate(
                                          "BookshelfWizard",
                                          "The Bookshelf Manager will close when the current download finishes."));
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

void BtBookshelfWizard::stopDownload() {
    if (currentPage() == m_installFinalPage) {
        m_installFinalPage->slotStopInstall();
    }
    if (currentPage() == m_sourcesProgressPage) {
        m_sourcesProgressPage->slotStopInstall();
    }
}

void BtBookshelfWizard::keyPressEvent(QKeyEvent * event) {
    if(event->key() == Qt::Key_Escape) {
        if (m_downloadInProgress) {
            m_closeRequested = true;
            m_closeMessageBox->show();
            stopDownload();
            return;
        }
    }
    QWizard::keyPressEvent(event);
}

void BtBookshelfWizard::downloadStarted() {
    m_downloadInProgress = true;
}

void BtBookshelfWizard::downloadFinished() {
    m_downloadInProgress = false;
    if (m_closeRequested) {
        m_closeMessageBox->hide();
        accept();
    }
}
