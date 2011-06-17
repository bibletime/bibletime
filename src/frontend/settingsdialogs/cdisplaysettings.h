/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
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
class QWebView;

class CDisplaySettingsPage: public BtConfigDialog::Page {

        Q_OBJECT

    public:

        CDisplaySettingsPage(CConfigurationDialog *parent = 0);

        void save();

        /** Reimplemented from BtConfigPage. */
        virtual const QIcon &icon() const;

        /** Reimplemented from BtConfigPage. */
        virtual QString header() const;

    protected slots:
        /** Update the style preview widget. */
        void updateStylePreview();

    private:
        QCheckBox* m_showLogoCheck;
        QComboBox* m_styleChooserCombo;
        QWebView* m_stylePreviewViewer;
};

#endif
