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
class QResizeEvent;
class QTabWidget;
class QTextBrowser;
class QWidget;

class BtAboutDialog: public QDialog {
        Q_OBJECT
        class LicenseTab;
    public:
        BtAboutDialog(QWidget *parent = nullptr, Qt::WindowFlags wflags = Qt::Dialog);

    protected:
        void resizeEvent(QResizeEvent* event) override;

    private:

        void retranslateUi();
        void retranslateBtTab();
        void retranslateContributorsTab();
        void retranslateSwordTab();
        void retranslateQtTab();
        void retranslateLicenceTab();

    private:
        QTabWidget *m_tabWidget;
        QTextBrowser *m_bibletimeTab;
        QTextBrowser *m_contributorsTab;
        QTextBrowser *m_swordTab;
        QTextBrowser *m_qtTab;
        LicenseTab * m_licenseTab = nullptr;
        QDialogButtonBox *m_buttonBox;
        QLabel *m_iconLabel;
        QLabel *m_versionLabel;
};
