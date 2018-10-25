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

#ifndef BTTABDIALOG_H
#define BTTABDIALOG_H

#include <QDialog>

class QDialogButtonBox;
class QUrl;
class QTextBrowser;

namespace Search {

class BtSearchSyntaxHelpDialog: public QDialog {
        Q_OBJECT
    public:
        BtSearchSyntaxHelpDialog(QWidget *parent = nullptr, Qt::WindowFlags wflags = Qt::Dialog);

    protected:
        void retranslateUi();

    protected slots:
        void linkClicked(const QUrl &url);

    private:
        QTextBrowser  *m_textBrowser;
        QDialogButtonBox *m_buttons;
};

} // namespace Search

#endif
