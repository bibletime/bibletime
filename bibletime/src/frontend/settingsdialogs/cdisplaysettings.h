/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/


#ifndef CDISPLAYSETTINGS_H
#define CDISPLAYSETTINGS_H


#include <QWidget>

class QCheckBox;
class QComboBox;
class KHTMLPart;

class CDisplaySettingsPage : public QWidget
{
	Q_OBJECT

public:
	CDisplaySettingsPage(QWidget* parent);
	void save();

protected slots:
	/** Update the style preview widget. */
	void updateStylePreview();

private:
	QCheckBox* m_showLogoCheck;
	QComboBox* m_styleChooserCombo;
	KHTMLPart* m_stylePreviewViewer;
};

#endif
