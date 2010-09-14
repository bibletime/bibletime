



































































































/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btinstallmodulechooserdialog.h"

#include "frontend/cmodulechooserdialog.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/btmoduletreeitem.h"

#include <QString>
#include <QList>
#include <QWidget>
#include <QString>
#include <QTreeWidgetItem>
#include <QBrush>
#include <QPushButton>

#include <QDebug>


BtInstallModuleChooserDialog::BtInstallModuleChooserDialog(QWidget* parent, QString title, QString label, QList<CSwordModuleInfo*>* empty)
        : CModuleChooserDialog(parent, title, label, empty) {
    qDebug("BtInstallModuleChooserDialog::BtInstallModuleChooserDialog start");
    init();
    okButton()->setText(tr("Install"));
    m_nameList = QStringList();
}

// Do nothing, the tree is initialized outside this class.
void BtInstallModuleChooserDialog::initModuleItem(BTModuleTreeItem*, QTreeWidgetItem*) {}

void BtInstallModuleChooserDialog::initModuleItem(QString name, QTreeWidgetItem* sourceItem) {
    /// \todo Use new bookshelf model instead
    /// \bug Valgrind reports memory leak:
    QTreeWidgetItem* moduleItem = new QTreeWidgetItem(sourceItem);
    moduleItem->setText(0, name);
    moduleItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    moduleItem->setCheckState(0, Qt::Checked);

    // prevent double items
    if (m_nameList.contains(name)) {
        qDebug() << "item already in list:" << name;
        //moduleItem->setCheckState(0, Qt::Unchecked);
        QBrush bg(Qt::red);
        moduleItem->setBackground(0, bg);
        //find and change the other offending items
        foreach (QTreeWidgetItem* doubleItem, treeWidget()->findItems(name, Qt::MatchFixedString | Qt::MatchCaseSensitive | Qt::MatchRecursive, 0)) {
            //doubleItem->setCheckState(0, Qt::Unchecked);
            //qDebug() << "CInstallModuleChooserDialog::initModuleItem" << doubleItem;
            doubleItem->setBackground(0, bg);
        }
        m_doubleCheckedModules[name] = true;
        enableOk(false);
    }
    m_nameList << name;
}

void BtInstallModuleChooserDialog::slotItemChecked(QTreeWidgetItem* item, int column) {
    QString moduleName = item->text(0);
    qDebug("BtInstallModuleChooserDialog::slotItemChecked start");
    // handle only non-toplevel items which has duplicates and where the first column was changed
    if (item->parent() && column == 0 && (findModuleItemsByName(moduleName).count() > 1))  {
        //prevent handling when the color is changed
        if (item->data(1, Qt::UserRole).toBool() == false) {
            qDebug("was not updating");
            item->setData(1, Qt::UserRole, true);
        }
        else {
            qDebug("was updating");
            item->setData(1, Qt::UserRole, false);
            return;
        }

        QList<QTreeWidgetItem*> doubleNameItems = findModuleItemsByName(moduleName);
        QList<QTreeWidgetItem*> doubleCheckedItems;
        foreach (QTreeWidgetItem* nItem, doubleNameItems) {
            if (nItem->checkState(0) == Qt::Checked) {
                doubleCheckedItems << nItem;
            }
        }

        if (doubleCheckedItems.count() > 1) {
            enableOk(false);
            // color the items
            qDebug() << "there were more than 1 item of the name" << moduleName;
            foreach (QTreeWidgetItem* i, doubleNameItems) {
                QBrush bg(Qt::red);
                i->setBackground(0, bg);
            }
            m_doubleCheckedModules[moduleName] = true;
        }
        else if (doubleCheckedItems.count() == 1) {
            qDebug() << "there were 1 checked items of the name" << moduleName;
            // uncolor the items
            foreach (QTreeWidgetItem* i, doubleNameItems) {
                i->setBackground(0, i->parent()->background(0));
            }
            m_doubleCheckedModules.remove(moduleName);
            if (m_doubleCheckedModules.count() == 0) {
                enableOk(true);
            }
        }
    }
}

QList<QTreeWidgetItem*> BtInstallModuleChooserDialog::findModuleItemsByName(QString name) {
    qDebug() << "BtInstallModuleChooserDialog::findModuleItemsByName:" << name << treeWidget()->topLevelItemCount();
    QList<QTreeWidgetItem*> doubleNamedAllItems = treeWidget()->findItems(name, Qt::MatchFixedString | Qt::MatchCaseSensitive | Qt::MatchRecursive);
    //qDebug() << "doubleNamedAllItems: " << doubleNamedAllItems.count();
    QList<QTreeWidgetItem*> doubleNamedModuleItems;
    foreach (QTreeWidgetItem* item, doubleNamedAllItems) {
        //qDebug() << "item:" << item;
        if (item->parent()) {
            doubleNamedModuleItems << item;
        }
    }
    //qDebug() << "module items:" << doubleNamedModuleItems.count();
    return doubleNamedModuleItems;
}

void BtInstallModuleChooserDialog::enableOk(bool enabled) {
    qDebug() << "BtInstallModuleChooserDialog::enableOk" << enabled;
    okButton()->setEnabled(enabled);
}
