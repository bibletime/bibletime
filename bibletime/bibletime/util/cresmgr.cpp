/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//own includes
#include "cresmgr.h"

#include "backend/cswordbackend.h"

//KDE includes
#include <kglobal.h>
#include <klocale.h>

namespace CResMgr {
	namespace modules {
		namespace bible {
			const QString icon_unlocked  = "bt_bible";
			const QString icon_locked    = "bt_bible_locked";
			const QString icon_add       = "bt_bible_add";
		}
		namespace commentary {
			const QString icon_unlocked  = "bt_commentary";
			const QString icon_locked    = "bt_commentary_locked";
			const QString icon_add       = "bt_commentary_add";
		}
		namespace lexicon {
			const QString icon_unlocked  = "bt_lexicon";
			const QString icon_locked    = "bt_lexicon_locked";
			const QString icon_add       = "bt_lexicon_add";
		}
		namespace book {
			const QString icon_unlocked  = "bt_book";
			const QString icon_locked    = "bt_book_locked";
			const QString icon_add       = "bt_book_add";
		}
	}

	namespace mainMenu { //Main menu
		namespace file { //Main menu->File
			namespace print { //a standard action
				QString tooltip;

			}
			namespace quit { //a standard action
				QString tooltip;

			}
		}

		namespace view { //Main menu->View
			namespace showMainIndex {
				QString tooltip;

				const QString icon        = "view_sidetree";
				const KShortcut accel     = Qt::Key_F9;
				const char* actionName    = "viewMainIndex_action";
			}
			namespace showInfoDisplay {
				QString tooltip;

				const QString icon        = "view_sidetree";
				const KShortcut accel     = Qt::Key_F8;
				const char* actionName    = "viewInfoDisplay_action";
			}
			namespace showToolBar { //a standard action
				QString tooltip;

			}
		}

		namespace mainIndex { //Main menu->Settings
			namespace search {
				QString tooltip;

				const QString icon        = "find";
				const KShortcut accel     = Qt::CTRL + Qt::Key_O;
				const char* actionName    = "mainindex_search_action";
			}
			namespace searchdefaultbible {
				QString tooltip;

				const QString icon        = "find";
				const KShortcut accel     = Qt::CTRL + Qt::ALT + Qt::Key_F;
				const char* actionName    = "mainindex_searchdefaultbible_action";
			}
		}

		namespace window { //Main menu->Window
			namespace loadProfile {
				QString tooltip;

				const QString icon        = "view_sidetree";
				const char* actionName    = "windowLoadProfile_action";
			}
			namespace saveProfile {
				QString tooltip;

				const QString icon        = "view_sidetree";
				const char* actionName    = "windowSaveProfile_action";
			}
			namespace saveToNewProfile {
				QString tooltip;

				const QString icon        = "view_sidetree";
				const KShortcut accel     = Qt::CTRL + Qt::ALT + Qt::Key_S;
				const char* actionName    = "windowSaveToNewProfile_action";
			}
			namespace deleteProfile {
				QString tooltip;

				const QString icon        = "view_sidetree";
				const char* actionName    = "windowDeleteProfile_action";
			}
			namespace showFullscreen {
				QString tooltip;

				const QString icon        = "window_fullscreen";
				const KShortcut accel     = Qt::CTRL + Qt::SHIFT + Qt::Key_F;
				const char* actionName    = "windowFullscreen_action";
			}
			namespace arrangementMode {
				QString tooltip;

				const QString icon        = "bt_cascade_auto";
				const KShortcut accel     = KKeySequence();
				const char* actionName    = "windowArrangementMode_action";

				namespace manual {
					QString tooltip;

					const QString icon        = "bt_tile";
					const KShortcut accel     = Qt::CTRL + Qt::ALT + Qt::Key_M;
					const char* actionName    = "windowArrangementManual_action";
				}
				namespace autoTileHorizontal {
					QString tooltip;

					const QString icon        = "bt_tile_auto";
					const KShortcut accel     = Qt::CTRL + Qt::ALT + Qt::Key_H;
					const char* actionName    = "windowAutoTileHorizontal_action";
				}
				namespace autoTileVertical {
					QString tooltip;

					const QString icon        = "bt_tile_auto";
					const KShortcut accel     = Qt::CTRL + Qt::ALT + Qt::Key_G;
					const char* actionName    = "windowAutoTileVertical_action";
				}
				namespace autoCascade {
					QString tooltip;

