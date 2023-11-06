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

#include <QSortFilterProxyModel>

#include <QObject>
#include <QString>
#include <Qt>
#include "../drivers/cswordmoduleinfo.h"


class QModelIndex;

class BtBookshelfFilterModel: public QSortFilterProxyModel {

    Q_OBJECT

public: // methods:

    BtBookshelfFilterModel(QObject * parent = nullptr);

    bool enabled() const noexcept { return m_enabled; }

    bool filterAcceptsRow(int row, QModelIndex const & parent) const override;

    int nameFilterRole() const noexcept { return m_nameFilterRole; }
    int nameFilterKeyColumn() const noexcept { return m_nameFilterColumn; }
    QString const & nameFilter() const noexcept { return m_nameFilter; }
    Qt::CaseSensitivity nameFilterCase() const noexcept
    { return m_nameFilterCase; }

    int hiddenFilterRole() const noexcept { return m_hiddenFilterRole; }
    int hiddenFilterKeyColumn() const { return m_hiddenFilterColumn; }
    bool showHidden() const noexcept { return m_showHidden; }
    bool showShown() const noexcept { return m_showShown; }

public Q_SLOTS:

    void setEnabled(bool enable);

    // Name filter:
    void setNameFilterRole(int role);
    void setNameFilterKeyColumn(int column);
    void setNameFilterFixedString(QString const & nameFilter);
    void setNameFilterCase(Qt::CaseSensitivity value);

    // Hidden filter:
    void setHiddenFilterRole(int role);
    void setHiddenFilterKeyColumn(int column);
    void setShowHidden(bool show);
    void setShowShown(bool show);

private: // methods:

    bool nameFilterAcceptsRow(int row, QModelIndex const & parent) const;
    bool hiddenFilterAcceptsRow(int row, QModelIndex const & parent) const;

private: // fields:

    bool m_enabled;

    // Name filter:
    QString m_nameFilter;
    int m_nameFilterRole;
    int m_nameFilterColumn;
    Qt::CaseSensitivity  m_nameFilterCase;

    // Hidden filter:
    int m_hiddenFilterRole;
    int m_hiddenFilterColumn;
    bool m_showHidden;
    bool m_showShown;

};
