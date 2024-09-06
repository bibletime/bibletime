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

#pragma once

#include <QKeySequence>
#include "bticons.h"


class QIcon;

#define BT_GETICON2(fname,name) \
    inline QIcon const & fname() { return BtIcons::instance().icon_ ## name; }
#define BT_GETICON(name) BT_GETICON2(icon,name)

namespace CResMgr {

BT_GETICON2(icon_clearEdit, edit_clear_locationbar)

namespace mainWindow {
BT_GETICON(bibletime)
BT_GETICON2(icon_openAction, folder_open)
}
namespace modules {

BT_GETICON2(icon_moduleLanguage, flag)

BT_GETICON2(icon_cult, stop)

namespace bible {
BT_GETICON2(icon_unlocked, bible)
BT_GETICON2(icon_locked, bible_locked)
BT_GETICON2(icon_add, bible_add)
} /* namespace bible { */

namespace commentary {
BT_GETICON2(icon_unlocked, commentary)
BT_GETICON2(icon_locked, commentary_locked)
BT_GETICON2(icon_add, commentary_add)
} /* namespace commentary { */

namespace lexicon {
BT_GETICON2(icon_unlocked, lexicon)
BT_GETICON2(icon_locked, lexicon_locked)
BT_GETICON2(icon_add, lexicon_add)
} /* namespace lexicon { */

namespace book {
BT_GETICON2(icon_unlocked, book)
BT_GETICON2(icon_locked, book_locked)
BT_GETICON2(icon_add, book_add)
} /* namespace book { */
} /* namespace modules { */

namespace categories {
namespace bibles { BT_GETICON(bible) }
namespace commentaries { BT_GETICON(commentary) }
namespace lexicons { BT_GETICON(lexicon) }
namespace dailydevotional { BT_GETICON(calendar) }
namespace books { BT_GETICON(books) }
namespace glossary { BT_GETICON(dictionary) }
namespace images { BT_GETICON(map) }
namespace cults { BT_GETICON(questionable) }
} /* namespace categories { */

namespace mainMenu { // Main menu

namespace view { // Main menu->View
namespace showBookshelf { BT_GETICON(books) }
namespace showBookmarks { BT_GETICON(bookmark) }
namespace showMag { BT_GETICON(document_magnifier) }
namespace showMainIndex {
BT_GETICON(view_index)
extern const QKeySequence accel;
extern QString const actionName;
} /* namespace showMainIndex { */
namespace showInfoDisplay {
BT_GETICON(view_mag)
extern const QKeySequence accel;
extern QString const actionName;
} /* namespace showInfoDisplay { */
} /* namespace view { */

namespace mainIndex { //configuration for the main index and the view->search menu
namespace search {
BT_GETICON(find)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace searchdefaultbible {
BT_GETICON(find)
extern const QKeySequence accel;
extern QString const actionName;
}
}

namespace window { //Main menu->Window
namespace quit { BT_GETICON(exit) }
namespace loadProfile {
BT_GETICON(view_profile)
extern QString const actionName;
}
namespace saveProfile {
BT_GETICON(view_profile)
extern QString const actionName;
}
namespace saveToNewProfile {
BT_GETICON(view_profile)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace deleteProfile {
BT_GETICON(view_profile)
extern QString const actionName;
}
namespace showFullscreen {
BT_GETICON(window_fullscreen)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace arrangementMode {
BT_GETICON(cascade_auto)
extern const QKeySequence accel;
extern QString const actionName;

namespace manual {
BT_GETICON(manual)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace autoTileHorizontal {
BT_GETICON(tile_horiz_auto)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace autoTileVertical {
BT_GETICON(tile_vert_auto)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace autoTile {
BT_GETICON(tile_auto)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace autoTabbed {
BT_GETICON(tabbed)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace autoCascade {
BT_GETICON(cascade_auto)
extern const QKeySequence accel;
extern QString const actionName;
}
}
namespace tileHorizontal {
BT_GETICON(tile_horiz)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace tileVertical {
BT_GETICON(tile_vert)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace tile {
BT_GETICON(tile)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace cascade {
BT_GETICON(cascade)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace close {
BT_GETICON(fileclose)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace closeAll {
BT_GETICON(fileclose)
extern const QKeySequence accel;
extern QString const actionName;
}
}

namespace settings { //Main menu->Settings
namespace configureDialog { BT_GETICON(configure) }
namespace swordSetupDialog {
BT_GETICON(swordconfig)
extern const QKeySequence accel;
extern QString const actionName;
}
}

namespace help { //Main menu->Help
namespace handbook {
BT_GETICON(contents2)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace bibleStudyHowTo {
BT_GETICON(contents2)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace tipOfTheDay {
BT_GETICON(light_bulb)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace aboutBibleTime { BT_GETICON(bibletime) }
}
}  //end of main menu

namespace findWidget {
BT_GETICON2(icon_close, stop)
BT_GETICON2(icon_previous, back)
BT_GETICON2(icon_next, forward)
}

namespace searchdialog {
BT_GETICON(find)
BT_GETICON2(icon_close, stop)
BT_GETICON2(icon_help, questionmark)
BT_GETICON2(icon_chooseWorks, checkbox)
BT_GETICON2(icon_setupScope, configure)

namespace result {
namespace moduleList {
namespace copyMenu { BT_GETICON(edit_copy) }
namespace saveMenu { BT_GETICON(file_save) }
namespace printMenu { BT_GETICON(print) }
}
namespace foundItems {
namespace copyMenu { BT_GETICON(edit_copy) }
namespace saveMenu { BT_GETICON(file_save) }
namespace printMenu { BT_GETICON(print) }
}
}
}

namespace displaywindows {
namespace displaySettings {
BT_GETICON(displayconfig)
}

namespace general {
BT_GETICON2(icon_removeModule, fileclose)
BT_GETICON2(icon_addModule, plus)
BT_GETICON2(icon_replaceModule, checkbox)
namespace search {
BT_GETICON(find)
extern const QKeySequence accel;
extern QString const actionName;
}

namespace backInHistory {
BT_GETICON(back)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace forwardInHistory {
BT_GETICON(forward)
extern const QKeySequence accel;
extern QString const actionName;
}

namespace findStrongs {
extern const QKeySequence accel;
extern QString const actionName;
}
}

namespace bibleWindow {
namespace nextBook {
extern const QKeySequence accel;
}
namespace previousBook {
extern const QKeySequence accel;
}

namespace nextChapter {
extern const QKeySequence accel;
}
namespace previousChapter {
extern const QKeySequence accel;
}

namespace nextVerse {
extern const QKeySequence accel;
}
namespace previousVerse {
extern const QKeySequence accel;
}

namespace copyMenu { BT_GETICON(edit_copy) }
namespace saveMenu { BT_GETICON(file_save) }
namespace printMenu { BT_GETICON(print) }
}
namespace commentaryWindow {
namespace syncWindow {
BT_GETICON(sync)
extern const QKeySequence accel;
extern QString const actionName;
}

}

namespace lexiconWindow {
namespace nextEntry {
extern const QKeySequence accel;
}
namespace previousEntry {
extern const QKeySequence accel;
}
}

namespace writeWindow {
namespace saveText {
BT_GETICON(file_save)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace restoreText {
BT_GETICON(import)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace deleteEntry {
BT_GETICON(delete)
extern const QKeySequence accel;
extern QString const actionName;
}

//formatting buttons
namespace boldText {
BT_GETICON(text_bold)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace italicText {
BT_GETICON(text_italic)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace underlinedText {
BT_GETICON(text_under)
extern const QKeySequence accel;
extern QString const actionName;
}

namespace alignLeft {
BT_GETICON(text_leftalign)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace alignCenter {
BT_GETICON(text_center)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace alignRight {
BT_GETICON(text_rightalign)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace fontFamily {
extern const QKeySequence accel;
extern QString const actionName;
}
namespace fontSize {
extern const QKeySequence accel;
extern QString const actionName;
}
}
}

namespace settings {
namespace audio { BT_GETICON(audio) }
namespace startup { BT_GETICON(startconfig) }
namespace fonts { BT_GETICON(fonts) }
namespace profiles { BT_GETICON(view_profile) }
namespace sword { BT_GETICON(swordconfig) }
namespace keys { BT_GETICON(key_bindings) }
}

namespace mainIndex { //configuration for the main index and the view->search menu
namespace showHide { BT_GETICON(layer_visible_on) }
namespace search {
BT_GETICON(find)
extern const QKeySequence accel;
extern QString const actionName;
}
namespace newFolder { BT_GETICON(folder_add) }
namespace changeFolder { BT_GETICON(folder) }
namespace openedFolder { BT_GETICON(folder_open) }
namespace closedFolder { BT_GETICON(folder) }

namespace bookmark { BT_GETICON(bookmark) }
namespace editBookmark { BT_GETICON(bookmark) }
namespace sortFolderBookmarks { BT_GETICON(null) }
namespace sortAllBookmarks { BT_GETICON(null) }
namespace importBookmarks { BT_GETICON(import) }
namespace exportBookmarks { BT_GETICON(export) }
namespace printBookmarks { BT_GETICON(print) }
namespace deleteItems { BT_GETICON(delete) }

namespace unlockModule { BT_GETICON(unlock) }
namespace aboutModule { BT_GETICON(info) }
namespace grouping { BT_GETICON(view_tree) }
}

namespace bookshelfmgr {
namespace installpage {
BT_GETICON(bible_install)
BT_GETICON2(icon_refresh, refresh)
BT_GETICON2(icon_addSource, plus)
BT_GETICON2(icon_deleteSource, delete)
BT_GETICON2(icon_install, plus)
BT_GETICON2(icon_path, configure)
}
namespace removepage {
BT_GETICON(bible_uninstall)
BT_GETICON2(icon_remove, delete)
}
namespace indexpage {
BT_GETICON(document_magnifier)
BT_GETICON2(icon_create, plus)
BT_GETICON2(icon_delete, delete)
}
namespace paths {
BT_GETICON2(icon_add, add)
BT_GETICON2(icon_edit, pencil)
BT_GETICON2(icon_remove, remove)
}
}
}

#undef BT_GETICON
#undef BT_GETICON2