					const QString icon        = "bt_cascade_auto";
					const KShortcut accel     = Qt::CTRL + Qt::ALT + Qt::Key_J;
					const char* actionName    = "windowAutoCascade_action";
				}
			}
			namespace tileHorizontal {
				QString tooltip;

				const QString icon        = "bt_tile";
				const KShortcut accel     = Qt::CTRL + Qt::Key_H;
				const char* actionName    = "windowTileHorizontal_action";
			}
			namespace tileVertical {
				QString tooltip;

				const QString icon        = "bt_tile";
				const KShortcut accel     = Qt::CTRL + Qt::Key_G;
				const char* actionName    = "windowTileVertical_action";
			}
			namespace cascade {
				QString tooltip;

				const QString icon        = "bt_cascade";
				const KShortcut accel     = Qt::CTRL + Qt::Key_J;
				const char* actionName    = "windowCascade_action";
			}
			namespace closeAll {
				QString tooltip;

				const QString icon        = "fileclose";
				const KShortcut accel     = Qt::CTRL + Qt::ALT + Qt::Key_W;
				const char* actionName    = "windowCloseAll_action";
			}
		}

		namespace settings { //Main menu->Settings
			namespace editToolBar { // available as KStdAction
				QString tooltip;

			}
			namespace optionsDialog { // available as KStdAction
				QString tooltip;

			}
			namespace swordSetupDialog {
				QString tooltip;

				const QString icon        = "bt_swordconfig";
				const KShortcut accel     = Qt::Key_F4;
				const char* actionName    = "options_sword_setup";
			}

		}

		namespace help { //Main menu->Help
			namespace handbook {
				QString tooltip;

				const QString icon        = "contents";
				const KShortcut accel     = Qt::Key_F1;
				const char* actionName    = "helpHandbook_action";
			}
			namespace bibleStudyHowTo {
				QString tooltip;

				const QString icon        = "contents";
				const KShortcut accel     = Qt::Key_F2;
				const char* actionName    = "helpHowTo_action";
			}
			namespace bugreport { // available as KStdAction
				QString tooltip;

			}
			namespace dailyTip {
				QString tooltip;

				const QString icon        = "idea";
				const KShortcut accel     = Qt::Key_F3;
				const char* actionName    = "helpDailyTip_action";
			}
			namespace aboutBibleTime { // available as KStdAction
				QString tooltip;

			}
			namespace aboutKDE { // available as KStdAction
				QString tooltip;

			}
		}
	}  //end of main menu

	namespace searchdialog {
		const QString icon = "find";

		namespace searchButton {
			QString tooltip;

		}
		namespace cancelSearchButton {
			QString tooltip;

		}

		namespace options {
			namespace moduleChooserButton {
				QString tooltip;

			}
			namespace searchedText {
				QString tooltip;

			}
			namespace searchType {
				namespace multipleWords_and {
					QString tooltip;

				}
				namespace multipleWords_or {
					QString tooltip;

				}
				namespace exactMatch {
					QString tooltip;

				}
				namespace regExp {
					QString tooltip;

				}
			}

			namespace searchOptions {
				namespace caseSensitive {
					QString tooltip;

				}
			}
			namespace chooseScope {
				QString tooltip;

			}
			namespace scopeEditor {
				namespace rangeList {
					QString tooltip;

				}
				namespace nameEdit {
					QString tooltip;

				}
				namespace editRange {
					QString tooltip;

				}
				namespace parsedResult {
					QString tooltip;

				}
				namespace addNewRange {
					QString tooltip;

				}
				namespace deleteCurrentRange {
					QString tooltip;

				}

			}
		}
		namespace result {
			namespace moduleList {
				QString tooltip;


				namespace copyMenu {
					const QString icon = "editcopy";
				}
				namespace saveMenu {
					const QString icon = "filesave";
				}
				namespace printMenu {
					const QString icon = "fileprint";
				}
			}
			namespace foundItems {
				QString tooltip;


				namespace copyMenu {
					const QString icon = "editcopy";
				}
				namespace saveMenu {
					const QString icon = "filesave";
				}
				namespace printMenu {
					const QString icon = "fileprint";
				}
			}
			namespace textPreview {
				QString tooltip;

			}
		}
	}

	namespace workspace {}

	namespace displaywindows {
/*		namespace transliteration {
			const QString icon = "bt_displaytranslit";
		}*/
		namespace displaySettings {
			const QString icon = "bt_displayconfig";
		}

