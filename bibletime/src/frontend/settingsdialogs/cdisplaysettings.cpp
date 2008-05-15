/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "cdisplaysettings.h"
#include "cdisplaysettings.moc"

#include "backend/config/cbtconfig.h"

#include "backend/rendering/cdisplayrendering.h"
#include "backend/managers/cdisplaytemplatemgr.h"

#include "util/cresmgr.h"
#include "util/ctoolclass.h"
#include "util/cpointers.h"


#include <khtml_part.h>
#include <khtmlview.h>

#include <QCheckBox>
#include <QComboBox>
#include <QVBoxLayout>
#include <QLabel>


/** Initializes the startup section of the OD. */
CDisplaySettingsPage::CDisplaySettingsPage(QWidget* parent)
	: QWidget(parent)
{

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setSpacing( 5 );

	{ //startup logo
		m_showLogoCheck = new QCheckBox(this);
		m_showLogoCheck->setText(tr("Show startuplogo"));
		m_showLogoCheck->setToolTip(CResMgr::settings::startup::showLogo::tooltip);


		m_showLogoCheck->setChecked(CBTConfig::get(CBTConfig::logo));
		layout->addWidget(m_showLogoCheck);
		layout->addSpacing(20);
	}

	layout->addWidget(
		CToolClass::explanationLabel(
				this,
				tr("Display templates"),
				tr("Display templates define how text is displayed. Please choose a template you like.")
				)
		);

	layout->addSpacing( 5 );

	QHBoxLayout* hboxlayout = new QHBoxLayout();

	m_styleChooserCombo = new QComboBox( this ); //create first to enable buddy for label
	connect( m_styleChooserCombo, SIGNAL( activated( int ) ),
			 this, SLOT( updateStylePreview() ) );

	QLabel* availableLabel = new QLabel(tr("Available display styles:"), this);
	availableLabel->setBuddy(m_styleChooserCombo);
	hboxlayout->addWidget(availableLabel);
	hboxlayout->addWidget( m_styleChooserCombo );
	hboxlayout->addStretch();
	layout->addLayout( hboxlayout );

	m_stylePreviewViewer = new KHTMLPart(this);
	QLabel* previewLabel = new QLabel(tr("Style preview"), this);
	previewLabel->setBuddy(m_stylePreviewViewer->view());
	layout->addWidget(previewLabel);
	layout->addWidget(m_stylePreviewViewer->view());
	m_styleChooserCombo->addItems(
		CPointers::displayTemplateManager()->availableTemplates()
	);

	for (int i = 0; i < m_styleChooserCombo->count(); ++i) {
		if ( m_styleChooserCombo->itemText(i) == CBTConfig::get(CBTConfig::displayStyle) ) {
			m_styleChooserCombo->setCurrentIndex( i );
			break;
		}
	}

	updateStylePreview(); //render it
}


void CDisplaySettingsPage::updateStylePreview() {
	//update the style preview widget
	qDebug("CDisplaySettingsPage::updateStylePreview");
	using namespace Rendering;

	const QString styleName = m_styleChooserCombo->currentText();
	qDebug() << "style name: " << styleName;
	CTextRendering::KeyTree tree;

	CTextRendering::KeyTreeItem::Settings settings;
	settings.highlight = false;

	tree.append( new CTextRendering::KeyTreeItem(
					 QString("\n<span class=\"entryname\"><a name=\"John316\" href=\"sword://Bible/WEB/John 3:16\">16</a></span>%1")
					 .arg(tr("For God so loved the world, that he gave his one and only Son, that whoever believes in him should not perish, but have eternal life.")),
					 settings));

	tree.append( new CTextRendering::KeyTreeItem(
					 QString("\n<span class=\"entryname\"><a name=\"John317\" href=\"sword://Bible/WEB/John 3:17\">17</a></span>%1")
					 .arg(tr("For God didn't send his Son into the world to judge the world, but that the world should be saved through him.")),
					 settings));

	settings.highlight = true;

	tree.append( new CTextRendering::KeyTreeItem(
					 QString("\n<span class=\"entryname\"><a name=\"John318\" href=\"sword://Bible/WEB/John 3:18\">18</a></span>%1")
					 .arg(tr("He who believes in him is not judged. He who doesn't believe has been judged already, because he has not believed in the name of the one and only Son of God.")),
					 settings) );

	settings.highlight = false;

	tree.append( new CTextRendering::KeyTreeItem(
					 QString("\n<span class=\"entryname\"><a name=\"John319\" href=\"sword://Bible/WEB/John 3:19\">19</a></span>%1")
					 .arg(tr("This is the judgment, that the light has come into the world, and men loved the darkness rather than the light; for their works were evil.")),
					 settings));

	tree.append( new CTextRendering::KeyTreeItem(
					 QString("\n<span class=\"entryname\"><a name=\"John320\" href=\"sword://Bible/WEB/John 3:20\">20</a></span>%1<br/>")
					 .arg(tr("For everyone who does evil hates the light, and doesn't come to the light, lest his works would be exposed.")),
					 settings));

	tree.append( new CTextRendering::KeyTreeItem(
					 QString("\n<span class=\"entryname\"><a name=\"John321\" href=\"sword://Bible/WEB/John 3:21\">21</a></span>%1")
					 .arg(tr("But he who does the truth comes to the light, that his works may be revealed, that they have been done in God.")),
					 settings));

	const QString oldStyleName = CBTConfig::get
									 (CBTConfig::displayStyle);
	//qDebug() << "old style name: " << oldStyleName;
	CBTConfig::set
		(CBTConfig::displayStyle, styleName);
	//qDebug() << "new style name: " << CBTConfig::get(CBTConfig::displayStyle);
	CDisplayRendering render;
	m_stylePreviewViewer->begin();
	m_stylePreviewViewer->write( render.renderKeyTree(tree));
	m_stylePreviewViewer->end();
	
	CBTConfig::set
		(CBTConfig::displayStyle, oldStyleName);
	qDebug("CDisplaySettingsPage::updateStylePreview end");
}

void CDisplaySettingsPage::save()
{
	CBTConfig::set
		( CBTConfig::logo, m_showLogoCheck->isChecked() );
	CBTConfig::set
		( CBTConfig::displayStyle, m_styleChooserCombo->currentText() );
}
