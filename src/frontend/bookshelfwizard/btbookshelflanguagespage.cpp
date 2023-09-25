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

#include "btbookshelflanguagespage.h"

#include <memory>
#include <QApplication>
#include <QList>
#include <QListView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStringLiteral>
#include <Qt>
#include <QVBoxLayout>
#include <set>
#include "../../backend/btinstallbackend.h"
#include "../../backend/config/btconfig.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../backend/language.h"
#include "../../backend/managers/cswordbackend.h"
#include "../../util/btconnect.h"
#include "btbookshelfwizard.h"
#include "btbookshelfwizardenums.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <installmgr.h> // IWYU pragma: keep for BtInstallBackend::source()
#pragma GCC diagnostic pop


namespace {
QString const LanguagesKey = "GUI/BookshelfWizard/languages";
} // anonymous namespace

BtBookshelfLanguagesPage::BtBookshelfLanguagesPage(QWidget * parent)
    : BtBookshelfWizardPage(parent)
{
    // Setup UI:
    m_verticalLayout = new QVBoxLayout(this);
    m_languagesListView = new QListView(this);
    m_verticalLayout->addWidget(m_languagesListView);

    // Create languages Model:
    m_model = new QStandardItemModel(this);
    m_languagesListView->setModel(m_model);
    BT_CONNECT(m_model, &QStandardItemModel::dataChanged,
               this,    &BtBookshelfLanguagesPage::slotDataChanged);
}

void BtBookshelfLanguagesPage::retranslateUi() {
    setTitle(QApplication::translate("BookshelfWizard", "Choose Languages"));
    setSubTitle(QApplication::translate("BookshelfWizard",
                                        "Choose one or more languages to "
                                        "install works from."));
}

int BtBookshelfLanguagesPage::nextId() const
{ return WizardPage::installWorksPage; }

void BtBookshelfLanguagesPage::initializePage() {

    { // Select languages:
        QStringList languages;
        if (m_firstTimeInit) {
            languages = btConfig().value<QStringList>(LanguagesKey);
            if (languages.isEmpty())
                languages = QStringList{tr("English")};
            m_firstTimeInit = false;
        } else {
            languages = selectedLanguages();
        }

        bool scrolledToFirstSelected = false;
        for (int row = 0; row < m_model->rowCount(); ++row) {
            QStandardItem * const item = m_model->item(row, 0);
            if (languages.contains(item->text())) {
                item->setCheckState(Qt::Checked);
                // Scroll to first selected item:
                if (!scrolledToFirstSelected) {
                    m_languagesListView->scrollTo(m_model->indexFromItem(item));
                    scrolledToFirstSelected = true;
                }
            }
        }
    }

    retranslateUi();
}

void BtBookshelfLanguagesPage::initializeLanguages() {
    // Get languages from sources:
    std::set<QString> languages;
    for (auto const & sourceName : btWizard().selectedSources()) {
        std::unique_ptr<CSwordBackend const> const backend =
                BtInstallBackend::backend(BtInstallBackend::source(sourceName));
        for (auto const * module : backend->moduleList())
            languages.insert(module->language()->translatedName());
    }

    // Update languages model:
    m_model->clear();
    for (auto const & language : languages) {
        auto * const item = new QStandardItem(language);
        item->setCheckable(true);
        m_model->appendRow(item);
    }
    if (languages.size() == 1u)
        m_model->item(0, 0)->setCheckState(Qt::Checked);
}

bool BtBookshelfLanguagesPage::skipPage() const noexcept
{ return m_model->rowCount() == 1; }

void BtBookshelfLanguagesPage::slotDataChanged() { Q_EMIT completeChanged(); }

bool BtBookshelfLanguagesPage::isComplete() const
{ return selectedLanguages().count() > 0; }

QStringList BtBookshelfLanguagesPage::selectedLanguages() const {
    QStringList languages;
    for (int row = 0; row < m_model->rowCount(); ++row) {
        QStandardItem * const item = m_model->item(row,0);
        if (item->checkState() == Qt::Checked)
            languages << item->text();
    }
    return languages;
}
