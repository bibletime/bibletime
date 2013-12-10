/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2013 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTINSTALLPATHDIALOG_H
#define BTINSTALLPATHDIALOG_H

#include <QDialog>


class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;

class BtInstallPathDialog : public QDialog {
        Q_OBJECT
    public:
        BtInstallPathDialog();
        ~BtInstallPathDialog();

    public slots:
        virtual void accept();

    private slots:
        void slotAddClicked();
        void slotRemoveClicked();
        void slotEditClicked();

    private:
        void writeSwordConfig();
        void updateTopLevelItems();
        void addPathToList(QString path);

    private:
        QPushButton* m_editButton;
        QPushButton* m_addButton;
        QPushButton* m_removeButton;
        QTreeWidget* m_swordPathListBox;
        QTreeWidgetItem* m_writableItem;
        QTreeWidgetItem* m_readableItem;
        QTreeWidgetItem* m_nonexistingItem;
};

#endif
