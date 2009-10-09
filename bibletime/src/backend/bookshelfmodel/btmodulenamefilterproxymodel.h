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

#ifndef BTMODULENAMEFILTERPROXYMODEL_H
#define BTMODULENAMEFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class BtModuleNameFilterProxyModel: public QSortFilterProxyModel {
        Q_OBJECT
    public:
        BtModuleNameFilterProxyModel(QObject *parent = 0);
        virtual ~BtModuleNameFilterProxyModel();

        inline bool enabled() const {
            return m_enabled;
        }
        void setEnabled(bool enable);

        virtual bool filterAcceptsRow(int row, const QModelIndex &parent) const;

    protected:
        QString m_filter;
        bool    m_enabled;
};

#endif // BTMODULENAMEFILTERPROXYMODEL_H
