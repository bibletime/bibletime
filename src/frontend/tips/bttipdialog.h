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
#include <QStringList>
#include <Qt>


class QCheckBox;
class QDialogButtonBox;
class QTextBrowser;
class QWidget;
/**
  The Tip Of The Day dialog.
*/
class BtTipDialog: public QDialog {
        Q_OBJECT

    public: // methods:

        BtTipDialog(QWidget *parent = nullptr, Qt::WindowFlags wflags = Qt::Dialog);

    private: // methods:

        void retranslateUi();

        /** Sends the current tip to the web view */
        void displayTip();

    private: // fields:

        QDialogButtonBox* m_buttonBox;
        QTextBrowser* m_tipView;
        QCheckBox* m_showTipsCheckBox;
        int m_tipNumber;
        QStringList m_tips;
};
