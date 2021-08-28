/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include <QDialog>

#include <QObject>
#include <QString>
#include <Qt>


class BtBookshelfWidget;
class QDialogButtonBox;
class QLabel;
class QWidget;

class BtModuleChooserDialog : public QDialog {
    Q_OBJECT

    protected:
        explicit BtModuleChooserDialog(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

        void retranslateUi();

        QLabel * label() const { return m_captionLabel; }

        BtBookshelfWidget * bookshelfWidget() const
        { return m_bookshelfWidget; }

        QDialogButtonBox * buttonBox() const { return m_buttonBox; }

    private:
        QLabel            *m_captionLabel;
        BtBookshelfWidget *m_bookshelfWidget;
        QDialogButtonBox  *m_buttonBox;
};
