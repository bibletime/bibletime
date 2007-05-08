/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef COPTIONSDIALOG_H
#define COPTIONSDIALOG_H

#include "cprofilemgr.h"
#include "cbtconfig.h"
#include "util/cpointers.h"

//QT includes
#include <qwidget.h>
#include <qframe.h>
#include <qguardedptr.h>
#include <qlabel.h>
#include <qstring.h>
#include <qmap.h>
#include <qdict.h>
#include <qmap.h>

//KDE includes
#include <kdialogbase.h>
#include <kfontdialog.h>
#include <kaccel.h>
#include <kcolorbutton.h>
#include <kapp.h>
#include <klocale.h>

class QHBox;
class QCheckBox;
class QComboBox;
class QRadioButton;
class QWidgetStack;

class KListBox;
class KKeyChooser;
class KTabCtl;
class KHTMLPart;
class KActionCollection;

/**
 * The optionsdialog of BibleTime
  * @author The BibleTime Team
  */
class COptionsDialog : public KDialogBase, public CPointers  {
	Q_OBJECT

public:
	COptionsDialog(QWidget *parent, const char *name, KActionCollection* actionCollection);
	virtual ~COptionsDialog();

	enum Parts {
		Startup,
		Font,
		DisplayTemplates,
		Desk,
		Hotkeys
	};

	/** Opens the page which contaisn the given part ID. */
	const bool showPart( COptionsDialog::Parts ID );

private:
	struct Settings {
		struct StartupSettings {
			QCheckBox* showTips;
			QCheckBox* showLogo;
		}
		startup;

		struct SwordSettings {
			QComboBox* standardBible;
			QComboBox* standardCommentary;
			QComboBox* standardLexicon;
			QComboBox* standardDailyDevotional;
			QComboBox* standardHebrewStrong;
			QComboBox* standardGreekStrong;
			QComboBox* standardHebrewMorph;
			QComboBox* standardGreekMorph;
			QCheckBox* lineBreaks;
			QCheckBox* verseNumbers;
			QCheckBox* headings;
			QCheckBox* hebrewPoints;
			QCheckBox* hebrewCantillation;
			QCheckBox* morphSegmentation;
			QCheckBox* greekAccents;
			QCheckBox* textualVariants;
			QCheckBox* scriptureReferences;
		}
		swords; // not: sword instead of sword -> namespace error

		struct KeySettings {
			QComboBox* typeChooser;
			QWidgetStack* keyChooserStack;

			struct WindowType {
				QGuardedPtr<KKeyChooser> keyChooser;
				KActionCollection* actionCollection;
				QString title;

				WindowType() {
					keyChooser = 0;
					actionCollection = 0;
				};
				WindowType(const QString& newTitle) {
					title = newTitle;

					keyChooser = 0;
					actionCollection = 0;
				}
			};

			WindowType application;
			WindowType general;
			WindowType bible;
			WindowType commentary;
			WindowType lexicon;
			WindowType book;
		}
		keys;

		struct DisplayStyleSettings {
			QComboBox* styleChooser;
			KHTMLPart* stylePreview;
		}
		displayStyle;

		struct FontSettings {
			QComboBox* swordLocaleCombo;
			KFontChooser* fontChooser;
			QComboBox* usage;
			QCheckBox* useOwnFontBox;
			//the pair os to check whether the standard font (bool == false) or whether an own font should be used (true)
			QMap<QString,CBTConfig::FontSettingsPair> fontMap;
		}
		fonts;
	}
	m_settings;

protected slots: // Protected slots
	/**
	* Called when a new font in the fonts page was selected.
	*/
	void newDisplayWindowFontSelected(const QFont &);
	/**
	* Called when the combobox contents is changed
	*/
	void newDisplayWindowFontAreaSelected(const QString&);
	/**
	* Called if the OK button was clicked
	*/
	void slotOk();
	/**
	* Called if the Apply button was clicked
	* commented out for the time being.  ck
	*/
	void slotApply();
	/**
	* This slot is called when the "Use own font for language" bo was clicked.
	*/
	void useOwnFontClicked(bool);
	/**
	* Update the style preview widget
	*/
	void updateStylePreview();
	/**
	* The type of the keyaccel area changed
	*/
	void slotKeyChooserTypeChanged(const QString&);

protected: // Protected methods
	/**
	* Initializes the startup section of the OD.
	*/
	void initDisplay();
	/**
	* Init Sword section.
	*/
	void initSword();
	/**
	* Init accel key section.
	*/
	void initAccelerators();
	/**
	* Init fonts section.
	*/
	void initLanguages();

	/** Save the display settings.
	*/
	void saveDisplay();
	/** Save the key accel settings.
	*/
	void saveAccelerators();
	/** Save the font settings.
	*/
	void saveLanguages();
	/** Save the Sword specific settings.
	*/
	void saveSword();

signals: // Signals
	void signalSettingsChanged();
};

#endif
