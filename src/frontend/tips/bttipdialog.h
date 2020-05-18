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

#ifndef BTTIPDIALOG_H
#define BTTIPDIALOG_H

#include <QDialog>
#include <QStringList>


class QAbstractButton;
class QCheckBox;
class QDialogButtonBox;
class QPushButton;
class QUrl;
class QTextBrowser;
/**
  The Tip Of The Day dialog.
*/
class BtTipDialog: public QDialog {
        Q_OBJECT

    public: /* Methods: */

        BtTipDialog(QWidget *parent = nullptr, Qt::WindowFlags wflags = Qt::Dialog);

    private: /* Methods: */

        void retranslateUi();

        /** Sends the current tip to the web view */
        void displayTip();

    private: /* Fields: */

        QDialogButtonBox* m_buttonBox;
        QTextBrowser* m_tipView;
        QCheckBox* m_showTipsCheckBox;
        int m_tipNumber;
        QStringList m_tips;
};

#endif
