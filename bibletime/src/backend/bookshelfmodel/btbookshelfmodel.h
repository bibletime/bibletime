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

#ifndef BTBOOKSHELFMODEL_H
#define BTBOOKSHELFMODEL_H

#include <QAbstractListModel>

#include "backend/drivers/cswordmoduleinfo.h"

class BtBookshelfModel: public QAbstractListModel {
    Q_OBJECT
    public:
        enum ModuleRole {
            ModuleNameRole = Qt::DisplayRole,
            ModuleIconRole = Qt::DecorationRole,
            ModulePointerRole  = Qt::UserRole,
            ModuleCategoryRole = Qt::UserRole + 1,
            ModuleLanguageRole = Qt::UserRole + 2,
            UserRole = Qt::UserRole + 100
        };

        BtBookshelfModel(QObject *parent = 0);
        virtual ~BtBookshelfModel();

        virtual int rowCount(const QModelIndex &parent) const;
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation,
                                    int role = Qt::DisplayRole) const;

        inline CSwordModuleInfo *module(const QModelIndex &index) const {
            return (CSwordModuleInfo *)
                data(index, BtBookshelfModel::ModulePointerRole)
                    .value<void *>();
        }

        static QIcon categoryIcon(const CSwordModuleInfo::Category &category);
        static QString categoryName(const CSwordModuleInfo::Category &category);
        static QString languageName(const CLanguageMgr::Language *language);

        void addModule(CSwordModuleInfo * const module);
        void addModules(const QSet<CSwordModuleInfo *> &modules);
        void addModules(const QList<CSwordModuleInfo *> &modules);
        void removeModule(CSwordModuleInfo * const module);
        void removeModules(const QSet<CSwordModuleInfo *> &modules);
        void removeModules(const QList<CSwordModuleInfo *> &modules);

        CSwordModuleInfo* getModule(const QString &name) const;
        inline const QList<CSwordModuleInfo *> &modules() const {
            return m_data;
        }

    protected:
        QList<CSwordModuleInfo *> m_data;
};

#endif // BTBOOKSHELFMODEL_H
