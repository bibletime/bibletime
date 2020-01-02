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

#ifndef BTBOOKSHELFLANGUAGESPAGE
#define BTBOOKSHELFLANGUAGESPAGE

#include "btbookshelfwizardpage.h"

#include <QStringList>


class BtBookshelfWizard;
class BtListModel;
class QListView;
class QVBoxLayout;

class BtBookshelfLanguagesPage final: public BtBookshelfWizardPage {

    Q_OBJECT

public: /* Methods: */

    BtBookshelfLanguagesPage(QWidget * parent = 0);

    void initializeLanguages();
    void initializePage() final override;
    bool isComplete() const final override;
    int nextId() const final override;
    QStringList selectedLanguages() const;
    bool skipPage() const noexcept;

private slots:

    void slotDataChanged();

private: /* Methods: */

    void retranslateUi();

private: /* Fields: */

    bool m_firstTimeInit = true;
    QListView * m_languagesListView;
    QVBoxLayout * m_verticalLayout;
    BtListModel * m_model;

}; /* class BtBookshelfLanguagesPage */

#endif
