/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2025 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include <QSortFilterProxyModel>

#include <QString>


class QModelIndex;

class BtBookshelfFilterModel: public QSortFilterProxyModel {

    Q_OBJECT

public: // methods:

    BtBookshelfFilterModel(QObject * parent = nullptr);

    bool filterAcceptsRow(int row, QModelIndex const & parent) const override;

    QString const & nameFilter() const noexcept { return m_nameFilter; }

    bool showHidden() const noexcept { return m_showHidden; }

public Q_SLOTS:

    // Name filter:
    void setNameFilterFixedString(QString const & nameFilter);

    // Hidden filter:
    void setShowHidden(bool show);

private: // fields:

    // Name filter:
    QString m_nameFilter;

    // Hidden filter:
    bool m_showHidden;

};
