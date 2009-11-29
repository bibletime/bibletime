/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTBOOKSHELFFILTERMODEL_H
#define BTBOOKSHELFFILTERMODEL_H

#include <QSortFilterProxyModel>


class BtBookshelfFilterModel: public QSortFilterProxyModel {
    Q_OBJECT
    public:
        BtBookshelfFilterModel(QObject *parent = 0);
        virtual ~BtBookshelfFilterModel();

        // Common methods:
        inline bool enabled() const {
            return m_enabled;
        }

        virtual bool filterAcceptsRow(int row, const QModelIndex &parent) const;

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

        inline const Qt::CaseSensitivity nameFilterCase() const {
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


    public slots:
        void setEnabled(bool enable);

        // Name filter:
        void setNameFilterRole(int role);
        void setNameFilterKeyColumn(int column);
        void setNameFilterFixedString(const QString &nameFilter);
        void setNameFilterCase(Qt::CaseSensitivity value);

        // Hidden filter:
        void setHiddenFilterRole(int role);
        void setHiddenFilterKeyColumn(int column);
        void setShowHidden(bool show);
        void setShowShown(bool show);

    protected:
        bool nameFilterAcceptsRow(int row, const QModelIndex &parent) const;
        bool hiddenFilterAcceptsRow(int row, const QModelIndex &parent) const;

    protected:
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

#endif // BTBOOKSHELFFILTERMODEL_H
