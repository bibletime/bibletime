/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CDISPLAYSETTINGS_H
#define CDISPLAYSETTINGS_H

#include "frontend/bookshelfmanager/btconfigdialog.h"

#include <QWidget>


class QCheckBox;
class QComboBox;
class QWebView;

class CDisplaySettingsPage : public BtConfigPage {
        Q_OBJECT
    public:
        CDisplaySettingsPage(QWidget* parent);
        void save();
        QString iconName();
        QString label();
        QString header();

    protected slots:
        /** Update the style preview widget. */
        void updateStylePreview();

    private:
        QCheckBox* m_showLogoCheck;
        QComboBox* m_styleChooserCombo;
        QWebView* m_stylePreviewViewer;
};

#endif
