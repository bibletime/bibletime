/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfwizard/btbookshelflanguagespage.h"

#include "backend/btinstallbackend.h"
#include "backend/config/btconfig.h"
#include "backend/managers/clanguagemgr.h"
#include "backend/models/btlistmodel.h"
#include "frontend/bookshelfwizard/btbookshelfwizard.h"
#include "frontend/bookshelfwizard/btbookshelfwizardenums.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QListView>
#include <QPushButton>
#include <QSet>
#include <QVBoxLayout>

namespace {
const QString LanguagesKey = "GUI/BookshelfWizard/languages";
} // anonymous namespace

BtBookshelfLanguagesPage::BtBookshelfLanguagesPage(QWidget *parent)
    : QWizardPage(parent),
      m_firstTimeInit(true) {

    setupUi();
    createLanguagesModel();
    retranslateUi();
}

void BtBookshelfLanguagesPage::setupUi() {
    m_verticalLayout = new QVBoxLayout(this);
    m_verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
    m_languagesListView = new QListView(this);
    m_languagesListView->setObjectName(QStringLiteral("languagesListView"));
    m_verticalLayout->addWidget(m_languagesListView);
}

void BtBookshelfLanguagesPage::createLanguagesModel() {
    m_model = new BtListModel(true, this);
    m_languagesListView->setModel(m_model);
    connect(m_model, &QStandardItemModel::dataChanged,
            this, &BtBookshelfLanguagesPage::slotDataChanged);
}

void BtBookshelfLanguagesPage::retranslateUi() {
    setTitle(QApplication::translate("BookshelfWizard", "Choose Languages", 0));
    setSubTitle(QApplication::translate("BookshelfWizard", "Choose one or more languages to install works from.", 0));
}

int BtBookshelfLanguagesPage::nextId() const {
    return WizardPage::installWorksPage;
}

BtBookshelfWizard *BtBookshelfLanguagesPage::btWizard() {
    return qobject_cast<BtBookshelfWizard*>(wizard());
}

void BtBookshelfLanguagesPage::initializePage() {
    QStringList saveLanguages = selectedLanguages();
    if (m_firstTimeInit)
        saveLanguages = loadInitialLanguages();
    m_firstTimeInit = false;

    QStringList sources = btWizard()->selectedSources();
    QStringList languages = getLanguagesFromSources(sources);
    updateLanguagesModel(languages);
    selectLanguagesInModel(saveLanguages);
    scrollToFirstSelected();
    }

QStringList BtBookshelfLanguagesPage::getLanguagesFromSources(
        const QStringList& sources) {
    QSet<QString> languages;
    for (auto sourceName : sources) {
        sword::InstallSource source = BtInstallBackend::source(sourceName);
        CSwordBackend *backend = BtInstallBackend::backend(source);
        auto modules = backend->moduleList();
        for (auto module : modules) {
            const CLanguageMgr::Language *language = module->language();
            QString lang = language->translatedName();
            languages << lang;
        }
    }
    return languages.toList();
}

void BtBookshelfLanguagesPage::updateLanguagesModel(const QStringList& languages) {

    QStringList sortLanguages = languages;
    qSort(sortLanguages);
    m_model->clear();
    for (auto lang : sortLanguages) {
        m_model->appendItem(lang);
    }
}

QStringList BtBookshelfLanguagesPage::loadInitialLanguages() {
    QStringList languages = btConfig().value<QStringList>(LanguagesKey,QStringList());
    return languages;
}

void BtBookshelfLanguagesPage::selectLanguagesInModel(const QStringList& languages) {
    int rows = m_model->rowCount();
    for (int row = 0; row < rows; ++row) {
        QStandardItem *item = m_model->item(row,0);
        QString language = item->text();
        if (languages.contains(language))
            item->setCheckState(Qt::Checked);
    }
}

void BtBookshelfLanguagesPage::scrollToFirstSelected() {
    int rows = m_model->rowCount();
    for (int row = 0; row < rows; ++row) {
        QStandardItem *item = m_model->item(row,0);
        if (item->checkState() == Qt::Checked) {
            QModelIndex index = m_model->indexFromItem(item);
            m_languagesListView->scrollTo(index);
            return;
        }
    }
}

void BtBookshelfLanguagesPage::slotDataChanged() {
    emit completeChanged();
}

bool BtBookshelfLanguagesPage::isComplete() const {
    return selectedLanguages().count() > 0;
}

QStringList BtBookshelfLanguagesPage::selectedLanguages() const {
    QStringList languages;
    int rows = m_model->rowCount();
    for (int row = 0; row < rows; ++row) {
        QStandardItem *item = m_model->item(row,0);
        if (item->checkState() == Qt::Checked) {
            QString language = item->text();
            languages << language;
        }
    }
    return languages;
}
