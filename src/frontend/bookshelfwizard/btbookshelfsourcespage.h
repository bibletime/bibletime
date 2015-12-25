/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTBOOKSHELFSOURCESPAGE
#define BTBOOKSHELFSOURCESPAGE

#include <QStringList>
#include <QWizardPage>

class BtBookshelfWizard;
class BtInstallMgr;
class BtListModel;
class QProgressDialog;
class QTableView;
class QVBoxLayout;

class BtBookshelfSourcesPage: public QWizardPage {

    Q_OBJECT

public:
    BtBookshelfSourcesPage(QWidget *parent = 0);

    bool isComplete() const;
    void initializePage();
    int nextId() const;
    QStringList selectedSources() const;

private slots:
    void slotDataChanged();
    void slotItemClicked(const QModelIndex& index);

private:
    void addNewSource();
    void createSourcesModel();
    void deleteRemoteSource(const QString& source);
    QStringList loadInitialSources();
    void retranslateUi();
    void selectSourcesInModel(const QStringList& sources);
    void setupUi();
    void updateSourcesModel();
    void updateRemoteLibraries();
    void updateRemoteLibrariesList();
    void updateRemoteLibraryWorks(QProgressDialog* dlg);
    void updateRemoteSources();
    void updateRemoteWorks();

    bool m_firstTimeInit;
    QTableView *m_sourcesTableView;
    QVBoxLayout *m_verticalLayout;
    BtListModel *m_model;
    QProgressDialog* m_progressDialog;
    BtInstallMgr* m_currentInstallMgr;
};

#endif
