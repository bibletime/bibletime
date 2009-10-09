/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTINSTALLPATHDIALOG_H
#define BTINSTALLPATHDIALOG_H

#include <QDialog>

class QPushButton;
class QTreeWidget;


class BtInstallPathDialog : public QDialog {
        Q_OBJECT
    public:
        BtInstallPathDialog();

    public slots:
        virtual void accept();

    private slots:
        void slotAddClicked();
        void slotRemoveClicked();
        void slotEditClicked();

    private:
        void writeSwordConfig();

    private:
        QPushButton* m_editButton;
        QPushButton* m_addButton;
        QPushButton* m_removeButton;
        QTreeWidget* m_swordPathListBox;

};

#endif