		namespace general {
			namespace scrollButton {
				QString tooltip;
			}

			namespace search {
				QString tooltip;

				const QString icon        = "find";
				const KShortcut accel     = Qt::CTRL + Qt::Key_L;
				const char* actionName    = "window_search_action";
			}

			namespace backInHistory {
				QString tooltip;

				const QString icon        = "previous";
				const KShortcut accel     = Qt::ALT + Qt::Key_Left;
				const char* actionName    = "window_history_back_action";
			}
			namespace forwardInHistory {
				QString tooltip;

				const QString icon        = "next";
				const KShortcut accel     = Qt::ALT + Qt::Key_Right;
				const char* actionName    = "window_history_forward_action";
			}
			namespace findStrongs {
				QString tooltip;

				const QString icon        = "bt_findstrongs";
				const KShortcut accel     = KShortcut(0);
				const char* actionName    = "window_find_strongs_action";
			}

		}
		namespace bibleWindow {
			namespace bookList {
				QString tooltip;

			}
			namespace nextBook {
				QString tooltip;

				const KShortcut accel = Qt::CTRL + Qt::Key_Y;
			}
			namespace previousBook {
				QString tooltip;

				const KShortcut accel = Qt::CTRL + Qt::SHIFT + Qt::Key_Y;
			}

			namespace chapterList {
				QString tooltip;
			}
			namespace nextChapter {
				QString tooltip;
				const KShortcut accel = Qt::CTRL + Qt::Key_X;
			}
			namespace previousChapter {
				QString tooltip;
				const KShortcut accel = Qt::CTRL + Qt::SHIFT + Qt::Key_X;
			}
			namespace verseList {
				QString tooltip;
			}
			namespace nextVerse {
				QString tooltip;
				const KShortcut accel = Qt::CTRL + Qt::Key_V;
			}
			namespace previousVerse {
				QString tooltip;
				const KShortcut accel = Qt::CTRL + Qt::SHIFT + Qt::Key_V;
			}

			namespace copyMenu {
				const QString icon = "editcopy";
			}
			namespace saveMenu {
				const QString icon = "filesave";
			}
			namespace printMenu {
				const QString icon = "fileprint";
			}
		}
		namespace commentaryWindow {
			namespace syncWindow {
				QString tooltip;
				const QString icon   = "bt_sync";
				const KShortcut accel  = KKeySequence();
				const char* actionName = "commentary_syncWindow";
			}
		}
		namespace lexiconWindow {
			namespace entryList {
				QString tooltip;
			}
			namespace nextEntry {
				QString tooltip;
				const KShortcut accel = Qt::CTRL + Qt::Key_V;
			}
			namespace previousEntry {
				QString tooltip;
				const KShortcut accel = Qt::CTRL + Qt::SHIFT + Qt::Key_V;
			}

			namespace copyMenu {
				const QString icon = "editcopy";
			}
			namespace saveMenu {
				const QString icon = "filesave";
			}
			namespace printMenu {
				const QString icon = "fileprint";
			}
		}
		namespace bookWindow {
			namespace toggleTree {
				const QString icon = "view_sidetree";
				const KShortcut accel = KKeySequence();
			}
		}

		namespace writeWindow {
			namespace saveText {
				QString tooltip;

				const QString icon      = "filesave";
				const KShortcut accel   = KKeySequence();
				const char* actionName  = "writeWindow_saveText";
			}
			namespace restoreText {
				QString tooltip;

				const QString icon      = "undo";
				const KShortcut accel   = KKeySequence();
				const char* actionName  = "writeWindow_restoreText";
			}
			namespace deleteEntry {
				QString tooltip;

				const QString icon      = "editdelete";
				const KShortcut accel   = KKeySequence();
				const char* actionName  = "writeWindow_deleteEntry";
			}

			//formatting buttons
			namespace boldText {
				QString tooltip;

				const QString icon      = "text_bold";
				const KShortcut accel   = KKeySequence();
				const char* actionName  = "writeWindow_boldText";
			}
			namespace italicText {
				QString tooltip;

				const QString icon      = "text_italic";
				const KShortcut accel   = KKeySequence();
				const char* actionName  = "writeWindow_italicText";
			}
			namespace underlinedText {
				QString tooltip;

				const QString icon      = "text_under";
				const KShortcut accel   = KKeySequence();
				const char* actionName  = "writeWindow_underlineText";
			}

