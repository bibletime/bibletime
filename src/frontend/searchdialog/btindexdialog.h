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

#ifndef BTINDEXDIALOG_H
#define BTINDEXDIALOG_H

#include <QDialog>


class QCheckBox;
class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;

/**
* This class encapsulates the "Manage search indices" page of the Bookshelf
* Manager.  It allows for creation and deletion of search indicies for each
* installed module.  It also allows for deletion of orphaned indices.
*/
class BtIndexDialog final: public QDialog {

    Q_OBJECT

public: /* Methods: */

    BtIndexDialog(QWidget * parent = nullptr,
                  Qt::WindowFlags f = Qt::WindowFlags());

private: /* Methods: */

    void populateModuleList();
    void retranslateUi();

private slots:

    void autoDeleteOrphanedIndicesChanged(int newState);
    void slotSwordSetupChanged();
    void createIndices();
    void deleteIndices();

private: /* Fields: */

    QCheckBox * m_autoDeleteOrphanedIndicesBox;
    QTreeWidget * m_moduleList;
    QPushButton * m_deleteButton;
    QPushButton * m_createButton;
    QPushButton * m_closeButton;

    QTreeWidgetItem * m_modsWithIndices;
    QTreeWidgetItem * m_modsWithoutIndices;

};

#endif /* BTINDEXDIALOG_H */
