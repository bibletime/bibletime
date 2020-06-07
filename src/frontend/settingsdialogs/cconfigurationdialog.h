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

#ifndef CCONFIGURATIONDIALOG_H
#define CCONFIGURATIONDIALOG_H

#include "btconfigdialog.h"


class BtActionCollection;
class CAcceleratorSettingsPage;
class CDisplaySettingsPage;
class BtFontSettingsPage;
class CSwordSettingsPage;
class QAbstractButton;
class QDialogButtonBox;
class QWidget;

class CConfigurationDialog : public BtConfigDialog {

    Q_OBJECT

public: /* Methods: */

    CConfigurationDialog(QWidget * parent,
                         BtActionCollection * actionCollection);
    virtual ~CConfigurationDialog();

    void save();

Q_SIGNALS:

    void signalSettingsChanged();

private: /* Methods: */

    // Load the settings from the resource file
    void loadDialogSettings();

    // Save the settings to the resource file
    void saveDialogSettings() const;

private: /* Fields: */

    BtActionCollection * m_actionCollection = nullptr;
    CDisplaySettingsPage * m_displayPage = nullptr;
    CSwordSettingsPage * m_swordPage = nullptr;
    CAcceleratorSettingsPage * m_acceleratorsPage = nullptr;
    BtFontSettingsPage * m_fontsPage = nullptr;
    QDialogButtonBox * m_bbox = nullptr;

};

#endif
