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

#ifndef BTLISTMODEL_H
#define BTLISTMODEL_H

#include <QStandardItemModel>

/**
    \brief This is a generic list model with optional checkboxes on first
    column and optional delete icon in second column.
\ */

class BtListModel: public QStandardItemModel {

    Q_OBJECT

public:

    BtListModel(bool checkable, QObject *parent = nullptr, int numColumns = 1);

    void appendItem(const QString& title, const QString& tooltip = QString());

private:
    bool m_checkable;
    int m_columnCount;

};
#endif
