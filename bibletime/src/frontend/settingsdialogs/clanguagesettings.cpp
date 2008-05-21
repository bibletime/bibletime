//
// C++ Implementation: clanguagesettings
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 1999-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "clanguagesettings.h"
#include "clanguagesettings.moc"

#include "util/cpointers.h"
#include "util/ctoolclass.h"
#include "util/cresmgr.h"
#include "util/directoryutil.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>



#include <kfontchooser.h>


//Sword includes
#include <localemgr.h>
#include <swlocale.h>

CLanguageSettingsPage::CLanguageSettingsPage(QWidget *parent)
 : QWidget(parent)
{

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setSpacing( 5 );

	//Sword locales
	layout->addWidget(
	CToolClass::explanationLabel(
			this,
			tr("Language for names of Bible books"),
			tr("Select the language in which the Biblical book names are displayed.<blockquote><small>Translated names come from the Sword project.</small></blockquote>")
			));

	m_swordLocaleCombo = new QComboBox(this);
	QLabel* label = new QLabel( tr("Language for names of Bible books:"), this);
	label->setBuddy(m_swordLocaleCombo);
	m_swordLocaleCombo->setToolTip(CResMgr::settings::sword::general::language::tooltip);


	QHBoxLayout* hBoxLayout = new QHBoxLayout();
	hBoxLayout->addWidget(label);
	hBoxLayout->addWidget(m_swordLocaleCombo);
	hBoxLayout->addStretch();
	layout->addLayout(hBoxLayout);

	QStringList languageNames;
	languageNames.append( languageMgr()->languageForAbbrev("en_US")->translatedName() );

	std::list<sword::SWBuf> locales = sword::LocaleMgr::getSystemLocaleMgr()->getAvailableLocales();
	for (std::list<sword::SWBuf>::const_iterator it = locales.begin(); it != locales.end(); it++) {
		//    qWarning("working on %s", (*it).c_str());
		const CLanguageMgr::Language* const l = 
			CPointers::languageMgr()->languageForAbbrev( sword::LocaleMgr::getSystemLocaleMgr()->getLocale((*it).c_str())->getName() );

		if (l->isValid()) {
			languageNames.append( l->translatedName() );
		}
		else {
			languageNames.append(
				sword::LocaleMgr::getSystemLocaleMgr()->getLocale((*it).c_str())->getDescription()
			);
		}
	} //for

	languageNames.sort();
	m_swordLocaleCombo->addItems( languageNames );

	const CLanguageMgr::Language* const l =
		CPointers::languageMgr()->languageForAbbrev( CBTConfig::get(CBTConfig::language) );

	QString currentLanguageName;
	if ( l->isValid() && languageNames.contains(l->translatedName()) ) { //tranlated language name is in the box
		currentLanguageName = l->translatedName();
	}
	else { //a language like "German Abbrevs" might be the language to set
		sword::SWLocale* locale =
			sword::LocaleMgr::LocaleMgr::getSystemLocaleMgr()->getLocale( CBTConfig::get(CBTConfig::language).toLocal8Bit() );
		if (locale) {
			currentLanguageName = QString::fromLatin1(locale->getDescription());
		}
	}

	if (currentLanguageName.isEmpty()) { // set english as default if nothing was chosen
		Q_ASSERT(languageMgr()->languageForAbbrev("en_US"));
		currentLanguageName = languageMgr()->languageForAbbrev("en_US")->translatedName();
	}

	//now set the item with the right name as current item
	for (int i = 0; i < m_swordLocaleCombo->count(); ++i) {
		if (currentLanguageName == m_swordLocaleCombo->itemText(i)) {
			m_swordLocaleCombo->setCurrentIndex(i);
			break; //item found, finish the loop
		}
	}

	layout->addSpacing( 20 );

	//Font settings
	
	layout->addWidget(
		CToolClass::explanationLabel(
			this,
			tr("Custom fonts"),
			tr("You can specify a custom font for each language that needs a special font \
 to be displayed correctly.")
		)
	);
	layout->addSpacing(5);
	QHBoxLayout* hLayout = new QHBoxLayout();

	m_usageCombo = new QComboBox(this);
	m_usageCombo->setToolTip(CResMgr::settings::fonts::typeChooser::tooltip);

	hLayout->addWidget(m_usageCombo);

	CLanguageMgr::LangMap langMap = languageMgr()->availableLanguages();

	for (CLanguageMgr::LangMapIterator it = langMap.constBegin() ; it != langMap.constEnd(); ++it ) {
		const QString name =
			(*it)->translatedName().isEmpty()
			? (*it)->abbrev()
			: (*it)->translatedName();

		m_fontMap.insert(name, CBTConfig::get(*it) );
	}

	for( QMap<QString, CBTConfig::FontSettingsPair>::Iterator it = m_fontMap.begin(); it != m_fontMap.end(); ++it ) {
		if ( m_fontMap[it.key()].first ) { //show font icon
			m_usageCombo->addItem(util::filesystem::DirectoryUtil::getIcon("fonts.svg"), it.key() );
		}
		else { //don't show icon for font
			m_usageCombo->addItem(it.key());
		}
	}

	m_useOwnFontCheck = new QCheckBox(tr("Use custom font"), this);
	connect(m_useOwnFontCheck, SIGNAL(toggled(bool)), SLOT(useOwnFontClicked(bool)) );
	hLayout->addWidget(m_useOwnFontCheck);

	layout->addLayout(hLayout);
	//#warning TODO: remember the last selected font and jump there.

	m_fontChooser = new KFontChooser(this);
	//TODO: Eeli's wishlist: why not show something relevant here, like a Bible verse in chosen (not tr()'ed!) language?
	m_fontChooser->setSampleText(tr("The quick brown fox jumps over the lazy dog."));
	layout->addWidget(m_fontChooser);

	connect(m_fontChooser, SIGNAL(fontSelected(const QFont&)), SLOT(newDisplayWindowFontSelected(const QFont&)));
	connect(m_usageCombo, SIGNAL(activated(const QString&)), SLOT(newDisplayWindowFontAreaSelected(const QString&)));

	m_fontChooser->setFont( m_fontMap[m_usageCombo->currentText()].second );
	useOwnFontClicked( m_fontMap[m_usageCombo->currentText()].first );
	m_useOwnFontCheck->setChecked( m_fontMap[m_usageCombo->currentText()].first );
	m_fontChooser->setMinimumSize(m_fontChooser->sizeHint());


}


