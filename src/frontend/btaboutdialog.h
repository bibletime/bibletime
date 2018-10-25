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

#ifndef BTABOUTDIALOG_H
#define BTABOUTDIALOG_H

#include <QDialog>

class QDialogButtonBox;
class QLabel;
class QTabWidget;
class QUrl;
class QTextBrowser;

class BtAboutDialog: public QDialog {
        Q_OBJECT
    public:
        BtAboutDialog(QWidget *parent = nullptr, Qt::WindowFlags wflags = Qt::Dialog);

    protected:
        void resizeEvent(QResizeEvent* event) override;

    private:
        void initTab(QTextBrowser *&tab);

        void retranslateUi();
        void retranslateBtTab();
        void retranslateContributorsTab();
        void retranslateSwordTab();
        void retranslateQtTab();
        void retranslateLicenceTab();

    private slots:
        void linkClicked(const QUrl &url);

    private:
        QTabWidget *m_tabWidget;
        QTextBrowser *m_bibletimeTab;
        QTextBrowser *m_contributorsTab;
        QTextBrowser *m_swordTab;
        QTextBrowser *m_qtTab;
        QTextBrowser *m_licenceTab;
        QDialogButtonBox *m_buttonBox;
        QLabel *m_iconLabel;
        QLabel *m_versionLabel;
};

#endif
