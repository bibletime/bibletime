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


class QDialogButtonBox;
class QLabel;
class QPushButton;
class QWidget;

class BtWelcomeDialog: public QDialog {

    Q_OBJECT

public: // methods:

    BtWelcomeDialog(QWidget * parent = nullptr,
                    Qt::WindowFlags wflags = Qt::Dialog);

protected: // methods:

    void retranslateUi();

private: // fields:

    QLabel * m_iconLabel;
    QLabel * m_label;
    QPushButton * m_laterButton;
    QPushButton * m_installButton;
    QDialogButtonBox * m_buttonBox;

};
