/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/config/btconfig.h"
#include "backend/btinstallbackend.h"
#include "backend/btinstallmgr.h"
#include "backend/models/btlistmodel.h"
#include "frontend/bookshelfmanager/cswordsetupinstallsourcesdialog.h"
#include "frontend/bookshelfmanager/installpage/btrefreshprogressdialog.h"
#include "frontend/bookshelfwizard/btbookshelfsourcespage.h"
#include "frontend/bookshelfwizard/btbookshelfwizardenums.h"
#include "util/bticons.h"
#include "util/btconnect.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QDate>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QProgressDialog>
#include <QString>
#include <QTableView>
#include <QVBoxLayout>

namespace {
const QString SourcesKey = "GUI/BookshelfWizard/sources";
const QString lastUpdate = "GUI/BookshelfWizard/lastUpdate";
} // anonymous namespace


BtBookshelfSourcesPage::BtBookshelfSourcesPage(QWidget *parent)
    : QWizardPage(parent),
      m_firstTimeInit(true) {

    setupUi();
    createSourcesModel();
    retranslateUi();
}

void BtBookshelfSourcesPage::setupUi() {

    m_verticalLayout = new QVBoxLayout(this);
    m_verticalLayout->setObjectName(QStringLiteral("m_verticalLayout"));
    m_sourcesTableView = new QTableView(this);
    m_sourcesTableView->setObjectName(QStringLiteral("sourcesListView"));
    m_sourcesTableView->horizontalHeader()->setVisible(false);
    m_sourcesTableView->verticalHeader()->setVisible(false);
    connect(m_sourcesTableView, &QAbstractItemView::clicked,
            this, &BtBookshelfSourcesPage::slotItemClicked);

    m_verticalLayout->addWidget(m_sourcesTableView);

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
}

void BtBookshelfSourcesPage::createSourcesModel() {
    m_model = new BtListModel(true, this, 2);
    m_sourcesTableView->setModel(m_model);
    connect(m_model, &QStandardItemModel::dataChanged,
            this, &BtBookshelfSourcesPage::slotDataChanged);
}

void BtBookshelfSourcesPage::retranslateUi() {
    setTitle(QApplication::translate("BookshelfWizard", "Choose Remote Libraries", 0));
    setSubTitle(QApplication::translate("BookshelfWizard",
                                        "Choose one or more remote libraries to install works from.", 0));
}

int BtBookshelfSourcesPage::nextId() const {
    return pages::languagesPage;
}

void BtBookshelfSourcesPage::initializePage() {
    updateRemoteLibraries();
    QStringList saveSources = selectedSources();
    if (m_firstTimeInit)
        saveSources = loadInitialSources();
    m_firstTimeInit = false;

    updateSourcesModel();
    selectSourcesInModel(saveSources);
}

QStringList BtBookshelfSourcesPage::loadInitialSources() {
    QStringList sources = btConfig().value<QStringList>(SourcesKey,QStringList());
    return sources;
}

void BtBookshelfSourcesPage::selectSourcesInModel(const QStringList& sources) {
    int rows = m_model->rowCount();
    for (int row = 0; row < rows; ++row) {
        QStandardItem *item = m_model->item(row,0);
        QString source = item->text();
        if (sources.contains(source))
            item->setCheckState(Qt::Checked);
    }
}

void BtBookshelfSourcesPage::updateSourcesModel() {
    QStringList sourceList = BtInstallBackend::sourceNameList();
    m_model->clear();
    for (QString source : sourceList) {
        m_model->appendItem(source);
        int lastRow = m_model->rowCount() - 1;
        QStandardItem *item = m_model->item(lastRow,1);
        QIcon icon = BtIcons::instance().icon_delete;
        item->setIcon(icon);
    }
    QStandardItem* item = new QStandardItem(tr("< Add new remote library >"));
    m_model->appendRow(item);

    m_sourcesTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    m_sourcesTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_sourcesTableView->setColumnWidth(1, 30);
    m_sourcesTableView->setShowGrid(false);
}

void BtBookshelfSourcesPage::slotDataChanged() {
    emit completeChanged();
}

bool BtBookshelfSourcesPage::isComplete() const {
    return selectedSources().count() > 0;
}

