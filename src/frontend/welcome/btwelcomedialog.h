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

#ifndef BTWELCOMEDIALOG_H
#define BTWELCOMEDIALOG_H

#include <QDialog>

class QDialogButtonBox;
class QLabel;

class BtWelcomeDialog: public QDialog {

    Q_OBJECT

public:

    BtWelcomeDialog(QWidget *parent = nullptr, Qt::WindowFlags wflags = Qt::Dialog);

    static void openWelcome();

protected:
    void retranslateUi();

private slots:
    void slotAccept();

private:

    QLabel *m_iconLabel;
    QLabel *m_label;
    QPushButton *m_laterButton;
    QPushButton *m_installButton;
    QDialogButtonBox* m_buttonBox;
};

#endif
