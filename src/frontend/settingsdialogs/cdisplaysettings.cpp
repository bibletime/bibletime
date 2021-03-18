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
#include <localemgr.h>
#include <swlocale.h>

using SBLCI = std::list<sword::SWBuf>::const_iterator;

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
    BT_CONNECT(m_styleChooserCombo, SIGNAL(activated(int)),
               this,                SLOT(updateStylePreview()));

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
        const std::list<sword::SWBuf> locales = sword::LocaleMgr::getSystemLocaleMgr()->getAvailableLocales();
        for (SBLCI it = locales.begin(); it != locales.end(); ++it) {
            const char * abbr = sword::LocaleMgr::getSystemLocaleMgr()->getLocale((*it).c_str())->getName();
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
    using SSMCI = QMap<QString, QString>::const_iterator;

    QMap<QString, QString> languageNames;
    BT_ASSERT(CLanguageMgr::instance()->languageForAbbrev("en_US"));
    languageNames.insert(CLanguageMgr::instance()->languageForAbbrev("en_US")->translatedName(), "en_US");

    const std::list<sword::SWBuf> locales = sword::LocaleMgr::getSystemLocaleMgr()->getAvailableLocales();
    for (SBLCI it = locales.begin(); it != locales.end(); ++it) {
        const char * const abbreviation = sword::LocaleMgr::getSystemLocaleMgr()->getLocale((*it).c_str())->getName();
        const CLanguageMgr::Language * const l = CLanguageMgr::instance()->languageForAbbrev(abbreviation);

        if (l->isValid()) {
            languageNames.insert(l->translatedName(), abbreviation);
        } else {
            languageNames.insert(
                sword::LocaleMgr::getSystemLocaleMgr()->getLocale((*it).c_str())->getDescription(),
                abbreviation);
        }
    }

    int index = 0;
    QVector<QString> atv = bookNameAbbreviationsTryVector();
    for (SSMCI it = languageNames.constBegin(); it != languageNames.constEnd(); ++it) {
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
                     QString(tr("<div class=\"sectiontitle\">CHAPTER 3</div><div></div>")),
                     settings));

    tree.append( new CTextRendering::KeyTreeItem(
                     QString(tr("<div  xml:lang=\"en\" lang=\"en\" class=\"sectiontitle\">The New Birth </div>")),
                     settings));

    tree.append( new CTextRendering::KeyTreeItem(
                     QString("\n<span class=\"entryname\"><a name=\"crossref\" href=\"sword://Bible/WEB/John 3:1\">1 </a></span>%1")
                     .arg(tr(
"<span lemma=\"G1161\">Now</span> <span lemma=\"G444\">there was a man</span>"
"<span lemma=\"G5330\"> of the Pharisees</span>, <span lemma=\"G3686\">named</span> "
"<span class=\"crossreference\">"
"<a href=\"sword://Bible/NASB/John 7:50; \" name=crossref>John 7:50</a>;"
"<a href=\"sword://Bible/NASB/John 19:39; \" name=crossref>19:39</a>"
"</span>"
"<span lemma=\"G3530\"> Nicodemus</span>, a "
"<span class=\"crossreference\"><a href=\"sword://Bible/NASB/Luke 23:13; \" name=crossref>Luke 23:13</a>;"
"<a href=\"sword://Bible/NASB/John 7:26; \" name=crossref> John 7:26</a>,"
"<a href=\"sword://Bible/NASB/John 7:48; \" name=crossref>48</a></span> <span lemma=\"G758\">ruler</span>"
"<span lemma=\"G2453\"> of the Jews</span>;"
                    )), settings));

    tree.append( new CTextRendering::KeyTreeItem(
                     QString("\n<span class=\"entryname\"><a name=\"crossref\" href=\"sword://Bible/WEB/John 3:1\">2 </a></span>%1")
                     .arg(tr(
                              "<span lemma=\"G3778\">this </span>"
"<span lemma=\"G3778\">man </span>"
"<span lemma=\"G2064\">came </span>"
"<span lemma=\"G3571\">to Jesus by night </span>"
"<span lemma=\"G3004\">and said</span> to Him, <span class=\"crossreference\">"
"<a href=\"sword://Bible/NASB/Matthew 23:7; \" name=crossref>\"Matt 23:7</a>;"
"<a href=\"sword://Bible/NASB/John 3:26; \" name=crossref> John 3:26</a></span>"
"<span lemma=\"G4461\"> Rabbi</span>,"
"<span lemma=\"G3609a\">we know </span>"
"<span lemma=\"G2064\">that You have come </span>"
"<span lemma=\"G2316\">from God </span>"
"<span class=\"transchange\" title=\"Added text\"><span class=\"added\">as </span></span>"
"<span lemma=\"G1320\">a teacher</span>; "
"<span lemma=\"G3762\">for no </span>"
"<span lemma=\"G3762\">one </span>"
"<span lemma=\"G1410\">can </span>"
"<span lemma=\"G4160\">do </span>"
"<span lemma=\"G3778\">these </span>"
"<span class=\"footnote\" note=\"NASB/John 3:2/1\">(1)</span>"
"<span class=\"crossreference\"><a href=\"sword://Bible/NASB/John 2:11; \" name=crossref>John 2:11</a></span>"
"<span lemma=\"G4592\"> signs </span>"
"<span lemma=\"G4160\">that You do </span>"
"<span lemma=\"G1437|G3361\">unless </span>"
"<span class=\"crossreference\"><a href=\"sword://Bible/NASB/John 9:33; \" name=crossref>John 9:33</a>;"
"<a href=\"sword://Bible/NASB/John 10:38; \" name=crossref>10:38</a>;"
"<a href=\"sword://Bible/NASB/John 14:10-John 14:11; \" name=crossref>14:10f</a>;"
"<a href=\"sword://Bible/NASB/Acts 2:22; \" name=crossref>Acts 2:22</a>;"
"<a href=\"sword://Bible/NASB/Acts 10:38; \" name=crossref>10:38</a></span>"
"<span lemma=\"G2316\"> God</span> is with him.\""
                    )), settings));


    tree.append( new CTextRendering::KeyTreeItem(
                     QString("\n<span class=\"entryname\"><a name=\"crossref\" href=\"sword://Bible/WEB/John 3:1\">3 </a></span>%1")
                     .arg(tr(
"<span lemma=\"G2424\">Jesus answered and said to him</span>,"
"<span class=\"jesuswords\">\"Truly, truly, I say to you, unless one  "
"<span class=\"crossreference\">"
"<a href=\"sword://Bible/NASB/II Corinthians 5:17; \" name=crossref>2 Cor 5:17</a>; "
"<a href=\"sword://Bible/NASB/I Peter 1:23; \" name=crossref>1 Pet 1:23</a>"
"</span> "
"<span lemma=\"G1080\">is born </span>  "
"<span class=\"footnote\" note=\"NASB/John 3:3/1\">(1)</span> "
"<span lemma=\"G509\">again</span> "
"<span lemma=\"G1410|G3756\">he cannot</span> "
"<span lemma=\"G3708\">see</span> "
"<span class=\"crossreference\">"
"<a href=\"sword://Bible/NASB/Matthew 19:24; \" name=crossref>Matt 19:24</a>; "
"<a href=\"sword://Bible/NASB/Matthew 21:31; \" name=crossref>21:31</a>; "
"<a href=\"sword://Bible/NASB/Mark 9:47; \" name=crossref>Mark 9:47</a>; "
"<a href=\"sword://Bible/NASB/Mark 10:14-Mark 10:15; \" name=crossref>10:14f</a>; "
"<a href=\"sword://Bible/NASB/John 3:5; \" name=crossref>John 3:5</a>"
"</span> "
"<span lemma=\"G932\">the kingdom </span> "
"<span lemma=\"G2316\">of God </span>.\""
"</span>"
                    )), settings));

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

void CDisplaySettingsPage::save() {
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
