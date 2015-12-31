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
#include "frontend/bookshelfwizard/btbookshelflanguagespage.h"
#include "frontend/bookshelfwizard/btbookshelfsourcespage.h"
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
      m_sourcesPage(new BtBookshelfSourcesPage),
      m_languagesPage(new BtBookshelfLanguagesPage),
      m_installPage(new BtBookshelfWorksPage(BtBookshelfWorksPage::install)),
      m_updatePage(new BtBookshelfWorksPage(BtBookshelfWorksPage::update)),
      m_removePage(new BtBookshelfWorksPage(BtBookshelfWorksPage::remove))
{
    addPage(m_taskPage);
    addPage(m_sourcesPage);
    addPage(m_languagesPage);
    addPage(m_installPage);
    addPage(m_updatePage);
    addPage(m_removePage);

    QRect rect = geometry();
    rect.setWidth(780);
    setGeometry(rect);

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
    saveWizardGeometry();

    if (currentPage() == m_installPage) {
        saveSettings();
        if (m_installPage->manageWorks())
            QDialog::accept();
    }

    if (currentPage() == m_updatePage)
        if (m_updatePage->manageWorks())
            QDialog::accept();

    if (currentPage() == m_removePage)
        if (m_removePage->manageWorks())
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
