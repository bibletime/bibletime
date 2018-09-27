/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
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

        /** Enter tips in this function */
        void initTips();

        /** Sends the current tip to the web view */
        void displayTip();

    private slots:

        /** Called when the show tips at startup checkbox changes. */
        void startupBoxChanged(bool checked);

        /** Called when the next tip button is pressed. */
        void nextTip();

        /** Called when any link in a tip is clicked. */
        void linkClicked(const QUrl& url);

    private: /* Fields: */

        QDialogButtonBox* m_buttonBox;
        QTextBrowser* m_tipView;
        QCheckBox* m_showTipsCheckBox;
        int m_tipNumber;
        QStringList m_tips;
};

#endif
