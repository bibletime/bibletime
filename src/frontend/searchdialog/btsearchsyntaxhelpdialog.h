/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTTABDIALOG_H
#define BTTABDIALOG_H

#include <QDialog>

class QDialogButtonBox;
class QUrl;
class QWebView;

namespace Search {

class BtSearchSyntaxHelpDialog: public QDialog {
        Q_OBJECT
    public:
        BtSearchSyntaxHelpDialog(QWidget *parent = 0, Qt::WindowFlags wflags = Qt::Dialog);
        ~BtSearchSyntaxHelpDialog();

    protected:
        void retranslateUi();

    protected slots:
        void linkClicked(const QUrl &url);

    private:
        QWebView         *m_webView;
        QDialogButtonBox *m_buttons;
};

} // namespace Search

#endif
