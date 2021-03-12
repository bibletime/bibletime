/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "cdisplaysettings.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLocale>
#include <QVBoxLayout>
#include <QFormLayout>
#include "../../backend/config/btconfig.h"
#include "../../backend/managers/colormanager.h"
#include "../../backend/managers/cdisplaytemplatemgr.h"
#include "../../backend/rendering/cdisplayrendering.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../../util/tool.h"
#include "../bibletimeapp.h"
#include "cconfigurationdialog.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <localemgr.h>
#include <swlocale.h>
#pragma GCC diagnostic pop


/** Initializes the startup section of the OD. */
CDisplaySettingsPage::CDisplaySettingsPage(CConfigurationDialog *parent)
        : BtConfigDialog::Page(CResMgr::settings::startup::icon(), parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QFormLayout *formLayout = new QFormLayout();

    //startup logo
    m_showLogoLabel = new QLabel(this);
    m_showLogoCheck = new QCheckBox(this);
    m_showLogoCheck->setChecked(btConfig().value<bool>("GUI/showSplashScreen", true));
    formLayout->addRow(m_showLogoLabel, m_showLogoCheck);

    m_swordLocaleCombo = new QComboBox(this);
    m_languageNamesLabel = new QLabel(this);
    m_languageNamesLabel->setBuddy(m_swordLocaleCombo);
    formLayout->addRow(m_languageNamesLabel, m_swordLocaleCombo);

    initSwordLocaleCombo();

    m_styleChooserCombo = new QComboBox( this ); //create first to enable buddy for label
    BT_CONNECT(m_styleChooserCombo,
               static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
               this, &CDisplaySettingsPage::updateStylePreview);

    m_availableLabel = new QLabel(this);
    m_availableLabel->setBuddy(m_styleChooserCombo);
    formLayout->addRow(m_availableLabel, m_styleChooserCombo );
    mainLayout->addLayout(formLayout);

    m_previewLabel = new QLabel(this);
    m_previewLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    m_stylePreviewViewer = new QLabel(this);
    m_stylePreviewViewer->setWordWrap(true);
    m_stylePreviewViewer->setTextFormat(Qt::RichText);
    m_stylePreviewViewer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    m_stylePreviewViewer->setAlignment(Qt::AlignTop);
    m_stylePreviewViewer->setMargin(15);
    m_stylePreviewViewer->setAutoFillBackground(true);

    mainLayout->addWidget(m_previewLabel);
    mainLayout->addWidget(m_stylePreviewViewer);

    CDisplayTemplateMgr * tMgr = CDisplayTemplateMgr::instance();
    m_styleChooserCombo->addItems(tMgr->availableTemplates());

    for (int i = 0; i < m_styleChooserCombo->count(); ++i) {
        if (m_styleChooserCombo->itemText(i) == CDisplayTemplateMgr::activeTemplateName()) {
            m_styleChooserCombo->setCurrentIndex(i);
            break;
        }
    }

    m_transifexLabel = new QLabel(this);
    mainLayout->addWidget(m_transifexLabel);

    retranslateUi(); // also calls updateStylePreview();
}

void CDisplaySettingsPage::retranslateUi() {
    setHeaderText(tr("Display"));

    m_languageNamesLabel->setText(tr("Language for names of Bible books:"));
    const QString toolTip(tr("The languages which can be used for the biblical book names. Translations are provided by the Sword library."));
    m_languageNamesLabel->setToolTip(toolTip);
    m_swordLocaleCombo->setToolTip(toolTip);

    m_showLogoLabel->setText(tr("Show startup logo:"));
    m_showLogoLabel->setToolTip(tr("Show the BibleTime logo on startup."));

    m_availableLabel->setText(tr("Available display styles:"));
    m_previewLabel->setText(tr("Style preview"));

    updateStylePreview();

    m_transifexLabel->setWordWrap(true);
    m_transifexLabel->setOpenExternalLinks(true);
    m_transifexLabel->setText(tr(
        "Did you know? You can help translating the GUI of BibleTime to your "
        "language at "
        "<a href=\"https://www.transifex.com/bibletime/bibletime/\">Transifex"
        "</a>."));
}

void CDisplaySettingsPage::resetLanguage() {
    QVector<QString> atv = bookNameAbbreviationsTryVector();

    QString best = "en_US";
    BT_ASSERT(atv.contains(best));
    int i = atv.indexOf(best);
    if (i > 0) {
        atv.resize(i);
        auto & localeMgr = *sword::LocaleMgr::getSystemLocaleMgr();
        auto const locales = localeMgr.getAvailableLocales();
        for (auto const & locale : locales) {
            const char * abbr = localeMgr.getLocale(locale.c_str())->getName();
            i = atv.indexOf(abbr);
            if (i >= 0) {
                best = abbr;
                if (i == 0)
                    break;
                atv.resize(i);
            }
        }
    }
    btConfig().setValue("GUI/booknameLanguage", best);
}

QVector<QString> CDisplaySettingsPage::bookNameAbbreviationsTryVector() {
    QVector<QString> atv;
    atv.reserve(4);
    {
        QString settingsLanguage = btConfig().value<QString>("GUI/booknameLanguage");
        if (!settingsLanguage.isEmpty())
            atv.append(settingsLanguage);
    }
    {
        const QString localeLanguageAndCountry = QLocale().name();
        if (!localeLanguageAndCountry.isEmpty()) {
            atv.append(localeLanguageAndCountry);
            int i = localeLanguageAndCountry.indexOf('_');
            if (i > 0)
                atv.append(localeLanguageAndCountry.left(i));
        }
    }
    BT_ASSERT(CLanguageMgr::instance()->languageForAbbrev("en_US"));
    atv.append("en_US");
    return atv;
}

