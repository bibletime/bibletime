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

#include "cconfigurationdialog.h"

#include <QByteArray>
#include "../../backend/config/btconfig.h"
#include "btfontsettings.h"
#include "bttexttospeechsettings.h"
#include "cacceleratorsettings.h"
#include "cdisplaysettings.h"
#include "cswordsettings.h"


namespace {
auto const GeometryKey = QStringLiteral("GUI/SettingsDialog/geometry");
} // anonymous namespace

CConfigurationDialog::CConfigurationDialog(QWidget * const parent,
                                           Qt::WindowFlags const f)
    : BtConfigDialog(parent, f)
{
    setWindowTitle(tr("Configure BibleTime"));
    setAttribute(Qt::WA_DeleteOnClose);

    addPage(new CDisplaySettingsPage(this));
    addPage(new CSwordSettingsPage(this));
    addPage(new BtFontSettingsPage(this));
    addPage(new CAcceleratorSettingsPage(this));
#ifdef BUILD_TEXT_TO_SPEECH
    addPage(new BtTextToSpeechSettingsPage(this));
#endif

    restoreGeometry(btConfig().value<QByteArray>(GeometryKey, QByteArray()));
}

CConfigurationDialog::~CConfigurationDialog() noexcept
{ btConfig().setValue(GeometryKey, saveGeometry()); }
