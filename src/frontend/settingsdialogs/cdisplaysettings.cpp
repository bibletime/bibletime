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

#include "cdisplaysettings.h"

#include <map>
#include <memory>
#include <QCheckBox>
#include <QComboBox>
#include <QFont>
#include <QLabel>
#include <QLocale>
#include <QMap>
#include <QNonConstOverload>
#include <QPalette>
#include <QSizePolicy>
#include <Qt>
#include <QVBoxLayout>
#include <QFormLayout>
#include <utility>
#include "../../backend/config/btconfig.h"
#include "../../backend/language.h"
#include "../../backend/managers/btlocalemgr.h"
#include "../../backend/managers/colormanager.h"
#include "../../backend/managers/cdisplaytemplatemgr.h"
#include "../../backend/rendering/cdisplayrendering.h"
#include "../../backend/rendering/ctextrendering.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../../util/tool.h"
#include "cconfigurationdialog.h"

// Sword includes:
#include <swbuf.h>


/** Initializes the startup section of the OD. */
CDisplaySettingsPage::CDisplaySettingsPage(CConfigurationDialog *parent)
        : BtConfigDialog::Page(CResMgr::settings::startup::icon(), parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QFormLayout *formLayout = new QFormLayout();

    //startup logo
    m_showLogoLabel = new QLabel(this);
    m_showLogoCheck = new QCheckBox(this);
    m_showLogoCheck->setChecked(
                btConfig().value<bool>(QStringLiteral("GUI/showSplashScreen"),
                                       true));
    formLayout->addRow(m_showLogoLabel, m_showLogoCheck);

    m_swordLocaleCombo = new QComboBox(this);
    m_languageNamesLabel = new QLabel(this);
    m_languageNamesLabel->setBuddy(m_swordLocaleCombo);
    formLayout->addRow(m_languageNamesLabel, m_swordLocaleCombo);

    initSwordLocaleCombo();

    // Light/Dark Mode
    m_lightDarkLabel = new QLabel(this);
    m_lightDarkCombo = new QComboBox(this);
    formLayout->addRow(m_lightDarkLabel, m_lightDarkCombo);

    m_styleChooserCombo = new QComboBox( this ); //create first to enable buddy for label
    BT_CONNECT(m_styleChooserCombo, qOverload<int>(&QComboBox::activated),
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
    QFont font = m_stylePreviewViewer->font();
    font.setPointSize(font.pointSize()+2);
    m_stylePreviewViewer->setFont(font);

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
    m_lightDarkCombo->setCurrentIndex(
                btConfig().value<int>(QStringLiteral("GUI/lightDarkMode")));
}

void CDisplaySettingsPage::retranslateUi() {
    setHeaderText(tr("Display"));

    m_languageNamesLabel->setText(tr("Language for names of Bible books:"));
    const QString toolTip(tr("The languages which can be used for the biblical book names. Translations are provided by the Sword library."));
    m_languageNamesLabel->setToolTip(toolTip);
    m_swordLocaleCombo->setToolTip(toolTip);

    m_showLogoLabel->setText(tr("Show startup logo:"));
    m_showLogoLabel->setToolTip(tr("Show the BibleTime logo on startup."));

    m_lightDarkLabel->setText(tr("Light / dark Mode (Requires restart)"));
    m_lightDarkCombo->addItem(tr("System default"));
    m_lightDarkCombo->addItem(tr("Light"));
    m_lightDarkCombo->addItem(tr("Dark"));

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

    BT_ASSERT(!atv.isEmpty());
    QString best = atv.takeLast();
    for (auto const & localePair : BtLocaleMgr::internalSwordLocales()) {
        auto const & swordAbbreviation = localePair.first;
        if (swordAbbreviation == "locales")
            continue;
        auto abbreviation =
                util::tool::fixSwordBcp47(swordAbbreviation.c_str());
        if (auto const i = atv.indexOf(abbreviation); i >= 0) {
            best = std::move(abbreviation);
            if (i == 0)
                break;
            atv.resize(i);
        }
    }
    btConfig().setValue(QStringLiteral("GUI/booknameLanguage"), best);
}

QVector<QString> CDisplaySettingsPage::bookNameAbbreviationsTryVector() {
    QVector<QString> atv;
    atv.reserve(4);
    {
        QString settingsLanguage = btConfig().booknameLanguage();
        if (!settingsLanguage.isEmpty())
            atv.append(settingsLanguage);
    }
    {
        auto localeLanguageAndCountry = QLocale().name(); // ISO 639 _ ISO 3166
        if (!localeLanguageAndCountry.isEmpty()) {
            localeLanguageAndCountry.replace('_', '-'); // Ensure BCP 47
            atv.append(localeLanguageAndCountry);
            if (auto const i = localeLanguageAndCountry.indexOf('-'); i > 0)
                atv.append(localeLanguageAndCountry.left(i));
        }
    }
    atv.append(Language::fromAbbrev({})->abbrev());
    return atv;
}

void CDisplaySettingsPage::initSwordLocaleCombo() {
    QMap<QString, QString> languageNames;
    {
        auto const defaultLanguage = Language::fromAbbrev({});
        languageNames.insert(defaultLanguage->translatedName(),
                             defaultLanguage->abbrev());
    }

    for (auto const & localePair : BtLocaleMgr::internalSwordLocales()) {
        auto const & swordAbbreviation = localePair.first;
        if (swordAbbreviation.size() <= 0)
            continue; // work around Sword not checking [Meta] Name= validity
        if (swordAbbreviation == "locales")
            continue;
        auto abbreviation =
                util::tool::fixSwordBcp47(swordAbbreviation.c_str());
        auto const l = Language::fromAbbrev(std::move(abbreviation));
        languageNames.insert(l->translatedName(), l->abbrev());
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

    auto const previewText =
            tr(R"====(<div class="sectiontitle">CHAPTER 3</div>

<div xml:lang="en" lang="en" class="sectiontitle">The New Birth</div>

<span class="entryname">
    <a name="crossref" href="sword://Bible/WEB/John 3:1">1</a>
</span>
<span lemma="G1161">Now</span>
<span lemma="G444">there was a man</span>
<span lemma="G5330">of the Pharisees</span>,
<span lemma="G3686">named</span>
<span class="crossreference">
    <a href="sword://Bible/NASB/John 7:50;" name="crossref">John 7:50</a>;
    <a href="sword://Bible/NASB/John 19:39;" name="crossref">19:39</a>
</span>
<span lemma="G3530">Nicodemus</span>, a
<span class="crossreference">
    <a href="sword://Bible/NASB/Luke 23:13;" name="crossref">Luke 23:13</a>;
    <a href="sword://Bible/NASB/John 7:26;" name="crossref">John 7:26</a>,<a href="sword://Bible/NASB/John 7:48;" name="crossref">48</a>
</span>
<span lemma="G758">ruler</span>
<span lemma="G2453">of the Jews</span>;

<span class="entryname">
    <a name="crossref" href="sword://Bible/WEB/John 3:2">2</a>
</span>
<span lemma="G3778">this</span>
<span lemma="G3778">man</span>
<span lemma="G2064">came</span>
<span lemma="G3571">to Jesus by night</span>
<span lemma="G3004">and said</span> to Him,
<span class="crossreference">
    <a href="sword://Bible/NASB/Matthew 23:7;" name="crossref">Matt 23:7</a>;
    <a href="sword://Bible/NASB/John 3:26;" name="crossref">John 3:26</a>
</span>
"<span lemma="G4461">Rabbi</span>,
<span lemma="G3609a">we know</span>
<span lemma="G2064">that You have come</span>
<span lemma="G2316">from God</span>
<span class="transchange" title="Added text">
    <span class="added">as</span>
</span>
<span lemma="G1320">a teacher</span>;
<span lemma="G3762">for no</span>
<span lemma="G3762">one</span>
<span lemma="G1410">can</span>
<span lemma="G4160">do</span>
<span lemma="G3778">these</span>
<span class="footnote" note="NASB/John 3:2/1">(1)</span>
<span class="crossreference">
    <a href="sword://Bible/NASB/John 2:11;" name="crossref">John 2:11</a>
</span>
<span lemma="G4592">signs</span>
<span lemma="G4160">that You do</span>
<span lemma="G1437|G3361">unless</span>
<span class="crossreference">
    <a href="sword://Bible/NASB/John 9:33;" name="crossref">John 9:33</a>;
    <a href="sword://Bible/NASB/John 10:38;" name="crossref">10:38</a>;
    <a href="sword://Bible/NASB/John 14:10-John 14:11;" name="crossref">
        14:10f</a>;
    <a href="sword://Bible/NASB/Acts 2:22;" name="crossref">Acts 2:22</a>;
    <a href="sword://Bible/NASB/Acts 10:38;" name="crossref">10:38</a>
</span>
<span lemma="G2316">God</span> is with him."

<span class="entryname">
    <a name="crossref" href="sword://Bible/WEB/John 3:3">3</a>
</span>
<span lemma="G2424">Jesus answered and said to him</span>,
<span class="jesuswords">
    "Truly, truly, I say to you, unless one
    <span class="crossreference">
        <a href="sword://Bible/NASB/II Corinthians 5:17;" name="crossref">
            2 Cor 5:17</a>;
        <a href="sword://Bible/NASB/I Peter 1:23;" name="crossref">
            1 Pet 1:23</a>
    </span>
    <span lemma="G1080">is born</span>
    <span class="footnote" note="NASB/John 3:3/1">(1)</span>
    <span lemma="G509">again</span>
    <span lemma="G1410|G3756">he cannot</span>
    <span lemma="G3708">see</span>
    <span class="crossreference">
        <a href="sword://Bible/NASB/Matthew 19:24;" name="crossref">
            Matt 19:24</a>;
        <a href="sword://Bible/NASB/Matthew 21:31;" name="crossref">21:31</a>;
        <a href="sword://Bible/NASB/Mark 9:47;" name="crossref">Mark 9:47</a>;
        <a href="sword://Bible/NASB/Mark 10:14-Mark 10:15;" name="crossref">
            10:14f</a>;
        <a href="sword://Bible/NASB/John 3:5;" name="crossref">John 3:5</a>
    </span>
    <span lemma="G932">the kingdom</span>
    <span lemma="G2316">of God</span>."
</span>)====",
               "This markup is used to render the preview text in the settings "
               "dialog. It contains a lot of markup because we want to "
               "showcase multiple textual features to the user comparing "
               "different display templates to choose from. For English we "
               "have chosen to use John 3:1-3 but translations are free to use "
               "other Bible passages as well as markup specific to the Bible "
               "translations of the language as long as the text continues to "
               "serve the intended purpose. At runtime, the translated text is "
               "automatically split into multiple verses when empty lines (two "
               "consecutive newline characters) are encountered.");
    for (auto const & verse
         : previewText.split(QStringLiteral("\n\n"), Qt::SkipEmptyParts))
        tree.emplace_back(verse, settings);

    CDisplayRendering render;
    render.setDisplayTemplateName(styleName);
    QString text = render.renderKeyTree(tree);
    text.replace(QStringLiteral("#CHAPTERTITLE#"), QString());

    // Update colors:
    QPalette p = m_stylePreviewViewer->palette();
    p.setColor(QPalette::Window, ColorManager::getBackgroundColor(styleName));
    p.setColor(QPalette::WindowText,
               ColorManager::getForegroundColor(styleName));
    m_stylePreviewViewer->setPalette(p);

    text = ColorManager::replaceColors(text, styleName);
    m_stylePreviewViewer->setText(text);
}

void CDisplaySettingsPage::save() const {
    btConfig().setValue(QStringLiteral("GUI/showSplashScreen"),
                        m_showLogoCheck->isChecked());
    btConfig().setValue(QStringLiteral("GUI/activeTemplateName"),
                        m_styleChooserCombo->currentText());
    btConfig().setValue(QStringLiteral("GUI/booknameLanguage"),
                        m_swordLocaleCombo->itemData(
                            m_swordLocaleCombo->currentIndex()));
    btConfig().setValue(QStringLiteral("GUI/lightDarkMode"),
                        m_lightDarkCombo->currentIndex());
}
