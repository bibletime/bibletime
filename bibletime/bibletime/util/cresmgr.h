/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CRESMGR_H
#define CRESMGR_H

//Qt includes
#include <qstring.h>
#include <qkeysequence.h>

//KDE includes
#include <kshortcut.h>

/** Provides static functions to easily access the Tooltip texts for all the frontend parts.
  * @author The BibleTime team
  */
namespace CResMgr {
	void init_i18n();

	namespace modules {
		namespace bible {
			extern const QString icon_unlocked;
			extern const QString icon_locked;
			extern const QString icon_add;
		}
		namespace commentary {
			extern const QString icon_unlocked;
			extern const QString icon_locked;
			extern const QString icon_add;
		};
		namespace lexicon {
			extern const QString icon_unlocked;
			extern const QString icon_locked;
			extern const QString icon_add;
		};
		namespace book {
			extern const QString icon_unlocked;
			extern const QString icon_locked;
			extern const QString icon_add;
		};
	};

	namespace mainMenu { //Main menu
		namespace file { //Main menu->File
			namespace print { //a standard action
				extern QString tooltip;

			}
			namespace quit { //a standard action
				extern QString tooltip;
			}
		}

		namespace view { //Main menu->View
			namespace showMainIndex {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}
			namespace showInfoDisplay {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}
			namespace showToolBar { //a standard action
				extern QString tooltip;
			}
		}

		namespace mainIndex { //configuration for the main index and the view->search menu
			namespace search {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}
			namespace searchdefaultbible {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}
		}

		namespace window { //Main menu->Window
			namespace loadProfile {
				extern QString tooltip;

				extern const QString icon;
				extern const char* actionName;
			}
			namespace saveProfile {
				extern QString tooltip;

				extern const QString icon;
				extern const char* actionName;
			}
			namespace saveToNewProfile {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}
			namespace deleteProfile {
				extern QString tooltip;

				extern const QString icon;
				extern const char* actionName;
			}
			namespace showFullscreen {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}
			namespace arrangementMode {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;

				namespace manual {
					extern QString tooltip;

					extern const QString icon;
					extern const KShortcut accel;
					extern const char* actionName;
				}
				namespace autoTileVertical {
					extern QString tooltip;

					extern const QString icon;
					extern const KShortcut accel;
					extern const char* actionName;
				}
				namespace autoTileHorizontal {
					extern QString tooltip;

					extern const QString icon;
					extern const KShortcut accel;
					extern const char* actionName;
				}
				namespace autoCascade {
					extern QString tooltip;

					extern const QString icon;
					extern const KShortcut accel;
					extern const char* actionName;
				}
			}
			namespace tileVertical {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}
			namespace tileHorizontal {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}
			namespace cascade {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}
			namespace closeAll {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}
		}

		namespace settings { //Main menu->Settings
			namespace editToolBar { // available as KStdAction
				extern QString tooltip;

			}
			namespace optionsDialog { // available as KStdAction
				extern QString tooltip;

			}
			namespace swordSetupDialog {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}
		}

		namespace help { //Main menu->Help
			namespace handbook {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}
			namespace bibleStudyHowTo {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}
			namespace bugreport { // available as KStdAction
				extern QString tooltip;
			}
			namespace dailyTip {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}
			namespace aboutBibleTime { // available as KStdAction
				extern QString tooltip;
			}
			namespace aboutKDE { // available as KStdAction
				extern QString tooltip;
			}
		}
	}  //end of main menu

	namespace searchdialog {
		extern const QString icon;

		namespace searchButton {
			extern QString tooltip;
		}
		namespace cancelSearchButton {
			extern QString tooltip;
		}

		namespace options {
			namespace moduleChooserButton {
				extern QString tooltip;
			}
			namespace searchedText {
				extern QString tooltip;
			}
			namespace searchType {
				namespace multipleWords_and {
					extern QString tooltip;
				}
				namespace multipleWords_or {
					extern QString tooltip;
				}
				namespace exactMatch {
					extern QString tooltip;
				}
				namespace regExp {
					extern QString tooltip;
				}
			}

			namespace searchOptions {
				namespace caseSensitive {
					extern QString tooltip;
				}
			}
			namespace chooseScope {
				extern QString tooltip;
			}
			namespace scopeEditor {
				namespace rangeList {
					extern QString tooltip;
				}
				namespace nameEdit {
					extern QString tooltip;
				}
				namespace editRange {
					extern QString tooltip;
				}
				namespace parsedResult {
					extern QString tooltip;
				}
				namespace addNewRange {
					extern QString tooltip;
				}
				namespace deleteCurrentRange {
					extern QString tooltip;
				}

			}
		}
		namespace result {
			namespace moduleList {
				extern QString tooltip;

				namespace copyMenu {
					extern const QString icon;
				}
				namespace saveMenu {
					extern const QString icon;
				}
				namespace printMenu {
					extern const QString icon;
				}
			}
			namespace foundItems {
				extern QString tooltip;

				namespace copyMenu {
					extern const QString icon;
				}
				namespace saveMenu {
					extern const QString icon;
				}
				namespace printMenu {
					extern const QString icon;
				}

			}
			namespace textPreview {
				extern QString tooltip;
			}
		}
	}

namespace workspace {}

	namespace displaywindows {
		namespace transliteration {
			extern const QString icon;
		}
		namespace displaySettings {
			extern const QString icon;
		}

