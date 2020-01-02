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

#include "cconfigurationdialog.h"

#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QLayout>
#include <QPushButton>
#include <QWidget>
#include "../../backend/config/btconfig.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../../util/directory.h"
#include "../messagedialog.h"
#include "btfontsettings.h"
#include "cacceleratorsettings.h"
#include "cdisplaysettings.h"
#include "cswordsettings.h"


namespace {
const QString GeometryKey = "GUI/SettingsDialog/geometry";
} // anonymous namespace

CConfigurationDialog::CConfigurationDialog(QWidget * parent, BtActionCollection* actionCollection )
        : BtConfigDialog(parent),
        m_actionCollection(actionCollection),
        m_displayPage(nullptr),
        m_swordPage(nullptr),
        m_acceleratorsPage(nullptr),
        m_fontsPage(nullptr),
        m_bbox(nullptr) {
    setWindowTitle(tr("Configure BibleTime"));
    setAttribute(Qt::WA_DeleteOnClose);

    // Add "Display" page
    m_displayPage = new CDisplaySettingsPage(this);
    addPage(m_displayPage);

    // Add "Desk" (sword) page
    m_swordPage = new CSwordSettingsPage(this);
    addPage(m_swordPage);

    // Add "Fonts" page
    m_fontsPage = new BtFontSettingsPage(this);
    addPage(m_fontsPage);

    // Add "Keyboard" (accelerators) page
    m_acceleratorsPage = new CAcceleratorSettingsPage(this);
    addPage(m_acceleratorsPage);

    // Dialog buttons
    m_bbox = new QDialogButtonBox(this);
    m_bbox->addButton(QDialogButtonBox::Ok);
    m_bbox->addButton(QDialogButtonBox::Apply);
    m_bbox->addButton(QDialogButtonBox::Cancel);
    message::prepareDialogBox(m_bbox);
    setButtonBox(m_bbox);
    BT_CONNECT(m_bbox, SIGNAL(clicked(QAbstractButton *)),
              SLOT(slotButtonClicked(QAbstractButton *)));

    loadDialogSettings();

    setCurrentPage(0);
}

CConfigurationDialog::~CConfigurationDialog() {
    saveDialogSettings();
}

/** Save the dialog settings **/
void CConfigurationDialog::save() {
    m_acceleratorsPage->save();
    m_fontsPage->save();
    m_swordPage->save();
    m_displayPage->save();
    emit signalSettingsChanged( );
}

/** Called if any button was clicked*/
void CConfigurationDialog::slotButtonClicked(QAbstractButton* button) {
    if (button == static_cast<QAbstractButton*>(m_bbox->button(QDialogButtonBox::Cancel))) {
        close();
        return;
    }

    save();

    if (button == static_cast<QAbstractButton*>(m_bbox->button(QDialogButtonBox::Ok)))
        close();
}

void CConfigurationDialog::loadDialogSettings() {
    restoreGeometry(btConfig().value<QByteArray>(GeometryKey, QByteArray()));
}

void CConfigurationDialog::saveDialogSettings() const {
    btConfig().setValue(GeometryKey, saveGeometry());
}
