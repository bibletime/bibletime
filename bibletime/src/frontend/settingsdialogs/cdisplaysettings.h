//
// C++ Interface: displaysettings
//
// Description: Widget for "Display" settings of configuration dialog.
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 1999-2007
//
// Copyright: See COPYING file that comes with this distribution
//
//


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
	QCheckBox* m_showTipsCheck;
	QCheckBox* m_showLogoCheck;
	QComboBox* m_styleChooserCombo;
	KHTMLPart* m_stylePreviewViewer;
};

#endif
