/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/settingsdialogs/cdisplaysettings.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QWebView>
#include "backend/config/cbtconfig.h"
#include "backend/managers/cdisplaytemplatemgr.h"
#include "backend/rendering/cdisplayrendering.h"
#include "frontend/settingsdialogs/cconfigurationdialog.h"
#include "util/cresmgr.h"
#include "util/directory.h"
#include "util/tool.h"


// ***********************
// Container for QWebView to control its size
class CWebViewerWidget : public QWidget {
    public:
        CWebViewerWidget(QWidget* parent = 0);
        ~CWebViewerWidget();
        virtual QSize sizeHint () const;
};

CWebViewerWidget::CWebViewerWidget(QWidget* parent)
        : QWidget(parent) {
}

CWebViewerWidget::~CWebViewerWidget() {
}

QSize CWebViewerWidget::sizeHint () const {
    return QSize(100, 100);
}
// ************************

/** Initializes the startup section of the OD. */
CDisplaySettingsPage::CDisplaySettingsPage(CConfigurationDialog *parent)
        : BtConfigDialog::Page(util::directory::getIcon(CResMgr::settings::startup::icon), parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    { //startup logo
        m_showLogoCheck = new QCheckBox(this);
        m_showLogoCheck->setChecked(CBTConfig::get(CBTConfig::logo));
        mainLayout->addWidget(m_showLogoCheck);
    }
    mainLayout->addSpacing(20);

    m_explanationLabel = new QLabel(this);
    mainLayout->addWidget(m_explanationLabel);

    QHBoxLayout* hboxlayout = new QHBoxLayout();

    m_styleChooserCombo = new QComboBox( this ); //create first to enable buddy for label
    connect( m_styleChooserCombo, SIGNAL( activated( int ) ),
             this, SLOT( updateStylePreview() ) );

    m_availableLabel = new QLabel(this);
    m_availableLabel->setBuddy(m_styleChooserCombo);
    hboxlayout->addWidget(m_availableLabel);
    hboxlayout->addWidget( m_styleChooserCombo );
    hboxlayout->addStretch();
    mainLayout->addLayout( hboxlayout );

    QWidget* webViewWidget = new CWebViewerWidget(this);
    QLayout* webViewLayout = new QVBoxLayout(webViewWidget);
    m_stylePreviewViewer = new QWebView(webViewWidget);
    m_previewLabel = new QLabel(webViewWidget);
    m_previewLabel->setBuddy(m_stylePreviewViewer);
    webViewLayout->addWidget(m_previewLabel);
    webViewLayout->addWidget(m_stylePreviewViewer);
    webViewWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    mainLayout->addWidget(webViewWidget);

    CDisplayTemplateMgr * tMgr = CDisplayTemplateMgr::instance();
    m_styleChooserCombo->addItems(tMgr->availableTemplates());

    for (int i = 0; i < m_styleChooserCombo->count(); ++i) {
        if (m_styleChooserCombo->itemText(i) == CDisplayTemplateMgr::activeTemplateName()) {
            m_styleChooserCombo->setCurrentIndex(i);
            break;
        }
    }

    retranslateUi(); // also calls updateStylePreview();
}

void CDisplaySettingsPage::retranslateUi() {
    setHeaderText(tr("Display"));

    util::tool::initExplanationLabel(
        m_explanationLabel,
        tr("Display templates"),
        tr("Display templates define how text is displayed.")
    );

    m_showLogoCheck->setText(tr("Show startup logo"));
    m_showLogoCheck->setToolTip(tr("Show the BibleTime logo on startup"));

    m_availableLabel->setText(tr("Available display styles:"));
    m_previewLabel->setText(tr("Style preview"));

    updateStylePreview();
}


void CDisplaySettingsPage::updateStylePreview() {
    //update the style preview widget
    using namespace Rendering;

    const QString styleName = m_styleChooserCombo->currentText();
    CTextRendering::KeyTree tree;

    CTextRendering::KeyTreeItem::Settings settings;
    settings.highlight = false;

    tree.append( new CTextRendering::KeyTreeItem(
                     QString("\n<span class=\"entryname\"><a name=\"John316\" href=\"sword://Bible/WEB/John 3:16\">16</a></span>%1")
                     .arg(tr("For God so loved the world, that he gave his one and only Son, that whoever believes in him should not perish, but have eternal life.")),
                     settings));

    settings.highlight = true;

    tree.append( new CTextRendering::KeyTreeItem(
                     QString("\n<span class=\"entryname\"><a name=\"John317\" href=\"sword://Bible/WEB/John 3:17\">17</a></span>%1")
                     .arg(tr("For God didn't send his Son into the world to judge the world, but that the world should be saved through him.")),
                     settings));

    settings.highlight = false;

    tree.append( new CTextRendering::KeyTreeItem(
                     QString("\n<span class=\"entryname\"><a name=\"John318\" href=\"sword://Bible/WEB/John 3:18\">18</a></span>%1")
                     .arg(tr("He who believes in him is not judged. He who doesn't believe has been judged already, because he has not believed in the name of the one and only Son of God.")),
                     settings) );

    tree.append( new CTextRendering::KeyTreeItem(
                     QString("\n<span class=\"entryname\"><a name=\"John319\" href=\"sword://Bible/WEB/John 3:19\">19</a></span>%1")
                     .arg(tr("This is the judgement, that the light has come into the world, and men loved the darkness rather than the light; for their works were evil.")),
                     settings));

    tree.append( new CTextRendering::KeyTreeItem(
                     QString("\n<span class=\"entryname\"><a name=\"John320\" href=\"sword://Bible/WEB/John 3:20\">20</a></span>%1<br/>")
                     .arg(tr("For everyone who does evil hates the light, and doesn't come to the light, lest his works would be exposed.")),
                     settings));

    tree.append( new CTextRendering::KeyTreeItem(
                     QString("\n<span class=\"entryname\"><a name=\"John321\" href=\"sword://Bible/WEB/John 3:21\">21</a></span>%1")
                     .arg(tr("But he who does the truth comes to the light, that his works may be revealed, that they have been done in God.")),
                     settings));

    /// \todo Remove the following hack:
    const QString oldStyleName = CDisplayTemplateMgr::activeTemplateName();
    CBTConfig::set(CBTConfig::displayStyle, styleName);
    CDisplayRendering render;
    m_stylePreviewViewer->setHtml( render.renderKeyTree(tree));

    CBTConfig::set(CBTConfig::displayStyle, oldStyleName);
}

void CDisplaySettingsPage::save() {
    CBTConfig::set(CBTConfig::logo, m_showLogoCheck->isChecked());
    CBTConfig::set(CBTConfig::displayStyle, m_styleChooserCombo->currentText());
}
