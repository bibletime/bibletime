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

#include "btlistmodel.h"

#include <QStandardItem>

BtListModel::BtListModel(bool checkable, QObject *parent, int numColumns)
    : QStandardItemModel(parent),
      m_checkable(checkable),
    m_columnCount(numColumns) {
    if (numColumns > 0)
        setColumnCount(numColumns);
}

void BtListModel::appendItem(const QString& title, const QString& tooltip){
    QList<QStandardItem*> items;
    QStandardItem* item = new QStandardItem(title);
    items.append(item);
    item->setToolTip(tooltip);
    if (m_checkable)
        item->setCheckable(true);
    for (int column = 1; column < m_columnCount; ++column) {
        QStandardItem* item2 = new QStandardItem();
        items.append(item2);
    }
    appendRow(items);

}
