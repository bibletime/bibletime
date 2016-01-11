/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfwizard/btbookshelflanguagespage.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QListView>
#include <QPushButton>
#include <QVBoxLayout>
#include <set>
#include "backend/btinstallbackend.h"
#include "backend/config/btconfig.h"
#include "backend/managers/clanguagemgr.h"
#include "backend/models/btlistmodel.h"
#include "frontend/bookshelfwizard/btbookshelfwizard.h"
#include "frontend/bookshelfwizard/btbookshelfwizardenums.h"
#include "util/btconnect.h"


namespace {
QString const LanguagesKey = "GUI/BookshelfWizard/languages";
} // anonymous namespace

BtBookshelfLanguagesPage::BtBookshelfLanguagesPage(QWidget * parent)
    : QWizardPage(parent)
{
    // Setup UI:
    m_verticalLayout = new QVBoxLayout(this);
    m_verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
    m_languagesListView = new QListView(this);
    m_languagesListView->setObjectName(QStringLiteral("languagesListView"));
    m_verticalLayout->addWidget(m_languagesListView);

    // Create languages Model:
    m_model = new BtListModel(true, this);
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

#define W \
    [](BtBookshelfLanguagesPage * const p) noexcept -> BtBookshelfWizard & {\
        BtBookshelfWizard * const w =  \
                qobject_cast<BtBookshelfWizard *>(p->wizard()); \
        BT_ASSERT(w); \
        return *w; \
    }(this)

void BtBookshelfLanguagesPage::initializePage() {

    { // Select languages:
        QStringList languages;
        if (m_firstTimeInit) {
            languages << btConfig().value<QStringList>(LanguagesKey,
                                                       QStringList{});
            if (languages.isEmpty())
                languages << tr("English");
            m_firstTimeInit = false;
        } else {
            languages << selectedLanguages();
        }

        int const rows = m_model->rowCount();
        bool scrolledToFirstSelected = false;
        for (int row = 0; row < rows; ++row) {
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
    for (auto const & sourceName : W.selectedSources())
        for (auto const * module :
             BtInstallBackend::backend(
                 BtInstallBackend::source(sourceName))->moduleList())
            languages.insert(module->language()->translatedName());

    // Update languages model:
    m_model->clear();
    for (auto const & lang : languages)
        m_model->appendItem(lang);
    if (languages.size() == 1u)
        m_model->item(0, 0)->setCheckState(Qt::Checked);
}

bool BtBookshelfLanguagesPage::skipPage() const
{ return m_model->rowCount() == 1; }

void BtBookshelfLanguagesPage::slotDataChanged() { emit completeChanged(); }

bool BtBookshelfLanguagesPage::isComplete() const
{ return selectedLanguages().count() > 0; }

QStringList BtBookshelfLanguagesPage::selectedLanguages() const {
    QStringList languages;
    int const rows = m_model->rowCount();
    for (int row = 0; row < rows; ++row) {
        QStandardItem * const item = m_model->item(row,0);
        if (item->checkState() == Qt::Checked)
            languages << item->text();
    }
    return languages;
}
