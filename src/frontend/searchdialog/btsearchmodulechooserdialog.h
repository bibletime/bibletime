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

#include "../btmodulechooserdialog.h"

#include <QObject>
#include <QString>
#include <Qt>
#include "../../backend/drivers/btconstmoduleset.h"


class QWidget;

class BtSearchModuleChooserDialog: public BtModuleChooserDialog {
    Q_OBJECT
    public:
        BtSearchModuleChooserDialog(QWidget *parent = nullptr,
                                    Qt::WindowFlags flags = Qt::WindowFlags());

        void setCheckedModules(BtConstModuleSet const & modules);

        BtConstModuleSet checkedModules() const;

    protected:
        void retranslateUi();
};
