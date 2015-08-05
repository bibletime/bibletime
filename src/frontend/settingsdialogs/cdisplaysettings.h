/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CDISPLAYSETTINGS_H
#define CDISPLAYSETTINGS_H

#include "frontend/bookshelfmanager/btconfigdialog.h"

#include <QWidget>


class CConfigurationDialog;
class QCheckBox;
class QComboBox;
class QLabel;
class QWebView;

class CDisplaySettingsPage: public BtConfigDialog::Page {

        Q_OBJECT

    public: /* Methods: */

        CDisplaySettingsPage(CConfigurationDialog *parent = 0);

        void save();

        static void resetLanguage();

    protected: /* Methods: */

        void retranslateUi();

    protected slots:
        /** Update the style preview widget. */
        void updateStylePreview();

    private: /* Methods: */

        static QVector<QString> bookNameAbbreviationsTryVector();
        void initSwordLocaleCombo();

    private: /* Fields: */

        QLabel* m_showLogoLabel;
        QCheckBox* m_showLogoCheck;
        QLabel *m_languageNamesLabel;
        QComboBox* m_swordLocaleCombo;
        QComboBox* m_styleChooserCombo;
        QLabel *m_availableLabel;
        QWebView* m_stylePreviewViewer;
        QLabel *m_previewLabel;

};

#endif