CLanguageSettingsPage::~CLanguageSettingsPage()
{
}

void CLanguageSettingsPage::save()
{
	for(QMap<QString, CBTConfig::FontSettingsPair>::Iterator it = m_fontMap.begin(); it != m_fontMap.end(); ++it ) {
		const CLanguageMgr::Language* const lang = languageMgr()->languageForTranslatedName(it.key());
		if (!lang->isValid()) { //we probably use a language, for which we have only the abbrev
			CLanguageMgr::Language l(it.key(), it.key(), it.key()); //create a temp language
			CBTConfig::set(&l, it.value());
		}
		else {
			CBTConfig::set(lang, it.value());
		}
	}


	QString languageAbbrev;

	const QString currentLanguageName = m_swordLocaleCombo->currentText();
	const CLanguageMgr::Language* const l = CPointers::languageMgr()->languageForTranslatedName( currentLanguageName );

	if (l && l->isValid()) {
		languageAbbrev = l->abbrev();
	}
	else { //it can be the lang abbrev like de_abbrev or the Sword description
		std::list <sword::SWBuf> locales = sword::LocaleMgr::getSystemLocaleMgr()->getAvailableLocales();

		for (std::list <sword::SWBuf>::iterator it = locales.begin(); it != locales.end(); it++) {
			sword::SWLocale* locale = sword::LocaleMgr::getSystemLocaleMgr()->getLocale((*it).c_str());
			Q_ASSERT(locale);

			if ( locale && (QString::fromLatin1(locale->getDescription()) == currentLanguageName) ) {
				languageAbbrev = QString::fromLatin1(locale->getName()); //we found the abbrevation for the current language
				break;
			}
		}

		if (languageAbbrev.isEmpty()) {
			languageAbbrev = currentLanguageName; //probably a non-standard locale name like de_abbrev
		}
	}

	if (!languageAbbrev.isEmpty()) {
		CBTConfig::set(CBTConfig::language, languageAbbrev);
	}
}

/**  */
void CLanguageSettingsPage::newDisplayWindowFontSelected(const QFont &newFont) {
	//belongs to the languages/fonts page
	CBTConfig::FontSettingsPair oldSettings = m_fontMap[ m_usageCombo->currentText() ];
	m_fontMap.insert( m_usageCombo->currentText(), CBTConfig::FontSettingsPair(oldSettings.first, newFont) );
}

/** Called when the combobox contents is changed */
void CLanguageSettingsPage::newDisplayWindowFontAreaSelected(const QString& usage)
{
	//belongs to fonts/languages
	useOwnFontClicked( m_fontMap[usage].first );
	m_useOwnFontCheck->setChecked( m_fontMap[usage].first );

	m_fontChooser->setFont( m_fontMap[usage].second );
}


/** This slot is called when the "Use own font for language" bo was clicked. */
void CLanguageSettingsPage::useOwnFontClicked( bool isOn ) {
	
	//belongs to fonts/languages
	
	m_fontChooser->setEnabled(isOn);
	m_fontMap[ m_usageCombo->currentText() ].first = isOn;

	if (isOn) { //show font icon
		m_usageCombo->setItemIcon(m_usageCombo->currentIndex(), util::filesystem::DirectoryUtil::getIcon("fonts.svg") );
	}
	else {    //don't show
		m_usageCombo->setItemText(m_usageCombo->currentIndex(), m_usageCombo->currentText() ); //TODO: should this change icon to empty?
	}
}
