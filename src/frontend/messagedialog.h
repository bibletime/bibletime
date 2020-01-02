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

#ifndef UTIL_DIALOG_UTIL_H
#define UTIL_DIALOG_UTIL_H

#include <QMessageBox>

class QAction;
class QDialogButtonBox;

namespace message {

/**
  Change the state of a QAction without triggering their signals.
  \param action The QAction instance to set the state for.
  \param[in] checked The new state.
*/
void setQActionCheckedNoTrigger(QAction * const action, const bool checked = true);

/**
  Translates standard buttons in a QDialogButtonBox.
*/
void prepareDialogBox(QDialogButtonBox *box);

/*
  Use util::showWarning() etc. instead of QMessageBox static functions.
  QMessageBox button texts are not translated trustworthily.
*/

QMessageBox::StandardButton showWarning(QWidget * parent, const QString & title, const QString & text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

QMessageBox::StandardButton showCritical(QWidget * parent, const QString & title, const QString & text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

QMessageBox::StandardButton showInformation(QWidget * parent, const QString & title, const QString & text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

QMessageBox::StandardButton showQuestion(QWidget * parent, const QString & title, const QString & text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

} // namespace message

#endif
