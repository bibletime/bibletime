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

#ifndef BTMODULEHIDDENFILTERPROXYMODEL_H
#define BTMODULEHIDDENFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>


class BtModuleHiddenFilterProxyModel: public QSortFilterProxyModel {
        Q_OBJECT
    public:
        BtModuleHiddenFilterProxyModel(QObject *parent = 0);
        virtual ~BtModuleHiddenFilterProxyModel();

        inline bool enabled() const {
            return m_enabled;
        }
        void setEnabled(bool enable);

        inline bool showHidden() const {
            return m_showHidden;
        }
        void setShowHidden(bool show);

        inline bool showShown() const {
            return m_showShown;
        }
        void setShowShown(bool show);

        virtual bool filterAcceptsRow(int row, const QModelIndex &parent) const;

    protected:
        bool m_enabled;
        bool m_showHidden;
        bool m_showShown;
};

#endif // BTMODULEHIDDENFILTERPROXYMODEL_H
