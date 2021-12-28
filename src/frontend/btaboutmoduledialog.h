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


class CSwordModuleInfo;
class QDialogButtonBox;
class QTextEdit;
class QWidget;

/**
    Dialog to show the information about a module.
*/
class BTAboutModuleDialog: public QDialog {

        Q_OBJECT

    public: // methods:

        BTAboutModuleDialog(const CSwordModuleInfo *moduleInfo,
                            QWidget *parent = nullptr,
                            Qt::WindowFlags flags = Qt::WindowFlags());

    protected: // methods:

        void retranslateUi();

    private: // fields:

        const CSwordModuleInfo *m_moduleInfo;
        QTextEdit        *m_textEdit;
        QDialogButtonBox *m_buttons;

}; /* class BTAboutModuleDialog */
