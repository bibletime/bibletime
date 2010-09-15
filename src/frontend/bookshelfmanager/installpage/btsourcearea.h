/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTSOURCEAREA_H
#define BTSOURCEAREA_H

#include <QWidget>

#include <QSet>
#include "backend/bookshelfmodel/btbookshelftreemodel.h"


class BtBookshelfModel;
class BtBookshelfWidget;
class BTModuleTreeItem;
class BtSourceAreaModel;
class BtSourceWidget;
class CSwordBackend;
class CSwordModuleInfo;
class QByteArray;
class QLabel;
class QModelIndex;
class QPushButton;

/**
* Area for one install source.
*
* - Tree widget for modules
* - Buttons for handling the source(s): refresh, remove, add
*
* Each source has
* QTreeWidget, populated with the module tree if the source
* module list is in a local cache. Refreshing the source refreshes
* the cache and rebuilds the module tree. Sources are not refreshed
* automatically, only by the user action, one source at a time.
*/
class BtSourceArea : public QWidget {
        Q_OBJECT

        friend class BtSourceWidget;
    public:

        BtSourceArea(const QString &sourceName, BtSourceWidget *parent = 0);
        ~BtSourceArea();

        void initView();
        void prepareRemove();
        /** Reimplemented from QWidget. */
        QSize sizeHint() const;
        void initTreeFirstTime();

        const QSet<CSwordModuleInfo*> &selectedModules() const;

        void syncGroupingOrder(const BtBookshelfTreeModel::Grouping &groupingOrder);
        void syncHeaderState(const QByteArray &state);

    public slots:
        /** Create a module tree for a tree widget */
        void createModuleTree();

    signals:
        void signalInstallablesChanged();
        void signalCreateTree();

    private slots:
        void slotCreateTree();
        void slotItemDoubleClicked(const QModelIndex &itemIndex);
        void slotViewGroupingOrderChanged();
        void slotHeaderChanged();

    private:
        QString m_sourceName;
        bool m_treeAlreadyInitialized;
        CSwordBackend* m_remoteBackend; // needed for the module list
        BtBookshelfModel *m_model;
        BtSourceAreaModel *m_treeModel;

        BtSourceWidget *m_parent;

        BtBookshelfWidget *m_view;
        QLabel* m_refreshTimeLabel;
        QPushButton* m_refreshButton;
        QPushButton* m_deleteButton;
        QPushButton* m_addButton;
};

#endif
