/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTFONTSETTINGS_H
#define BTFONTSETTINGS_H

#include "frontend/bookshelfmanager/btconfigdialog.h"
#include "util/cpointers.h"

#include <QMap>
#include <QWidget>
#include "backend/config/cbtconfig.h"


class CFontChooser;
class QCheckBox;
class QComboBox;

/**
    @author The BibleTime team <info@bibletime.info>
*/
class BtFontSettingsPage : public BtConfigPage, CPointers {
        Q_OBJECT
    public:
        BtFontSettingsPage(QWidget *parent = 0);
        ~BtFontSettingsPage();
        void save();
        QString iconName();
        QString label();
        QString header();

    protected slots:

        // This slot is called when the "Use own font for language" button was clicked.
        void useOwnFontClicked(bool);

        // Called when a new font in the fonts page was selected.
        void newDisplayWindowFontSelected(const QFont &);

        // Called when the combobox contents is changed
        void newDisplayWindowFontAreaSelected(const QString&);

    private:
        //QComboBox* m_swordLocaleCombo;
        QComboBox* m_usageCombo;
        QCheckBox* m_useOwnFontCheck;
        CFontChooser* m_fontChooser;

        QMap<QString, CBTConfig::FontSettingsPair> m_fontMap;
};

#endif
