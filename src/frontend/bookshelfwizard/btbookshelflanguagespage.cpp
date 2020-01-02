/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btbookshelflanguagespage.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QListView>
#include <QPushButton>
#include <QVBoxLayout>
#include <set>
#include "../../backend/btinstallbackend.h"
#include "../../backend/config/btconfig.h"
#include "../../backend/managers/clanguagemgr.h"
#include "../../backend/models/btlistmodel.h"
#include "../../util/btconnect.h"
#include "btbookshelfwizard.h"
#include "btbookshelfwizardenums.h"


namespace {
QString const LanguagesKey = "GUI/BookshelfWizard/languages";
} // anonymous namespace

BtBookshelfLanguagesPage::BtBookshelfLanguagesPage(QWidget * parent)
    : BtBookshelfWizardPage(parent)
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
    for (auto const & sourceName : btWizard().selectedSources())
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

bool BtBookshelfLanguagesPage::skipPage() const noexcept
{ return m_model->rowCount() == 1; }

void BtBookshelfLanguagesPage::slotDataChanged() { emit completeChanged(); }

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
