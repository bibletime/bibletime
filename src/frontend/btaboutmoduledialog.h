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

#ifndef BTABOUTMODULEDIALOG_H
#define BTABOUTMODULEDIALOG_H

#include <QDialog>


class CSwordModuleInfo;
class QDialogButtonBox;
class QTextEdit;

/**
    Dialog to show the information about a module.
*/
class BTAboutModuleDialog: public QDialog {

        Q_OBJECT

    public: /* Methods: */

        BTAboutModuleDialog(const CSwordModuleInfo *moduleInfo,
                            QWidget *parent = nullptr,
                            Qt::WindowFlags flags = Qt::WindowFlags());

    protected: /* Methods: */

        void retranslateUi();

    private: /* Fields: */

        const CSwordModuleInfo *m_moduleInfo;
        QTextEdit        *m_textEdit;
        QDialogButtonBox *m_buttons;

}; /* class BTAboutModuleDialog */

#endif
