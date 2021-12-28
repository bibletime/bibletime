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

#pragma once

#include "btbookshelfwizardpage.h"

#include <QObject>
#include <QString>
#include <QStringList>


class QListView;
class QStandardItemModel;
class QVBoxLayout;
class QWidget;

class BtBookshelfLanguagesPage final: public BtBookshelfWizardPage {

    Q_OBJECT

public: // methods:

    BtBookshelfLanguagesPage(QWidget * parent = nullptr);

    void initializeLanguages();
    void initializePage() final override;
    bool isComplete() const final override;
    int nextId() const final override;
    QStringList selectedLanguages() const;
    bool skipPage() const noexcept;

private Q_SLOTS:

    void slotDataChanged();

private: // methods:

    void retranslateUi();

private: // fields:

    bool m_firstTimeInit = true;
    QListView * m_languagesListView;
    QVBoxLayout * m_verticalLayout;
    QStandardItemModel * m_model;

}; /* class BtBookshelfLanguagesPage */
