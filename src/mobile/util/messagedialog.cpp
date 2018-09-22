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

#include "messagedialog.h"

#include <QMessageBox>
#include <QString>


namespace message {

int showWarning(QWidget * /*parent*/,
                const QString & /*title*/,
                const QString & /*text*/,
                QMessageBox::StandardButtons /*buttons*/,
                QMessageBox::StandardButton /*defaultButton*/) {
    // TODO - implement showWarning
    return 0;
}

int showCritical(QWidget * /*parent*/,
                const QString & /*title*/,
                const QString & /*text*/,
                QMessageBox::StandardButtons /*buttons*/,
                QMessageBox::StandardButton /*defaultButton*/) {
    // TODO - implement showCritical
    return 0;
}

int showQuestion(QWidget * /*parent*/,
                const QString & /*title*/,
                const QString & /*text*/,
                QMessageBox::StandardButtons /*buttons*/,
                QMessageBox::StandardButton /*defaultButton*/) {
    // TODO - implement showQuestion
    return 0;
}

} // namespace message