void CDisplaySettingsPage::initSwordLocaleCombo() {
    QMap<QString, QString> languageNames;
    BT_ASSERT(CLanguageMgr::instance()->languageForAbbrev("en_US"));
    languageNames.insert(CLanguageMgr::instance()->languageForAbbrev("en_US")->translatedName(), "en_US");

    auto & localeMgr = *sword::LocaleMgr::getSystemLocaleMgr();
    for (auto const & locale : localeMgr.getAvailableLocales()) {
        const char * const abbreviation =
                localeMgr.getLocale(locale.c_str())->getName();
        const CLanguageMgr::Language * const l = CLanguageMgr::instance()->languageForAbbrev(abbreviation);

        if (l->isValid()) {
            languageNames.insert(l->translatedName(), abbreviation);
        } else {
            languageNames.insert(
                localeMgr.getLocale(locale.c_str())->getDescription(),
                abbreviation);
        }
    }

    int index = 0;
    QVector<QString> atv = bookNameAbbreviationsTryVector();
    for (auto it = languageNames.constBegin(); it != languageNames.constEnd(); ++it) {
        if (!atv.isEmpty()) {
            int i = atv.indexOf(it.value());
            if (i >= 0) {
                atv.resize(i);
                index = m_swordLocaleCombo->count();
            }
        }
        m_swordLocaleCombo->addItem(it.key(), it.value());
    }
    m_swordLocaleCombo->setCurrentIndex(index);
}


void CDisplaySettingsPage::updateStylePreview() {
    //update the style preview widget
    using namespace Rendering;

    const QString styleName = m_styleChooserCombo->currentText();
    CTextRendering::KeyTree tree;

    CTextRendering::KeyTreeItem::Settings settings;
    settings.highlight = false;

    tree.append( new CTextRendering::KeyTreeItem(
                     QString("\n<span class=\"entryname\"><a name=\"crossref\" href=\"sword://Bible/WEB/John 3:16\">16</a></span>%1")
                     .arg(tr("<span class=\"jesuswords\"> For God so loved the world, that he gave his one and only Son, that whoever believes in him should not perish, but have eternal life.</span>")),
                     settings));

    tree.append( new CTextRendering::KeyTreeItem(
                     QString("\n<span class=\"entryname\"><a name=\"crossref\" href=\"sword://Bible/WEB/John 3:17\">17</a></span>%1")
                     .arg(tr("<span class=\"jesuswords\">For God didn't send his Son into the world to judge the world, but that the world should be saved through him.</span>")),
                     settings));

    tree.append( new CTextRendering::KeyTreeItem(
                     QString("\n<span class=\"entryname\"><a name=\"crossref\" href=\"sword://Bible/WEB/John 3:18\">18</a></span>%1")
                     .arg(tr("<span class=\"jesuswords\">He who believes in him is not judged. He who doesn't believe has been judged already, because he has not believed in the name of the one and only Son of God.</span>")),
                     settings) );

    tree.append( new CTextRendering::KeyTreeItem(
                     QString("\n<span class=\"entryname\"><a name=\"crossref\" href=\"sword://Bible/WEB/John 3:19\">19</a></span>%1")
                     .arg(tr("<span class=\"jesuswords\">This is the judgement, that the light has come into the world, and men loved the darkness rather than the light; for their works were evil.</span>")),
                     settings));

    tree.append( new CTextRendering::KeyTreeItem(
                     QString("\n<span class=\"entryname\"><a name=\"crossref\" href=\"sword://Bible/WEB/John 3:20\">20</a></span>%1<br/>")
                     .arg(tr("<span class=\"jesuswords\">For everyone who does evil hates the light, and doesn't come to the light, lest his works would be exposed.</span>")),
                     settings));

    tree.append( new CTextRendering::KeyTreeItem(
                     QString("\n<span class=\"entryname\"><a name=\"crossref\" href=\"sword://Bible/WEB/John 3:21\">21</a></span>%1")
                     .arg(tr("<span class=\"jesuswords\">But he who does the truth comes to the light, that his works may be revealed, that they have been done in God.</span>")),
                     settings));

    /// \todo Remove the following hack:
    const QString oldStyleName = CDisplayTemplateMgr::activeTemplateName();
    btConfig().setValue("GUI/activeTemplateName", styleName);
    CDisplayRendering render;
    QString text = render.renderKeyTree(tree);
    text.replace("#CHAPTERTITLE#", "");
    updateColors();
    text = ColorManager::instance().replaceColors(text);
    m_stylePreviewViewer->setText(text);

    btConfig().setValue("GUI/activeTemplateName", oldStyleName);
}

void CDisplaySettingsPage::save() const {
    btConfig().setValue("GUI/showSplashScreen", m_showLogoCheck->isChecked() );
    btConfig().setValue("GUI/activeTemplateName", m_styleChooserCombo->currentText());
    btConfig().setValue("GUI/booknameLanguage", m_swordLocaleCombo->itemData(m_swordLocaleCombo->currentIndex()));
}

void CDisplaySettingsPage::updateColors(const QString& style) {
    QPalette p = m_stylePreviewViewer->palette();
    p.setColor(QPalette::Window, ColorManager::instance().getBackgroundColor(style));
    p.setColor(QPalette::WindowText, ColorManager::instance().getForegroundColor(style));
    m_stylePreviewViewer->setPalette(p);
}
