/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTTIPDIALOG_H
#define BTTIPDIALOG_H

#include <QDialog>
#include <QStringList>

class QAbstractButton;
class QCheckBox;
class QDialogButtonBox;
class QWebView;
class QUrl;

/**
* The Tip Of The Day dialog
*/
class BtTipDialog: public QDialog {
        Q_OBJECT

    public:
        BtTipDialog(QWidget *parent = 0, Qt::WindowFlags wflags = Qt::Dialog);
        ~BtTipDialog();

    public slots:

    /** Called when the show tips at startup checkbox changes. */
        void startupBoxChanged(int value);

    private slots:

        /** Called when the next tip button is pressed. */
        void nextTip(QAbstractButton* button);

        /** Called when any link in a tip is clicked. */
        void linkClicked(const QUrl& url);

    private:

        /** Enter tips in this function */
        void create_tips();

        /** Sends the current tip to the web view */
        void displayTip();

        QDialogButtonBox* m_buttonBox;
        QWebView* m_tipView;
        QCheckBox* m_showTipsCheckBox;
        int m_tipNumber;
        QStringList m_tips;
};

#endif
