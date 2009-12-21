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

#ifndef BTMODULECHOOSERDIALOG_H
#define BTMODULECHOOSERDIALOG_H

#include <QDialog>


class BtBookshelfTreeModel;
class BtBookshelfView;
class QAbstractItemModel;
class QDialogButtonBox;
class QLabel;

class BtModuleChooserDialog : public QDialog {
    Q_OBJECT
    public:
        virtual ~BtModuleChooserDialog();

    protected:
        explicit BtModuleChooserDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);

        void retranslateUi();

        inline QLabel *label() const { return m_captionLabel; }
        inline BtBookshelfView *treeView() const { return m_treeView; }
        inline QDialogButtonBox *buttonBox() const { return m_buttonBox; }

    private:
        QLabel           *m_captionLabel;
        BtBookshelfView  *m_treeView;
        QDialogButtonBox *m_buttonBox;
};

#endif // BTMODULECHOOSERDIALOG_H
