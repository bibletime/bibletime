/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

//TODO: rework this code

//own includes
#include "cresmgr.h"

#include <QObject> //for tr()

//KDE includes


namespace CResMgr {
	namespace modules {
		namespace bible {
			const QString icon_unlocked  = "bible.svg";
			const QString icon_locked    = "bible_locked.svg";
			const QString icon_add       = "bible_add.svg";
		} //bible
		namespace commentary {
			const QString icon_unlocked  = "commentary.svg";
			const QString icon_locked    = "commentary_locked.svg";
			const QString icon_add       = "commentary_add.svg";
		} //commentary
		namespace lexicon {
			const QString icon_unlocked  = "lexicon.svg";
			const QString icon_locked    = "lexicon_locked.svg";
			const QString icon_add       = "lexicon_add.svg";
		}//lexicon
		namespace book {
			const QString icon_unlocked  = "book.svg";
			const QString icon_locked    = "book_locked.svg";
			const QString icon_add       = "book_add.svg";
		}//book
	}//modules

	namespace categories {
		namespace bibles {
			const QString icon 			= "bible.svg";
		}
		namespace commentaries {
			const QString icon 			= "commentary.svg";
		}
		namespace lexicons {
			const QString icon 			= "dictionary.svg";
		}
		namespace dailydevotional {
			const QString icon 			= "calendar.svg";
		}
		namespace books {
			const QString icon 			= "books.svg";
		}
		namespace glossary {
			const QString icon 			= "dictionary.svg";
		}
		namespace images {
			const QString icon 			= "map.svg";
		}
		namespace cults {
			const QString icon 			= "questionable.svg";
		}
	}//categories
	namespace mainMenu { //Main menu
		namespace file { //Main menu->File
			namespace print { //a standard action
				QString tooltip;

			}
			namespace quit { //a standard action
				QString tooltip;

			}
		}//file

		namespace view { //Main menu->View
			namespace showMainIndex {
				QString tooltip;

				const QString icon        = "view_index.svg";
				const QKeySequence accel(Qt::Key_F9);
				const char* actionName    = "viewMainIndex_action";
			}
			namespace showInfoDisplay {
				QString tooltip;

				const QString icon        = "view_mag.svg";
				const QKeySequence accel(Qt::Key_F8);
				const char* actionName    = "viewInfoDisplay_action";
			}
			namespace showToolBar { //a standard action
				QString tooltip;

			}
		}//mainMenu::view

		namespace mainIndex {
			namespace search {
				QString tooltip;

				const QString icon        = "find.svg";
				const QKeySequence accel(Qt::CTRL + Qt::Key_O);
				const char* actionName    = "mainindex_search_action";
			}
			namespace searchdefaultbible {
				QString tooltip;

				const QString icon        = "find.svg";
				const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_F);
				const char* actionName    = "mainindex_searchdefaultbible_action";
			}
		}//mainMenu::mainIndex

		namespace window { //mainMenu::window
			namespace loadProfile {
				QString tooltip;

				const QString icon        = "view_profile.svg";
				const char* actionName    = "windowLoadProfile_action";
			}
			namespace saveProfile {
				QString tooltip;

				const QString icon        = "view_profile.svg";
				const char* actionName    = "windowSaveProfile_action";
			}
			namespace saveToNewProfile {
				QString tooltip;

				const QString icon        = "view_profile.svg";
				const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_S);
				const char* actionName    = "windowSaveToNewProfile_action";
			}
			namespace deleteProfile {
				QString tooltip;

				const QString icon        = "view_profile.svg";
				const char* actionName    = "windowDeleteProfile_action";
			}
			namespace showFullscreen {
				QString tooltip;

				const QString icon        = "window_fullscreen.svg";
				const QKeySequence accel(Qt::CTRL + Qt::SHIFT + Qt::Key_F);
				const char* actionName    = "windowFullscreen_action";
			}
			namespace arrangementMode {
				QString tooltip;