QStringList BtBookshelfSourcesPage::selectedSources() const {
    QStringList sources;
    int rows = m_model->rowCount();
    for (int row = 0; row < rows; ++row) {
        QStandardItem *item = m_model->item(row,0);
        if (item->checkState() == Qt::Checked) {
            QString source = item->text();
            sources << source;
        }
    }
    return sources;
}

void BtBookshelfSourcesPage::slotItemClicked(const QModelIndex& index) {
    if (index.column() == 1) {

        auto item1 = m_model->itemFromIndex(index);
        if (item1->icon().isNull())
            return;
        auto item0 = m_model->item(index.row(), 0);
        QString source = item0->text();
        deleteRemoteSource(source);
        QStringList saveSources = selectedSources();
        updateSourcesModel();
        selectSourcesInModel(saveSources);
    }
    else if (index.column() == 0) {
        auto item0 = m_model->itemFromIndex(index);
        if (!item0->isCheckable()) {
            addNewSource();
        }
    }
}

void BtBookshelfSourcesPage::deleteRemoteSource(const QString& source) {
    int ret = QMessageBox::warning(this, tr("Delete Source"),
                                   tr("Do you really want to delete this source?"),
                                   QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes)
        return;
    BtInstallBackend::deleteSource(QString(source));
}

static bool timeToUpdate() {
    QDate lastDate = btConfig().value<QDate>(lastUpdate);
    if (!lastDate.isValid())
        return true;
    if (QDate::currentDate().toJulianDay() - lastDate.toJulianDay() > 7)
        return true;
    return false;
}

void BtBookshelfSourcesPage::addNewSource() {

    CSwordSetupInstallSourcesDialog dlg;
    QStringList saveSources = selectedSources();
    if (dlg.exec() != QDialog::Accepted)
        return;
    if (dlg.wasRemoteListAdded()) {
        updateSourcesModel();
        selectSourcesInModel(saveSources);
        return;
    }
    sword::InstallSource newSource = dlg.getSource();
    if (*(newSource.type.c_str()) != '\0') // we have a valid source to add
        BtInstallBackend::addSource(newSource);
    updateSourcesModel();
    selectSourcesInModel(saveSources);
}

void BtBookshelfSourcesPage::updateRemoteLibraries() {
    if (!timeToUpdate())
        return;

    QProgressDialog dlg("", tr("Cancel"), 0 , 100, this);
    dlg.setWindowModality(Qt::WindowModal);
    dlg.setWindowTitle(tr("Updating Remote Libraries"));
    dlg.setMinimumWidth(450);

    dlg.setValue(0);
    dlg.setLabelText(tr("Getting Library List"));
    updateRemoteLibrariesList();

    if (dlg.wasCanceled())
        return;

    updateRemoteLibraryWorks(&dlg);
    dlg.setValue(100);
    btConfig().setValue<QDate>(lastUpdate,QDate::currentDate());
}

void BtBookshelfSourcesPage::updateRemoteLibrariesList() {
    BtInstallMgr iMgr;
    int ret = iMgr.refreshRemoteSourceConfiguration();
    if (ret ) {
        qWarning("InstallMgr: getting remote list returned an error.");
    }
}

void BtBookshelfSourcesPage::updateRemoteLibraryWorks(QProgressDialog* dlg) {
    QStringList sourceNames = BtInstallBackend::sourceNameList();
    BtInstallMgr iMgr;
    int sourceCount = sourceNames.count();
    for (int i=0; i<sourceCount; ++i) {
        if (dlg->wasCanceled())
            return;
        QString sourceName = sourceNames.at(i);
        int percent = 10 + 90 *((double)i/sourceCount);
        QString label = tr("Updating remote library") + " \"" + sourceName +"\"";
        dlg->setValue(percent);
        dlg->setLabelText(label);
        sword::InstallSource source = BtInstallBackend::source(sourceName);
        bool result = (iMgr.refreshRemoteSource(&source) == 0);
        if (result) {
            ;
        } else {
            QString error = QString(tr("Failed to refresh source %1")).arg(sourceName);
            //            qWarning(error);
        }
    }
}

