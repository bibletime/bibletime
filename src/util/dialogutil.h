/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef UTIL_DIALOG_UTIL_H
#define UTIL_DIALOG_UTIL_H

#include <QMessageBox>

class QDialogButtonBox;

namespace util {

/*
* Translate standard buttons in a QDialogButtonBox. Check that all used buttons are translated
* in the implementation.
*/
void prepareDialogBox(QDialogButtonBox *box);

/*
* Use util::showWarning() etc. instead of QMessageBox static functions.
* QMessageBox button texts are not translated trustworthily.
*/
QMessageBox::StandardButton showWarning(QWidget * parent, const QString & title, const QString & text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

QMessageBox::StandardButton showCritical(QWidget * parent, const QString & title, const QString & text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

QMessageBox::StandardButton showInformation(QWidget * parent, const QString & title, const QString & text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

QMessageBox::StandardButton showQuestion(QWidget * parent, const QString & title, const QString & text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

} // namespace util

#endif
