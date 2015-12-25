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

#ifndef BTBOOKSHELFLANGUAGESPAGE
#define BTBOOKSHELFLANGUAGESPAGE

#include <QStringList>
#include <QWizardPage>

class BtBookshelfWizard;
class BtListModel;
class QListView;
class QVBoxLayout;

class BtBookshelfLanguagesPage: public QWizardPage {

    Q_OBJECT

public:
    BtBookshelfLanguagesPage(QWidget *parent = 0);

    void initializePage();
    bool isComplete() const;
    int nextId() const;
    QStringList selectedLanguages() const;

private slots:
    void slotDataChanged();

private:
    BtBookshelfWizard *btWizard();
    void createLanguagesModel();
    QStringList getLanguagesFromSources(const QStringList& sources);
    QStringList loadInitialLanguages();
    void retranslateUi();
    void scrollToFirstSelected();
    void selectLanguagesInModel(const QStringList& languages);
    void setupUi();
    void updateLanguagesModel(const QStringList& languages);

    bool m_firstTimeInit;
    QListView *m_languagesListView;
    QVBoxLayout *m_verticalLayout;
    BtListModel *m_model;
};

#endif
