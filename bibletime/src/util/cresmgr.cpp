/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "util/cresmgr.h"

#include <QString>


namespace CResMgr {
namespace mainWindow {
const QString icon = "bibletime.svg";
} // mainWindow
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
const QString icon 			= "lexicon.svg";
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

namespace view { //Main menu->View
namespace showMainIndex {
const QString icon        = "view_index.svg";
const QKeySequence accel(Qt::Key_F9);
const char* actionName    = "viewMainIndex_action";
}
namespace showInfoDisplay {
const QString icon        = "view_mag.svg";
const QKeySequence accel(Qt::Key_F8);
const char* actionName    = "viewInfoDisplay_action";
}
}//mainMenu::view

namespace mainIndex {
namespace search {
const QString icon        = "find.svg";
const QKeySequence accel(Qt::CTRL + Qt::Key_O);
const char* actionName    = "mainindex_search_action";
}
namespace searchdefaultbible {
const QString icon        = "find.svg";
const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_F);
const char* actionName    = "mainindex_searchdefaultbible_action";
}
}//mainMenu::mainIndex

namespace window { //mainMenu::window
namespace loadProfile {
const QString icon        = "view_profile.svg";
const char* actionName    = "windowLoadProfile_action";
}
namespace saveProfile {
const QString icon        = "view_profile.svg";
const char* actionName    = "windowSaveProfile_action";
}
namespace saveToNewProfile {
const QString icon        = "view_profile.svg";
const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_S);
const char* actionName    = "windowSaveToNewProfile_action";
}
namespace deleteProfile {
const QString icon        = "view_profile.svg";
const char* actionName    = "windowDeleteProfile_action";
}
namespace showFullscreen {
const QString icon        = "window_fullscreen.svg";
const QKeySequence accel(Qt::Key_F5);
const char* actionName    = "windowFullscreen_action";
}
namespace arrangementMode {
const QString icon        = "cascade_auto.svg";
const QKeySequence accel;
const char* actionName    = "windowArrangementMode_action";

namespace manual {
const QString icon        = "tile.svg";
const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_M);
const char* actionName    = "windowArrangementManual_action";
}
namespace autoTileHorizontal {
const QString icon        = "tile_horiz.svg";
const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_H);
const char* actionName    = "windowAutoTileHorizontal_action";
}
namespace autoTileVertical {
const QString icon        = "tile_vert.svg";
const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_G);
const char* actionName    = "windowAutoTileVertical_action";
}
namespace autoTile {
const QString icon        = "tile_auto.svg";
const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_I);
const char* actionName    = "windowAutoTile_action";
}
namespace autoCascade {
const QString icon        = "cascade_auto.svg";
const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_J);
const char* actionName    = "windowAutoCascade_action";
}
}
namespace tileHorizontal {
const QString icon        = "tile_horiz.svg";
const QKeySequence accel(Qt::CTRL + Qt::Key_H);
const char* actionName    = "windowTileHorizontal_action";
}
namespace tileVertical {
const QString icon        = "tile_vert.svg";
const QKeySequence accel(Qt::CTRL + Qt::Key_G);
const char* actionName    = "windowTileVertical_action";
}
namespace tile {
const QString icon        = "tile.svg";
const QKeySequence accel(Qt::CTRL + Qt::Key_I);
const char* actionName    = "windowTile_action";
}
namespace cascade {
const QString icon        = "cascade.svg";
const QKeySequence accel(Qt::CTRL + Qt::Key_J);
const char* actionName    = "windowCascade_action";
}
namespace close {
const QString icon        = "fileclose.svg";
const QKeySequence accel(Qt::CTRL + Qt::Key_W);
const char* actionName    = "windowClose_action";
}
namespace closeAll {
const QString icon        = "fileclose.svg";
const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_W);
const char* actionName    = "windowCloseAll_action";
}
}//mainMenu::window

namespace settings { //Main menu->Settings
namespace swordSetupDialog {
const QString icon        = "swordconfig.svg";
const QKeySequence accel(Qt::Key_F4);
const char* actionName    = "options_sword_setup";
}

}//mainMenu::settings

namespace help { //Main menu->Help
namespace handbook {
const QString icon        = "contents2.svg";
const QKeySequence accel(Qt::Key_F1);
const char* actionName    = "helpHandbook_action";
}
namespace bibleStudyHowTo {
const QString icon        = "contents2.svg";
const QKeySequence accel(Qt::Key_F2);
const char* actionName    = "helpHowTo_action";
}
}//mainMenu::help
}  //end of mainMenu

