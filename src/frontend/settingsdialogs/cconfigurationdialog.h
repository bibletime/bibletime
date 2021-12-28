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

#include "btconfigdialog.h"

#include <QObject>
#include <QString>
#include <Qt>


class QWidget;

class CConfigurationDialog : public BtConfigDialog {

    Q_OBJECT

public: // methods:

    CConfigurationDialog(QWidget * const parent = nullptr,
                         Qt::WindowFlags const f = Qt::WindowFlags());
    ~CConfigurationDialog() noexcept override;

};
