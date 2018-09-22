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

#ifndef UTIL_DIALOG_UTIL_H
#define UTIL_DIALOG_UTIL_H

#include <QString>
#include <QMessageBox>

class QWidget;

namespace message {

int showWarning(QWidget * parent,
                const QString & title,
                const QString & text,
                QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

int showCritical(QWidget * parent,
                const QString & title,
                const QString & text,
                QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

int showQuestion(QWidget * parent,
                const QString & title,
                const QString & text,
                QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

} // namespace message

#endif