namespace searchdialog {
const QString icon = "find.svg";
const QString close_icon = "stop.svg";
const QString help_icon = "questionmark";
const QString chooseworks_icon = "checkbox";
const QString setupscope_icon = "configure";

namespace result {
namespace moduleList {

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
}
} //searchDialog

namespace displaywindows {
/*		namespace transliteration {
			const QString icon = "bt_displaytranslit";
		}*/
namespace displaySettings {
const QString icon = "displayconfig.svg";
}

namespace general {
const QString removemoduleicon = "fileclose";
const QString addmoduleicon = "plus";
const QString replacemoduleicon = "checkbox";
namespace search {
const QString icon        = "find.svg";
const QKeySequence accel(Qt::CTRL + Qt::Key_N);
const char* actionName    = "window_search_action";
}

namespace backInHistory {
const QString icon        = "back.svg";
const QKeySequence accel(Qt::ALT + Qt::Key_Left);
const char* actionName    = "window_history_back_action";
}
namespace forwardInHistory {
const QString icon        = "forward.svg";
const QKeySequence accel(Qt::ALT + Qt::Key_Right);
const char* actionName    = "window_history_forward_action";
}
namespace findStrongs {
const QString icon        = "bt_findstrongs.svg";
const QKeySequence accel;
const char* actionName    = "window_find_strongs_action";
}

}
namespace bibleWindow {
namespace nextBook {
const QKeySequence accel(Qt::CTRL + Qt::Key_Y);
}
namespace previousBook {
const QKeySequence accel(Qt::CTRL + Qt::SHIFT + Qt::Key_Y);
}

namespace nextChapter {
const QKeySequence accel(Qt::CTRL + Qt::Key_X);
}
namespace previousChapter {
const QKeySequence accel(Qt::CTRL + Qt::SHIFT + Qt::Key_X);
}
namespace nextVerse {
const QKeySequence accel(Qt::CTRL + Qt::Key_V);
}
namespace previousVerse {
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
const QKeySequence accel(Qt::CTRL + Qt::Key_V);
}
namespace previousEntry {
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
const QString icon      = "file_save";
const QKeySequence accel;
const char* actionName  = "writeWindow_saveText";
}
namespace restoreText {
const QString icon      = "import.svg";
const QKeySequence accel;
const char* actionName  = "writeWindow_restoreText";
}
namespace deleteEntry {
const QString icon      = "edit_delete.svg";
const QKeySequence accel;
const char* actionName  = "writeWindow_deleteEntry";
}

//formatting buttons
namespace boldText {
const QString icon      = "text_bold.svg";
const QKeySequence accel;
const char* actionName  = "writeWindow_boldText";
}
namespace italicText {
const QString icon      = "text_italic.svg";
const QKeySequence accel;
const char* actionName  = "writeWindow_italicText";
}
namespace underlinedText {
const QString icon      = "text_under.svg";
const QKeySequence accel;
const char* actionName  = "writeWindow_underlineText";
}

namespace alignLeft {
const QString icon      = "text_leftalign";
const QKeySequence accel;
const char* actionName  = "writeWindow_alignLeft";
}
namespace alignCenter {
const QString icon      = "text_center";
const QKeySequence accel;
const char* actionName  = "writeWindow_alignCenter";
}
namespace alignRight {
const QString icon      = "text_rightalign";
const QKeySequence accel;
const char* actionName  = "writeWindow_alignRight";
}
namespace alignJustify {
const QString icon      = "text_justify";
const QKeySequence accel;
const char* actionName = "writeWindow_alignJustify";
}

namespace fontFamily {
const QKeySequence accel;
const char* actionName = "writeWindow_fontFamily";
}
namespace fontSize {
const QKeySequence accel;
const char* actionName = "writeWindow_fontSize";
}
}
}//displayWindows

namespace settings {
namespace startup {
const QString icon = "startconfig.svg";
}
namespace fonts {
const QString icon = "fonts.svg";
}
namespace profiles {
const QString icon = "view_profile.svg";
}
namespace sword {
const QString icon = "swordconfig.svg";

}
namespace keys {
const QString icon = "key_bindings.svg";
}
}//settings

namespace mainIndex { // Bookshelf view
namespace search {
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
} //init_tr()
} //CResMgr
