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

#include "btbookshelfsourcespage.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QDate>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QModelIndex>
#include <QProgressDialog>
#include <QPushButton>
#include <QTableView>
#include <QtGlobal>
#include <QVBoxLayout>
#include "../../backend/config/btconfig.h"
#include "../../backend/btinstallbackend.h"
#include "../../backend/btinstallmgr.h"
#include "../../backend/models/btlistmodel.h"
#include "../../util/btconnect.h"
#include "cswordsetupinstallsourcesdialog.h"
#include "btbookshelflanguagespage.h"
#include "btbookshelfwizard.h"
#include "btbookshelfwizardenums.h"


namespace {
char const buttonPropertyName[] = "BtBookshelfSourcesPageButtonProperty";
constexpr bool const ButtonTagAdd = true;
constexpr bool const ButtonTagRemove = false;
QString const SourcesKey = "GUI/BookshelfWizard/sources";
QStringList const initialSelection{"CrossWire", "Bible.org", "Xiphos"};
} // anonymous namespace


BtBookshelfSourcesPage::BtBookshelfSourcesPage(QWidget * parent)
    : BtBookshelfWizardPage(parent)
{
    // Setup UI:
    m_verticalLayout = new QVBoxLayout(this);
    m_verticalLayout->setObjectName(QStringLiteral("m_verticalLayout"));
    m_sourcesTableView = new QTableView(this);
    m_sourcesTableView->setObjectName(QStringLiteral("sourcesListView"));
    m_sourcesTableView->horizontalHeader()->setVisible(false);
    m_sourcesTableView->verticalHeader()->setVisible(false);
    m_verticalLayout->addWidget(m_sourcesTableView);

    // Create sources model:
    m_model = new BtListModel(true, this, 2);
    m_sourcesTableView->setModel(m_model);
    BT_CONNECT(m_model, &QStandardItemModel::dataChanged,
               this,    &QWizardPage::completeChanged);
}

void BtBookshelfSourcesPage::retranslateUi() {
    setTitle(QApplication::translate("BookshelfWizard",
                                     "Choose Remote Libraries"));
    setSubTitle(QApplication::translate(
            "BookshelfWizard",
            "Choose one or more remote libraries to install works from."));
}

int BtBookshelfSourcesPage::nextId() const {
    auto & languagesPage = btWizard().languagesPage();
    languagesPage.initializeLanguages();
    return languagesPage.skipPage()
           ? WizardPage::installWorksPage
           : WizardPage::languagesPage;
}

void BtBookshelfSourcesPage::initializePage() {
    QStringList saveSources;
    if (m_firstTimeInit) {
        saveSources << btConfig().value<QStringList>(SourcesKey, QStringList{});
        if (saveSources.empty())
            saveSources << initialSelection;
        m_firstTimeInit = false;
    } else {
        saveSources << selectedSources();
    }

    // Do before updating models to get correct
    // column width for buttons
    retranslateUi();

    updateSourcesModel();
    selectSourcesInModel(saveSources);
}

void BtBookshelfSourcesPage::selectSourcesInModel(const QStringList& sources) {
    for (int row = 0; row < m_model->rowCount(); ++row) {
        QStandardItem * const item = m_model->item(row,0);
        if (sources.contains(item->text()))
            item->setCheckState(Qt::Checked);
    }
}

void BtBookshelfSourcesPage::updateSourcesModel() {
    QStringList sourceList = BtInstallBackend::sourceNameList();
    m_model->clear();
    m_model->setColumnCount(2);

    auto const addButton = [this](int row, int column,
                                  QString const & text, bool const tag) {
        QPushButton * const button = new QPushButton(text, this);
        button->setProperty(buttonPropertyName, tag);
        m_sourcesTableView->setIndexWidget(m_model->index(row, column), button);
        return button;
    };

    QString const removeText = tr("Remove");
    for (QString source : sourceList) {
        m_model->appendItem(source);
        int const row = m_model->rowCount() - 1;
        QPushButton * button = addButton(row, 1, removeText, ButtonTagRemove);
        BT_CONNECT(button, &QPushButton::clicked,
                   [this, row]{ slotButtonClicked(row); });
    }

    m_model->appendRow(new QStandardItem(tr("< Add new remote library >")));
    int const row = m_model->rowCount() - 1;
    QString const addText = tr("Add");
    QPushButton * const button = addButton(row, 1, addText, ButtonTagAdd);
    BT_CONNECT(button, &QPushButton::clicked,
               [this, row]{ slotButtonClicked(row); });

    m_sourcesTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    m_sourcesTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_sourcesTableView->setShowGrid(false);

    // Calculate button column width:
    QFontMetrics const fontMetrics = m_sourcesTableView->fontMetrics();
    #if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    m_sourcesTableView->setColumnWidth(
                1,
                qMax(fontMetrics.horizontalAdvance(removeText),
                     fontMetrics.horizontalAdvance(addText)) + 60);
    #else
    m_sourcesTableView->setColumnWidth(1,
                                       qMax(fontMetrics.width(removeText),
                                            fontMetrics.width(addText)) + 60);
    #endif
}

bool BtBookshelfSourcesPage::isComplete() const {
    for (int row = 0; row < m_model->rowCount(); ++row)
        if (m_model->item(row, 0)->checkState() == Qt::Checked)
            return true;
    return false;
}

QStringList BtBookshelfSourcesPage::selectedSources() const {
    QStringList sources;
    for (int row = 0; row < m_model->rowCount(); ++row) {
        QStandardItem * const item = m_model->item(row, 0);
        if (item->checkState() == Qt::Checked)
            sources << item->text();
    }
    return sources;
}

void BtBookshelfSourcesPage::slotButtonClicked(int row) {
    QModelIndex const index = m_model->index(row, 1);
    if (static_cast<QPushButton *>(
            m_sourcesTableView->indexWidget(index))->property(
                    buttonPropertyName).toBool() == ButtonTagRemove)
    {
        if (QMessageBox::warning(
                        this,
                        tr("Delete Source"),
                        tr("Do you really want to delete this source?"),
                        QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
            BtInstallBackend::deleteSource(
                    m_model->item(index.row(), 0)->text());
        QStringList const saveSources = selectedSources();
        updateSourcesModel();
        selectSourcesInModel(saveSources);
        return;
    }

    CSwordSetupInstallSourcesDialog dlg;
    QStringList const saveSources = selectedSources();
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