				const QString icon        = "cascade_auto.svg";
				const QKeySequence accel;
				const char* actionName    = "windowArrangementMode_action";

				namespace manual {
					QString tooltip;

					const QString icon        = "tile.svg";
					const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_M);
					const char* actionName    = "windowArrangementManual_action";
				}
				namespace autoTileHorizontal {
					QString tooltip;

					const QString icon        = "tile_auto.svg";
					const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_H);
					const char* actionName    = "windowAutoTileHorizontal_action";
				}
				namespace autoTileVertical {
					QString tooltip;

					const QString icon        = "tile_auto.svg";
					const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_G);
					const char* actionName    = "windowAutoTileVertical_action";
				}
				namespace autoCascade {
					QString tooltip;

					const QString icon        = "cascade_auto.svg";
					const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_J);
					const char* actionName    = "windowAutoCascade_action";
				}
			}
			namespace tileHorizontal {
				QString tooltip;

				const QString icon        = "tile.svg";
				const QKeySequence accel(Qt::CTRL + Qt::Key_H);
				const char* actionName    = "windowTileHorizontal_action";
			}
			namespace tileVertical {
				QString tooltip;

				const QString icon        = "tile.svg";
				const QKeySequence accel(Qt::CTRL + Qt::Key_G);
				const char* actionName    = "windowTileVertical_action";
			}
			namespace cascade {
				QString tooltip;

				const QString icon        = "cascade.svg";
				const QKeySequence accel(Qt::CTRL + Qt::Key_J);
				const char* actionName    = "windowCascade_action";
			}
			namespace closeAll {
				QString tooltip;

				const QString icon        = "fileclose.svg";
				const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_W);
				const char* actionName    = "windowCloseAll_action";
			}
		}//mainMenu::window

		namespace settings { //Main menu->Settings
			namespace editToolBar { // available as KStdAction
				QString tooltip;

			}
			namespace optionsDialog { // available as KStdAction
				QString tooltip;

			}
			namespace swordSetupDialog {
				QString tooltip;

				const QString icon        = "swordconfig.svg";
				const QKeySequence accel(Qt::Key_F4);
				const char* actionName    = "options_sword_setup";
			}

		}//mainMenu::settings

		namespace help { //Main menu->Help
			namespace handbook {
				QString tooltip;

				const QString icon        = "contents2.svg";
				const QKeySequence accel(Qt::Key_F1);
				const char* actionName    = "helpHandbook_action";
			}
			namespace bibleStudyHowTo {
				QString tooltip;

				const QString icon        = "contents2.svg";
				const QKeySequence accel(Qt::Key_F2);
				const char* actionName    = "helpHowTo_action";
			}
			namespace bugreport { // available as KStdAction
				QString tooltip;

			}
			namespace aboutBibleTime { // available as KStdAction
				QString tooltip;

			}
			namespace aboutKDE { // available as KStdAction
				QString tooltip;

			}
		}//mainMenu::help
	}  //end of mainMenu

	namespace searchdialog {
		const QString icon = "find.svg";

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
					const QString icon = "edit_copy.svg";
				}
				namespace saveMenu {
					const QString icon = "file_save.svg";
				}
				namespace printMenu {
					const QString icon = "print.svg";
				}
			}
			namespace foundItems {
				QString tooltip;


				namespace copyMenu {
					const QString icon = "edit_copy.svg";
				}
				namespace saveMenu {
					const QString icon = "file_save.svg";
				}
				namespace printMenu {
					const QString icon = "print.svg";
				}
			}
			namespace textPreview {
				QString tooltip;

			}
		}
	} //searchDialog

	namespace workspace {}

	namespace displaywindows {
/*		namespace transliteration {
			const QString icon = "bt_displaytranslit";
		}*/
		namespace displaySettings {
			const QString icon = "displayconfig.svg";
		}

		namespace general {
			namespace scrollButton {
				QString tooltip;
			}

			namespace search {
				QString tooltip;

				const QString icon        = "find.svg";
				const QKeySequence accel(Qt::CTRL + Qt::Key_L);
				const char* actionName    = "window_search_action";
			}

			namespace backInHistory {
				QString tooltip;

				const QString icon        = "back.svg";
				const QKeySequence accel(Qt::ALT + Qt::Key_Left);
				const char* actionName    = "window_history_back_action";
			}
			namespace forwardInHistory {
				QString tooltip;

				const QString icon        = "forward.svg";
				const QKeySequence accel(Qt::ALT + Qt::Key_Right);
				const char* actionName    = "window_history_forward_action";
			}
			namespace findStrongs {
				QString tooltip;

				const QString icon        = "bt_findstrongs.svg";
				const QKeySequence accel;
				const char* actionName    = "window_find_strongs_action";
			}

		}
		namespace bibleWindow {
			namespace bookList {
				QString tooltip;

			}
			namespace nextBook {
				QString tooltip;

				const QKeySequence accel(Qt::CTRL + Qt::Key_Y);
			}
			namespace previousBook {
				QString tooltip;

				const QKeySequence accel(Qt::CTRL + Qt::SHIFT + Qt::Key_Y);
			}

			namespace chapterList {
				QString tooltip;
			}
			namespace nextChapter {
				QString tooltip;
				const QKeySequence accel(Qt::CTRL + Qt::Key_X);
			}
			namespace previousChapter {
				QString tooltip;
				const QKeySequence accel(Qt::CTRL + Qt::SHIFT + Qt::Key_X);
			}
			namespace verseList {
				QString tooltip;
			}
			namespace nextVerse {
				QString tooltip;
				const QKeySequence accel(Qt::CTRL + Qt::Key_V);
			}
			namespace previousVerse {
				QString tooltip;
				const QKeySequence accel(Qt::CTRL + Qt::SHIFT + Qt::Key_V);
			}

			namespace copyMenu {
				const QString icon = "edit_copy.svg";
			}
			namespace saveMenu {
				const QString icon = "file_save.svg";
			}
			namespace printMenu {
				const QString icon = "print.svg";
			}
		}
		namespace commentaryWindow {
			namespace syncWindow {
				QString tooltip;
				const QString icon   = "sync.svg";
				const QKeySequence accel;
				const char* actionName = "commentary_syncWindow";
			}
		}
		namespace lexiconWindow {
			namespace entryList {
				QString tooltip;
			}
			namespace nextEntry {
				QString tooltip;
				const QKeySequence accel(Qt::CTRL + Qt::Key_V);
			}
			namespace previousEntry {
				QString tooltip;
				const QKeySequence accel(Qt::CTRL + Qt::SHIFT + Qt::Key_V);
			}

			namespace copyMenu {
				const QString icon = "edit_copy.svg";
			}
			namespace saveMenu {
				const QString icon = "file_save.svg";
			}
			namespace printMenu {
				const QString icon = "print.svg";
			}
		}
		namespace bookWindow {
			namespace toggleTree {
				const QString icon = "view_sidetree.svg";
				const QKeySequence accel;
			}
		}

		namespace writeWindow {
			namespace saveText {
				QString tooltip;

				const QString icon      = "file_save";
				const QKeySequence accel;
				const char* actionName  = "writeWindow_saveText";
			}
			namespace restoreText {
				QString tooltip;

				const QString icon      = "import.svg";
				const QKeySequence accel;
				const char* actionName  = "writeWindow_restoreText";
			}
			namespace deleteEntry {
				QString tooltip;

				const QString icon      = "edit_delete.svg";
				const QKeySequence accel;
				const char* actionName  = "writeWindow_deleteEntry";
			}

			//formatting buttons
			namespace boldText {
				QString tooltip;

				const QString icon      = "text_bold.svg";
				const QKeySequence accel;
				const char* actionName  = "writeWindow_boldText";
			}
			namespace italicText {
				QString tooltip;

				const QString icon      = "text_italic.svg";
				const QKeySequence accel;
				const char* actionName  = "writeWindow_italicText";
			}
			namespace underlinedText {
				QString tooltip;

				const QString icon      = "text_under.svg";
				const QKeySequence accel;
				const char* actionName  = "writeWindow_underlineText";
			}

			namespace alignLeft {
				QString tooltip;

				const QString icon      = "text_leftalign";
				const QKeySequence accel;
				const char* actionName  = "writeWindow_alignLeft";
			}
			namespace alignCenter {
				QString tooltip;

				const QString icon      = "text_center";
				const QKeySequence accel;
				const char* actionName  = "writeWindow_alignCenter";
			}
			namespace alignRight {
				QString tooltip;

				const QString icon      = "text_rightalign";
				const QKeySequence accel;
				const char* actionName  = "writeWindow_alignRight";
			}
			namespace alignJustify {
				QString tooltip;

				const QString icon      = "text_justify";
				const QKeySequence accel;
				const char* actionName = "writeWindow_alignJustify";
			}

			namespace fontFamily {
				QString tooltip;

				const QKeySequence accel;
				const char* actionName = "writeWindow_fontFamily";
			}
			namespace fontSize {
				QString tooltip;

				const QKeySequence accel;
				const char* actionName = "writeWindow_fontSize";
			}
			namespace fontColor {
				QString tooltip;
			}
		}
	}//displayWindows

	namespace settings {
		namespace startup {
			const QString icon = "startconfig.svg";
			namespace showLogo {
				QString tooltip;
			}
			namespace restoreWorkingArea {
				QString tooltip;
			}
		}
		namespace fonts {
			const QString icon = "fonts.svg";

			namespace typeChooser {
				QString tooltip;
			}
		}
		namespace profiles {
			const QString icon = "view_profile.svg";

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
			const QString icon = "swordconfig.svg";

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
			const QString icon = "key_bindings.svg";
		}
	}//settings

	namespace mainIndex { // Bookshelf view
		namespace search {
			QString tooltip;

			const QString icon        = "find.svg";
			const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_M);
			const char* actionName    = "GMsearch_action";
		}
		namespace newFolder {
			const QString icon = "folder_new.svg";
		}
		namespace changeFolder {
			const QString icon = "folder.svg";
		}
		namespace openedFolder {
			const QString icon = "folder_open.svg";
		}
		namespace closedFolder {
			const QString icon = "folder.svg";
		}

		namespace bookmark {
			const QString icon = "bookmark.svg";
		}
		namespace changeBookmark {
			const QString icon = "bookmark.svg";
		}
		namespace importBookmarks {
			const QString icon = "import.svg";
		}
		namespace exportBookmarks {
			const QString icon = "export.svg";
		}
		namespace printBookmarks {
			const QString icon = "print.svg";
		}
		namespace deleteItems {
			const QString icon = "edit_delete.svg";
		}

		namespace editModuleMenu {
			const QString icon = "pencil.svg";
		}
		namespace editModulePlain {
			const QString icon = "pencil.svg";
		}
		namespace editModuleHTML {
			const QString icon = "pencil.svg";
		}

		namespace unlockModule {
			const QString icon = "unlock.svg";
		}
		namespace aboutModule {
			const QString icon = "info.svg";
		}
		namespace grouping {
			const QString icon = "view-tree.svg";
		}
	}//mainIndex

	namespace bookshelfmgr {
		namespace installpage {
			const QString icon = "bible_add";
			const QString refresh_icon = "refresh";
			const QString delete_icon = "trash";
			const QString add_icon = "plus";
			const QString install_icon = "bible_add";
			const QString path_icon = "configure";
		}
		namespace removepage {
			const QString icon = "bible_remove";
			const QString remove_icon = "trash";
		}
		namespace indexpage {
			const QString icon = "document_magnifier";
			const QString create_icon = "folder_new";
			const QString delete_icon = "trash";
		}
		namespace paths {
			const QString add_icon = "plus";
			const QString edit_icon = "pencil";
			const QString remove_icon = "trash";
		}
	}
	
}



