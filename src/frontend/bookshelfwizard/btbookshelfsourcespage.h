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

#include <QString>
#include <QStringList>
#include <QWizardPage>

class BtBookshelfWizard;
class BtInstallMgr;
class BtListModel;
class QProgressDialog;
class QPushButton;
class QSignalMapper;
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
    void slotButtonClicked(int row);

private:
    QPushButton * addButton(int row, int column, const QString& text);
    void addNewSource();
    BtBookshelfWizard *btWizard() const;
    void calculateButtonColumnWidth();
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
    QSignalMapper *m_signalMapper;

    QString m_addText;
    QString m_removeText;
};

#endif
