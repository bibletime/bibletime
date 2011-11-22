/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTLANGUAGESETTINGS_H
#define BTLANGUAGESETTINGS_H

#include "frontend/bookshelfmanager/btconfigdialog.h"

#include <QMap>
#include <QWidget>
#include "backend/config/cbtconfig.h"


//class CFontChooser;
//class QCheckBox;
class QComboBox;

/**
    @author The BibleTime team <info@bibletime.info>
*/
class BtLanguageSettingsPage : public BtConfigPage {
        Q_OBJECT
    public:
        BtLanguageSettingsPage(QWidget *parent = 0);
        ~BtLanguageSettingsPage();
        void save();

        /** Reimplemented from BtConfigPage. */
        virtual const QIcon &icon() const;

        /** Reimplemented from BtConfigPage. */
        virtual QString header() const;

        static void resetLanguage();

    protected slots:

        // This slot is called when the "Use own font for language" button was clicked.
        //void useOwnFontClicked(bool);

        // Called when a new font in the fonts page was selected.
        //void newDisplayWindowFontSelected(const QFont &);

    private: /* Methods: */

        static QVector<QString> bookNameAbbreviationsTryVector();
        void initSwordLocaleCombo();

        // Called when the combobox contents is changed
        //void newDisplayWindowFontAreaSelected(const QString&);

    private:
        QComboBox* m_swordLocaleCombo;
        //QComboBox* m_usageCombo;
        //QCheckBox* m_useOwnFontCheck;
        //CFontChooser* m_fontChooser;

        //QMap<QString, CBTConfig::FontSettingsPair> m_fontMap;
};

#endif