			namespace alignLeft {
				QString tooltip;

				const QString icon      = "text_left";
				const KShortcut accel   = KKeySequence();
				const char* actionName  = "writeWindow_alignLeft";
			}
			namespace alignCenter {
				QString tooltip;

				const QString icon      = "text_center";
				const KShortcut accel   = KKeySequence();
				const char* actionName  = "writeWindow_alignCenter";
			}
			namespace alignRight {
				QString tooltip;

				const QString icon      = "rightjust";
				const KShortcut accel   = KKeySequence();
				const char* actionName  = "writeWindow_alignRight";
			}
			namespace alignJustify {
				QString tooltip;

				const QString icon      = "text_block";
				const KShortcut accel   = KKeySequence();
				const char* actionName = "writeWindow_alignJustify";
			}

			namespace fontFamily {
				QString tooltip;

				const KShortcut accel   = KKeySequence();
				const char* actionName = "writeWindow_fontFamily";
			}
			namespace fontSize {
				QString tooltip;

				const KShortcut accel   = KKeySequence();
				const char* actionName = "writeWindow_fontSize";
			}
			namespace fontColor {
				QString tooltip;
			}
		}
	}

	namespace settings {
		namespace startup {
			const QString icon = "bt_startconfig";
			namespace dailyTip {
				QString tooltip;
			}
			namespace showLogo {
				QString tooltip;
			}
			namespace restoreWorkingArea {
				QString tooltip;
			}
		}
		namespace fonts {
			const QString icon = "fonts";

			namespace typeChooser {
				QString tooltip;
			}
		}
		namespace profiles {
			const QString icon = "view_sidetree";

			namespace list {
				QString tooltip;
			}
			namespace createNew {
				QString tooltip;
			}
			namespace deleteCurrent {
				QString tooltip;
			}
			namespace renameCurrent {
				QString tooltip;
			}
		}
		namespace sword {
			const QString icon = "bt_swordconfig";

			namespace general {
				namespace language {
					QString tooltip;
				}
			}
			namespace modules {
				namespace bible {
					QString tooltip;
				}
				namespace commentary {
					QString tooltip;
				}
				namespace lexicon {
					QString tooltip;

				}
				namespace dailyDevotional {
					QString tooltip;
				}
				namespace hebrewStrongs {
					QString tooltip;
				}
				namespace greekStrongs {
					QString tooltip;
				}
				namespace hebrewMorph {
					QString tooltip;
				}
				namespace greekMorph {
					QString tooltip;
				}
			}
		}
		namespace keys {
			const QString icon = "key_bindings";
		}
	}

	namespace mainIndex { //Main menu->Settings
		namespace search {
			QString tooltip;

			const QString icon        = "find";
			const KShortcut accel     = Qt::CTRL + Qt::ALT + Qt::Key_M;
			const char* actionName    = "GMsearch_action";
		}
		namespace newFolder {
			const QString icon = "folder_new";
		}
		namespace changeFolder {
			const QString icon = "folder";
		}
		namespace openedFolder {
			const QString icon = "folder_open";
		}
		namespace closedFolder {
			const QString icon = "folder";
		}

		namespace bookmark {
			const QString icon = "bookmark";
		}
		namespace changeBookmark {
			const QString icon = "bookmark";
		}
		namespace importBookmarks {
			const QString icon = "bookmark";
		}
		namespace exportBookmarks {
			const QString icon = "bookmark";
		}
		namespace printBookmarks {
			const QString icon = "fileprint";
		}
		namespace deleteItems {
			const QString icon = "filedelete";
		}

		namespace editModuleMenu {
			const QString icon = "pencil";
		}
		namespace editModulePlain {
			const QString icon = "pencil";
		}
		namespace editModuleHTML {
			const QString icon = "pencil";
		}

		namespace unlockModule {
			const QString icon = "unlock";
		}
		namespace aboutModule {
			const QString icon = "info";
		}
	}
}



