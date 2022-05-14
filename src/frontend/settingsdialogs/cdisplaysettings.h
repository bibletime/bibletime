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
#include <QVector>


class CConfigurationDialog;
class QCheckBox;
class QComboBox;
class QLabel;

class CDisplaySettingsPage: public BtConfigDialog::Page {

        Q_OBJECT

    public: // methods:

        CDisplaySettingsPage(CConfigurationDialog *parent = nullptr);

        void save() const final override;

        static void resetLanguage();

    protected: // methods:

        void retranslateUi();

    private Q_SLOTS:
        /** Update the style preview widget. */
        void updateStylePreview();

    private: // methods:

        static QVector<QString> bookNameAbbreviationsTryVector();
        void initSwordLocaleCombo();

    private: // fields:

        QLabel* m_showLogoLabel;
        QCheckBox* m_showLogoCheck;
        QLabel *m_languageNamesLabel;
        QComboBox* m_swordLocaleCombo;
        QLabel* m_lightDarkLabel;
        QComboBox* m_lightDarkCombo;
        QLabel *m_transifexLabel;
        QComboBox* m_styleChooserCombo;
        QLabel *m_availableLabel;
        QLabel* m_stylePreviewViewer;
        QLabel *m_previewLabel;

};
