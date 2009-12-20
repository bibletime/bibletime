/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTSOURCEAREA_H
#define BTSOURCEAREA_H

#include <QWidget>

#include <boost/scoped_ptr.hpp>
#include <QApplication>
#include <QMap>
#include "backend/btmoduletreeitem.h"

// Sword includes:
#include <installmgr.h>


class QTreeWidget;
class QTreeWidgetItem;
class QLabel;
class QPushButton;

/**
* Area for one install source.
*
* - Tree widget for modules
* - Buttons for handling the source(s): refresh, edit, remove, add
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

        struct InstalledFilter : BTModuleTreeItem::Filter {
            InstalledFilter(QString sourceName);
            bool filter(CSwordModuleInfo*);
            sword::InstallSource m_source;
            boost::scoped_ptr<CSwordBackend> m_swordBackend;
        };

        BtSourceArea(const QString& sourceName);
        ~BtSourceArea();

        void initView();
        void prepareRemove();
        /** Reimplemented from QWidget. */
        virtual QSize sizeHint() const;
        void initTreeFirstTime();
        QTreeWidget* treeWidget();

        QMap<QString, bool>* selectedModules();

    public slots:
        void slotSwordSetupChanged();
        /** Create a module tree for a tree widget */
        void createModuleTree();

    signals:
        void signalSelectionChanged(QString sourceName, int selectedCount);
        void signalCreateTree();

    private slots:
        void slotCreateTree();
        void slotSelectionChanged(QTreeWidgetItem* item, int column);
        void slotItemDoubleClicked(QTreeWidgetItem* item, int column);
    private:
        void addToTree(BTModuleTreeItem* item, QTreeWidgetItem* widgetItem);

        QString m_sourceName;
        bool m_treeAlreadyInitialized;
        QMap<QString, bool> m_checkedModules;
        CSwordBackend* m_remoteBackend; // needed for the module list
        QList<CSwordModuleInfo*> m_moduleList;

        QTreeWidget* m_view;
        QLabel* m_refreshTimeLabel;
        QPushButton* m_refreshButton;
        QPushButton* m_editButton;
        QPushButton* m_deleteButton;
        QPushButton* m_addButton;
};

#endif
