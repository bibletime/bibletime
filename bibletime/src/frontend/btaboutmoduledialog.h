/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTABOUTMODULEDIALOG_H
#define BTABOUTMODULEDIALOG_H

#include <QDialog>


class CSwordModuleInfo;
class QWidget;

/**
    Dialog to show the information about a module.

    @author The BibleTime team <info@bibletime.info>
*/
class BTAboutModuleDialog : public QDialog {
    Q_OBJECT
    public:
        BTAboutModuleDialog(QWidget* parent, CSwordModuleInfo* info);
};

#endif
