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


class QStandardItemModel;
class QTableView;
class QVBoxLayout;
class QWidget;

class BtBookshelfSourcesPage final: public BtBookshelfWizardPage {

    Q_OBJECT

public: // methods:

    BtBookshelfSourcesPage(QWidget * parent = nullptr);

    bool isComplete() const final override;
    void initializePage() final override;
    int nextId() const final override;
    QStringList selectedSources() const;

private Q_SLOTS:

    void slotButtonClicked(int row);

private: // methods:

    void retranslateUi();
    void selectSourcesInModel(QStringList const & sources);
    void updateSourcesModel();

private: // fields:

    bool m_firstTimeInit = true;
    QTableView * m_sourcesTableView;
    QVBoxLayout * m_verticalLayout;
    QStandardItemModel * m_model;

}; /* class BtBookshelfSourcesPage */