namespace CResMgr {
	void init_i18n() {
		using namespace CResMgr;
		{
			using namespace mainMenu;
			{
				using namespace file;
				{
					using namespace print;
					tooltip     = i18n ("Open the printer dialog of BibleTime, where you can edit the print queue, assign styles to the items and print them.") ;
				}
				{
					using namespace quit;
					tooltip     = i18n ("Close BibleTime and save the settings.") ;
				}

				{
					using namespace view;
					{
						using namespace showMainIndex;
						tooltip     = i18n ("Show or hide the bookshelf.") ;
					}
					{
						using namespace showToolBar;
						tooltip     = i18n ("Toggle the main toolbar view.") ;
					}
				}

				{
					using namespace mainMenu::mainIndex;
					{
						using namespace search;
						tooltip     = i18n ("Open the search dialog to search in all works that are currently open.") ;
					}
					{
						using namespace searchdefaultbible;
						tooltip     = i18n ("Open the search dialog to search in the standard Bible.") ;
					}
				};

				{
					using namespace window;
					{
						using namespace loadProfile;
						tooltip     = i18n ("Restore a saved BibleTime session.") ;
					}
					{
						using namespace saveProfile;
						tooltip     = i18n ("Save current BibleTime session so that it can be reused later.") ;
					}
					{
						using namespace saveToNewProfile;
						tooltip     = i18n ("Create and save a new session.") ;
					}
					{
						using namespace deleteProfile;
						tooltip     = i18n ("Delete a BibleTime session.") ;
					}
					{
						using namespace showFullscreen;
						tooltip     = i18n ("Toggle fullscreen mode of the main window.") ;
					}
					{
						using namespace tileVertical;
						tooltip     = i18n ("Vertically tile the open windows.") ;
					}
					{
						using namespace tileHorizontal;
						tooltip     = i18n ("Horizontally tile the open windows.") ;
					}
					{
						using namespace cascade;
						tooltip     = i18n ("Cascade the open windows.") ;
					}
					{
						{
							using namespace arrangementMode;
							tooltip     = i18n ("Choose the way that is used to arrange the windows.") ;
						}
						{
							using namespace arrangementMode::autoTileVertical;
							tooltip     = i18n ("Automatically tile the open windows vertically.") ;
						}
						{
							using namespace arrangementMode::autoTileHorizontal;
							tooltip     = i18n ("Automatically tile the open windows horizontally.") ;
						}
						{
							using namespace arrangementMode::autoCascade;
							tooltip     = i18n ("Automatically cascade the open windows.") ;
						}
					}
					{
						using namespace closeAll;
						tooltip     = i18n ("Close all open windows.") ;
					}
				}

				{
					using namespace mainMenu::settings;
					{
						using namespace editToolBar;
						tooltip     = i18n ("Open BibleTime's toolbar editor.") ;
					}
					{
						using namespace optionsDialog;
						tooltip     = i18n ("Open the dialog to set most of BibleTime's preferences.") ;
					};
					{
						using namespace swordSetupDialog;
						tooltip     = i18n ("Open the dialog to configure your bookshelf and install/update/remove works.") ;
					}

				}

				{
					using namespace help;
					{
						using namespace handbook;
						tooltip     = i18n ("Open BibleTime's handbook in the KDE helpbrowser.") ;
					}
					{
						using namespace bibleStudyHowTo;
						tooltip     = i18n ("Open the Bible study HowTo included with BibleTime in the KDE helpbrowser. <BR>This HowTo is an introduction on how to study the Bible in an efficient way.") ;
					}
					{
						using namespace bugreport;
						tooltip     = i18n ("Send a bugreport to the developers of BibleTime.") ;
					}
					{
						using namespace dailyTip;
						tooltip     = i18n ("Show a daily tip each time BibleTime starts. <BR>The tips contain important Bible quotations and helpful tips for using BibleTime.") ;
					}
					{
						using namespace aboutBibleTime;
						tooltip     = i18n ("Show detailed information about BibleTime.") ;
					}
					{
						using namespace aboutKDE;
						tooltip     = i18n ("Show detailed information about the KDE project.") ;
					}
				}
			}
		}

		{
			using namespace searchdialog;
			{
				using namespace searchButton;
				tooltip     = i18n ("Start to search the text in each of the chosen work(s).") ;
			}
			{
				using namespace cancelSearchButton;
				tooltip     = i18n ("Stop the active search.") ;
			}

			{
				using namespace options;
				{
					using namespace moduleChooserButton;
					tooltip     = i18n ("Open a dialog to choose work(s) for the search.") ;
				}
				{
					using namespace searchedText;
					tooltip     = i18n ("Enter the text you want to search in the chosen work(s) here.") ;
				}
				{
					using namespace searchType;
					{
						using namespace multipleWords_and;
						tooltip     = i18n ("Treat the search text as multiple words. A text must contain all of the words to match. The order of the words is unimportant.") ;
					}
					{
						using namespace multipleWords_or;
						tooltip     = i18n ("Treat the search text as multiple words. A text must contain one or more words of to match. The order is unimportant.") ;
					}
					{
						using namespace exactMatch;
						tooltip     = i18n ("The search text will be used exactly as entered.") ;
					}
					{
						using namespace regExp;
						tooltip     = i18n ("Treat the search string as a GNU regular expression. The BibleTime handbook contains an introduction to regular expressions.") ;
					}
				}

				{
					using namespace searchOptions;
					{
						using namespace caseSensitive;
						tooltip     = i18n ("If you choose this option the search will distinguish between upper and lowercase characters.") ;
					}
				}
				{
					using namespace chooseScope;
					tooltip     = i18n ("Choose a scope from the list. \
Select the first item to use no scope, the second one is to use each work's last search result as search scope. \
The others are user defined search scopes.");
				}
				{
					using namespace scopeEditor;
					{
						using namespace rangeList;
						tooltip     = i18n ("Select an item from the list to edit the search scope.") ;
					}
					{
						using namespace nameEdit;
						tooltip     = i18n ("Change the name of the selected search scope.") ;
					}
					{
						using namespace editRange;
						tooltip     = i18n ("Change the search ranges of the selected search scope item. Have a look at the predefined search scopes to see how search ranges are constructed.") ;
					}
					{
						using namespace parsedResult;
						tooltip     = i18n ("Contains the search ranges which will be used for the search.") ;
					}
					{
						using namespace addNewRange;
						tooltip     = i18n ("Add a new search scope. First enter an appropriate name, then edit the search ranges.") ;
					}
					{
						using namespace deleteCurrentRange;
						tooltip     = i18n ("Deletes the selected search scope. If you close the dialog using Cancel the settings won't be saved.") ;
					}
				}
			}
			{
				using namespace result;
				{
					using namespace moduleList;
					tooltip     = i18n ("The list of works chosen for the search.") ;
				}
				{
					using namespace foundItems;
					tooltip     = i18n ("This list contains the search result of the selected work.") ;
				}
				{
					using namespace textPreview;
					tooltip     = i18n ("The text preview of the selected search result item.") ;
				}
			}
		}

		{
			using namespace displaywindows;
			{
				using namespace general;
				{
					{
						using namespace scrollButton;
						tooltip = i18n ("This button is useful to scroll through the entries of the list. Press the button and move the mouse to increase or decrease the item.") ;
					}
					{
						using namespace search;
						tooltip = i18n ("This button opens the search dialog with the work(s) of this window.") ;

					}
					{
						using namespace backInHistory;
						tooltip = i18n ("Go back one item in the display history.") ;
					}
					{
						using namespace forwardInHistory;
						tooltip = i18n ("Go forward one item in the display history.") ;

					}
					{
						using namespace findStrongs;
						tooltip = i18n ("Show all occurences of the Strong number currently under the mouse cursor.") ;
					}
				}
				using namespace bibleWindow;
				{
					using namespace bookList;
					tooltip = i18n ("This list contains the books which are available in this work.") ;
				}
				{
					using namespace nextBook;
					tooltip = i18n ("Show the next book of this work.") ;
				}
				{
					using namespace previousBook;
					tooltip = i18n ("Show the previous book of this work.") ;
				}
				{
					using namespace chapterList;
					tooltip = i18n ("This list contains the chapters which are available in the current book.") ;
				}
				{
					using namespace nextChapter;
					tooltip = i18n ("Show the next chapter of the work.") ;
				}
				{
					using namespace previousChapter;
					tooltip = i18n ("Show the previous chapter of the work.") ;
				}
				{
					using namespace verseList;
					tooltip = i18n ("This list contains the verses which are available in the current chapter.") ;
				}
				{
					using namespace nextVerse;
					tooltip = i18n ("In  Bible texts, the next verse will be highlighted. In commentaries, the next entry will be shown.") ;

				}
				{
					using namespace previousVerse;
					tooltip = i18n ("In Bible texts, the previous verse will be highlighted. In commentaries, the previous entry will be shown.") ;
				}
			}
			{
				using namespace commentaryWindow;
				{
					using namespace syncWindow;
					tooltip = i18n ("Synchronize the displayed entry of this work with the active Bible window.") ;
				}
			}
			{
				using namespace lexiconWindow;
				{
					using namespace entryList;
					tooltip = i18n ("This list contains the entries of the current work.") ;
				}
				{
					using namespace nextEntry;
					tooltip = i18n ("The next entry of the work will be shown.") ;
				}
				{
					using namespace previousEntry;
					tooltip = i18n ("The previous entry of the work will be shown.") ;
				}
			}

			{
				using namespace writeWindow;
				{
					using namespace saveText;
					tooltip   = i18n ("Save the curent text into the work. The old text will be overwritten.") ;
				}
				{
					using namespace restoreText;
					tooltip   = i18n ("Loads the old text from the work and loads it into the edit area. The unsaved text will be lost.") ;
				}
				{
					using namespace deleteEntry;
					tooltip   = i18n ("Deletes the current entry out of the work. The text will be lost.") ;
				}

				//formatting buttons
				{
					using namespace boldText;
					tooltip   = i18n ("Toggle bold formatting of the selected text.") ;
				}
				{
					using namespace italicText;
					tooltip   = i18n ("Toggle italic formatting of the selected text.") ;
				}
				{
					using namespace underlinedText;
					tooltip   = i18n ("Toggle underlined formatting of the selected text.") ;
				}

				{
					using namespace alignLeft;
					tooltip   = i18n ("The text will be aligned on the left side of the page.") ;
				}
				{
					using namespace alignCenter;
					tooltip   = i18n ("Centers the text horizontally.") ;
				}
				{
					using namespace alignRight;
					tooltip   = i18n ("Aligns the text on the right side of the page.") ;
				}
				{
					using namespace alignJustify;
					tooltip   = i18n ("Justifies the text on the page.") ;
				}

				{
					using namespace fontFamily;
					tooltip   = i18n ("Choose a new font for the selected text.") ;
				}
				{ using namespace fontSize;
					tooltip   = i18n ("Choose a new font size for the selected text.") ;
				}
				{ using namespace fontColor;
					tooltip   = i18n ("Choose a new color for the selected text.") ;
				}
			}
		}
		{
			using namespace settings;
			{
				using namespace startup;
				{
					using namespace dailyTip;
					tooltip   = i18n ("Activate this box to see a daily tip on startup.") ;
				}
				{
					using namespace showLogo;
					tooltip   = i18n ("Activate this to see the BibleTime logo on startup.") ;
				}
				{
					using namespace restoreWorkingArea;
					tooltip   = i18n ("Save the user's session when BibleTime is closed and restore it on the next startup.") ;
				}
			}
			{
				using namespace fonts;
				{
					using namespace typeChooser;
					tooltip   = i18n ("The font selection below will apply to all texts in this language.") ;
				}
			}
			{
				using namespace settings::sword;
				{
					using namespace general;
					{
						using namespace language;
						tooltip   = i18n ("Contains the languages which can be used for the biblical booknames.") ;
					}
				}
				{
					using namespace settings::sword::modules;
					{
						using namespace bible;
						tooltip   = i18n ("The standard Bible is used when a hyperlink into a Bible is clicked.") ;
					}
					{
						using namespace commentary;
						tooltip   = i18n ("The standard commentary is used when a hyperlink into a commentary is clicked.") ;
					}
					{
						using namespace lexicon;
						tooltip   = i18n ("The standard lexicon is used when a hyperlink into a lexicon is clicked.") ;
					}
					{
						using namespace dailyDevotional;
						tooltip   = i18n ("The standard devotional will be used to display a short start up devotional.") ;
					}
					{
						using namespace hebrewStrongs;
						tooltip   = i18n ("The standard Hebrew lexicon is used when a hyperlink into a Hebrew lexicon is clicked.") ;
					}
					{
						using namespace greekStrongs;
						tooltip   = i18n ("The standard Greek lexicon is used when a hyperlink into a Greek lexicon is clicked.") ;
					}
					{
						using namespace hebrewMorph;
						tooltip   = i18n ("The standard morphological lexicon for Hebrew texts is used when a hyperlink of a morphological tag in a Hebrew text is clicked.") ;
					}
					{
						using namespace greekMorph;
						tooltip   = i18n ("The standard morphological lexicon for Greek texts is used when a hyperlink of a morphological tag in a Greek text is clicked.") ;
					}
				}
			}
		}
		{
			using namespace mainIndex;
			{
				using namespace search;
				tooltip     = i18n ("Opens the search dialog to search in the work(s) that are currently open.") ;
			}
		}
	}
}
