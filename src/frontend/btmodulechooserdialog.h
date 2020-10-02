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

#ifndef BTMODULECHOOSERDIALOG_H
#define BTMODULECHOOSERDIALOG_H

#include <QDialog>

#include "btbookshelfwidget.h"


class CSwordModuleInfo;
class QDialogButtonBox;
class QLabel;

class BtModuleChooserDialog : public QDialog {
    Q_OBJECT

    protected:
        explicit BtModuleChooserDialog(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

        void retranslateUi();

        inline QLabel *label() const { return m_captionLabel; }
        inline BtBookshelfWidget *bookshelfWidget() const { return m_bookshelfWidget; }
        inline QDialogButtonBox *buttonBox() const { return m_buttonBox; }

    protected slots:
        void slotModuleAbout(CSwordModuleInfo *module);

    private:
        QLabel            *m_captionLabel;
        BtBookshelfWidget *m_bookshelfWidget;
        QDialogButtonBox  *m_buttonBox;
};

#endif // BTMODULECHOOSERDIALOG_H
