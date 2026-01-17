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

#include <optional>
#include <QString>
#include "../drivers/cswordmoduleinfo.h"


class QModelIndex;

class BtBookshelfFilterModel: public QSortFilterProxyModel {

    Q_OBJECT

public: // methods:

    BtBookshelfFilterModel(QObject * parent = nullptr);

    bool filterAcceptsRow(int row, QModelIndex const & parent) const override;

    QString const & nameFilter() const noexcept { return m_nameFilter; }

    bool showHidden() const noexcept { return m_showHidden; }

    std::optional<CSwordModuleInfo::ModuleType> const &
    moduleChooserType() const noexcept { return m_moduleChooserType; }

    CSwordModuleInfo * module(QModelIndex const & index) const noexcept;

public Q_SLOTS:

    // Name filter:
    void setNameFilterFixedString(QString const & nameFilter);

    // Hidden filter:
    void setShowHidden(bool show);

    // Module chooser type filter:
    void setModuleChooserType(std::optional<CSwordModuleInfo::ModuleType> type);

private: // methods:

    template <typename Field, typename Value>
    void changeFilter(Field & field, Value && value);

private: // fields:

    // Name filter:
    QString m_nameFilter;

    // Hidden filter:
    bool m_showHidden;

    // Module chooser type filter:
    std::optional<CSwordModuleInfo::ModuleType> m_moduleChooserType;

};
