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

#ifndef BTMODULECATEGORYFILTERPROXYMODEL_H
#define BTMODULECATEGORYFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

#include "backend/drivers/cswordmoduleinfo.h"


class BtModuleCategoryFilterProxyModel: public QSortFilterProxyModel {
    Q_OBJECT
    public:
        BtModuleCategoryFilterProxyModel(QObject *parent = 0);
        virtual ~BtModuleCategoryFilterProxyModel();

        inline bool enabled() const {
            return m_enabled;
        }
        void setEnabled(bool enable);

        inline CSwordModuleInfo::Categories shownCategories() const {
            return m_filter;
        }

        inline CSwordModuleInfo::Categories hiddenCategories() const {
            return ~m_filter & CSwordModuleInfo::AllCategories;
        }
        void setShownCategories(CSwordModuleInfo::Categories cs);
        void setHiddenCategories(CSwordModuleInfo::Categories cs);

        virtual bool filterAcceptsRow(int row, const QModelIndex &parent) const;

    protected:
        CSwordModuleInfo::Categories m_filter;
        bool                         m_enabled;
};

#endif // BTMODULECATEGORYFILTERPROXYMODEL_H