		namespace general {
			namespace scrollButton {
				extern QString tooltip;
			}
			namespace search {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}

			namespace backInHistory {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}
			namespace forwardInHistory {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}

			namespace findStrongs {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}
		}

		namespace bibleWindow {
			namespace bookList {
				extern QString tooltip;

			}
			namespace nextBook {
				extern QString tooltip;

				extern const KShortcut accel;
			}
			namespace previousBook {
				extern QString tooltip;

				extern const KShortcut accel;
			}

			namespace chapterList {
				extern QString tooltip;

			}
			namespace nextChapter {
				extern QString tooltip;

				extern const KShortcut accel;
			}
			namespace previousChapter {
				extern QString tooltip;

				extern const KShortcut accel;
			}

			namespace verseList {
				extern QString tooltip;

			}
			namespace nextVerse {
				extern QString tooltip;

				extern const KShortcut accel;
			}
			namespace previousVerse {
				extern QString tooltip;

				extern const KShortcut accel;
			}
			
			namespace copyMenu {
				extern const QString icon;
			}
			namespace saveMenu {
				extern const QString icon;
			}
			namespace printMenu {
				extern const QString icon;
			}
		}
		namespace commentaryWindow {
			namespace syncWindow {
				extern const QString icon;
				extern QString tooltip;

				extern const KShortcut accel;
				extern const char* actionName;
			}

		}

		namespace lexiconWindow {
			namespace entryList {
				extern QString tooltip;

			}
			namespace nextEntry {
				extern QString tooltip;

				extern const KShortcut accel;
			}
			namespace previousEntry {
				extern QString tooltip;

				extern const KShortcut accel;
			}

			namespace copyMenu {
				extern const QString icon;
			}
			namespace saveMenu {
				extern const QString icon;
			}
			namespace printMenu {
				extern const QString icon;
			}
		}
		namespace bookWindow {
			namespace toggleTree {
				extern const QString icon;
				extern const KShortcut accel;
			}
		}


		namespace writeWindow {
			namespace saveText {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}
			namespace restoreText {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}
			namespace deleteEntry {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}

			//formatting buttons
			namespace boldText {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}
			namespace italicText {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}
			namespace underlinedText {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}

			namespace alignLeft {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}
			namespace alignCenter {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}
			namespace alignRight {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}
			namespace alignJustify {
				extern QString tooltip;

				extern const QString icon;
				extern const KShortcut accel;
				extern const char* actionName;
			}

			namespace fontFamily {
				extern QString tooltip;

				extern const KShortcut accel;
				extern const char* actionName;
			}
			namespace fontSize {
				extern QString tooltip;

				extern const KShortcut accel;
				extern const char* actionName;
			}
			namespace fontColor {
				extern QString tooltip;

			}

		}
	}

	namespace settings {
		namespace startup {
			extern const QString icon;

			namespace dailyTip {
				extern QString tooltip;

			}
			namespace showLogo {
				extern QString tooltip;

			}
			namespace restoreWorkingArea {
				extern QString tooltip;

			}
		}
		namespace fonts {
			extern const QString icon;

			namespace typeChooser {
				extern QString tooltip;

			}
		}
		namespace profiles {
			extern const QString icon;

			namespace list {
				extern QString tooltip;

			}
			namespace createNew {
				extern QString tooltip;

			}
			namespace deleteCurrent {
				extern QString tooltip;

			}
			namespace renameCurrent {
				extern QString tooltip;

			}
		}
		namespace sword {
			extern const QString icon;

			namespace general {

				namespace language {
					extern QString tooltip;
				}
			}
			namespace modules {
				namespace bible {
					extern QString tooltip;
				}
				namespace commentary {
					extern QString tooltip;
				}
				namespace lexicon {
					extern QString tooltip;
				}
				namespace dailyDevotional {
					extern QString tooltip;
				}
				namespace hebrewStrongs {
					extern QString tooltip;
				}
				namespace greekStrongs {
					extern QString tooltip;
				}
				namespace hebrewMorph {
					extern QString tooltip;
				}
				namespace greekMorph {
					extern QString tooltip;
				}
			}
		}
		namespace keys {
			extern const QString icon;
		}
	}

	namespace mainIndex { //configuration for the main index and the view->search menu
		namespace search {
			extern QString tooltip;

			extern const QString icon;
			extern const KShortcut accel;
			extern const char* actionName;
		}
		namespace newFolder {
			extern const QString icon;
		}
		namespace changeFolder {
			extern const QString icon;
		}
		namespace openedFolder {
			extern const QString icon;
		}
		namespace closedFolder {
			extern const QString icon;
		}

		namespace bookmark {
			extern const QString icon;
		}
		namespace changeBookmark {
			extern const QString icon;
		}
		namespace importBookmarks {
			extern const QString icon;
		}
		namespace exportBookmarks {
			extern const QString icon;
		}
		namespace printBookmarks {
			extern const QString icon;
		}
		namespace deleteItems {
			extern const QString icon;
		}

		namespace editModuleMenu {
			extern const QString icon;
		}
		namespace editModulePlain {
			extern const QString icon;
		}
		namespace editModuleHTML {
			extern const QString icon;
		}

		namespace unlockModule {
			extern const QString icon;
		}
		namespace aboutModule {
			extern const QString icon;
		}
	}
}

#endif