namespace CResMgr {
	void init_tr() {
		using namespace CResMgr;
		{
			using namespace mainMenu;
			{
				using namespace file;
				{
					using namespace print;
					tooltip     = QObject::tr("Open the printer dialog") ;
				}
				{
					using namespace quit;
					tooltip     = QObject::tr("Close BibleTime and save the settings") ;
				}

				{
					using namespace view;
					{
						using namespace showToolBar;
						tooltip     = QObject::tr("Toggle the main toolbar view") ;
					}
				}

				{
					using namespace window;
					
					{
						{
							using namespace arrangementMode;
							tooltip     = QObject::tr("Choose the way to arrange the windows") ;
						}
					}
					
				}
			}
		}

		{
			using namespace displaywindows;
			{
				using namespace bibleWindow;
				
			}
			{
				using namespace commentaryWindow;
				{
					using namespace syncWindow;
					tooltip = QObject::tr("Synchronize the displayed entry of this work with the active Bible window") ;
				}
			}
			{
				using namespace lexiconWindow;
				{
					using namespace entryList;
					tooltip = QObject::tr("Entries of the current work") ;
				}
				{
					using namespace nextEntry;
					tooltip = QObject::tr("Next entry") ;
				}
				{
					using namespace previousEntry;
					tooltip = QObject::tr("Previous entry") ;
				}
			}

			{
				using namespace writeWindow;
				{
					using namespace saveText;
					tooltip   = QObject::tr("Save the current text into the work overwriting the old text") ;
				}
				{
					using namespace restoreText;
					tooltip   = QObject::tr("Load the old text from the work into the edit area deleting the text in the area") ;
				}
				{
					using namespace deleteEntry;
					tooltip   = QObject::tr("Deletes the current entry out of the work (no undo!)") ;
				}

				//formatting buttons
				{
					using namespace boldText;
					tooltip   = QObject::tr("Bold") ;
				}
				{
					using namespace italicText;
					tooltip   = QObject::tr("Italic") ;
				}
				{
					using namespace underlinedText;
					tooltip   = QObject::tr("Underline") ;
				}

				{
					using namespace alignLeft;
					tooltip   = QObject::tr("Align left") ;
				}
				{
					using namespace alignCenter;
					tooltip   = QObject::tr("Centered") ;
				}
				{
					using namespace alignRight;
					tooltip   = QObject::tr("Align right") ;
				}
				{
					using namespace alignJustify;
					tooltip   = QObject::tr("Justified") ;
				}

				{
					using namespace fontFamily;
					tooltip   = QObject::tr("Font") ;
				}
				{ using namespace fontSize;
					tooltip   = QObject::tr("Font size") ;
				}
				{ using namespace fontColor;
					tooltip   = QObject::tr("Font color") ;
				}
			}
		}
		{
			using namespace settings;
			{
				using namespace startup;
				{
					using namespace showLogo;
					tooltip   = QObject::tr("Show the BibleTime logo on startup") ;
				}
				{
					using namespace restoreWorkingArea;
					tooltip   = QObject::tr("Save the user's session when BibleTime is closed and restore it on the next startup") ;
				}
			}
			{
				using namespace fonts;
				{
					using namespace typeChooser;
					tooltip   = QObject::tr("The font selection below will apply to all texts in this language") ;
				}
			}
			{
				using namespace settings::sword;
				{
					using namespace general;
					{
						using namespace language;
						tooltip   = QObject::tr("The languages which can be used for the biblical booknames") ;
					}
				}
				
			}
		}
	} //init_tr()	
} //CResMgr
