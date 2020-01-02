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

#ifndef BTBOOKSHELFFILTERMODEL_H
#define BTBOOKSHELFFILTERMODEL_H

#include <QSortFilterProxyModel>

#include "../drivers/cswordmoduleinfo.h"


class BtBookshelfFilterModel: public QSortFilterProxyModel {

    Q_OBJECT

public: /* Methods: */

    BtBookshelfFilterModel(QObject * parent = nullptr);

    inline bool enabled() const {
        return m_enabled;
    }

    bool filterAcceptsRow(int row, const QModelIndex & parent) const override;

    // Name filter:
    inline int nameFilterRole() const {
        return m_nameFilterRole;
    }

    inline int nameFilterKeyColumn() const {
        return m_nameFilterColumn;
    }

    inline const QString &nameFilter() const {
        return m_nameFilter;
    }

    inline Qt::CaseSensitivity nameFilterCase() const {
        return m_nameFilterCase;
    }

    // Hidden filter:
    int hiddenFilterRole() const {
        return m_hiddenFilterRole;
    }

    int hiddenFilterKeyColumn() const {
        return m_hiddenFilterColumn;
    }

    inline bool showHidden() const {
        return m_showHidden;
    }

    inline bool showShown() const {
        return m_showShown;
    }

    // Category filter:
    int categoryFilterRole() const {
        return m_categoryFilterRole;
    }

    int categoryFilterKeyColumn() const {
        return m_categoryFilterColumn;
    }

    inline CSwordModuleInfo::Categories shownCategories() const {
        return m_categoryFilter;
    }

public slots:
    void setEnabled(bool enable);

    // Name filter:
    void setNameFilterRole(int role);
    void setNameFilterKeyColumn(int column);
    void setNameFilterFixedString(const QString & nameFilter);
    void setNameFilterCase(Qt::CaseSensitivity value);

    // Hidden filter:
    void setHiddenFilterRole(int role);
    void setHiddenFilterKeyColumn(int column);
    void setShowHidden(bool show);
    void setShowShown(bool show);

    // Category filter:
    void setCategoryFilterRole(int role);
    void setCategoryFilterKeyColumn(int column);
    void setShownCategories(const CSwordModuleInfo::Categories & categories);

private: /* Methods: */

    bool nameFilterAcceptsRow(int row, const QModelIndex & parent) const;
    bool hiddenFilterAcceptsRow(int row, const QModelIndex & parent) const;
    bool categoryFilterAcceptsRow(int row, const QModelIndex & parent) const;

private: /* Fields: */

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

    // Categories filter:
    CSwordModuleInfo::Categories m_categoryFilter;
    int m_categoryFilterRole;
    int m_categoryFilterColumn;

};

#endif // BTBOOKSHELFFILTERMODEL_H
