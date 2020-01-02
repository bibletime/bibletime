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

#include "cresmgr.h"


namespace CResMgr {

namespace mainMenu { // Main menu

namespace view { // Main menu->View
namespace showMainIndex {
const QKeySequence accel(Qt::Key_F9);
const char* actionName    = "viewMainIndex_action";
}
namespace showInfoDisplay {
const QKeySequence accel(Qt::Key_F8);
const char* actionName    = "viewInfoDisplay_action";
}
} // namespace view {

namespace mainIndex {
namespace search {
const QKeySequence accel(Qt::CTRL + Qt::Key_O);
const char* actionName    = "mainindex_search_action";
}
namespace searchdefaultbible {
const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_F);
const char* actionName    = "mainindex_searchdefaultbible_action";
}
} // namespace mainIndex {

namespace window { //mainMenu::window
namespace loadProfile {
const char* actionName    = "windowLoadProfile_action";
}
namespace saveProfile {
const char* actionName    = "windowSaveProfile_action";
}
namespace saveToNewProfile {
const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_S);
const char* actionName    = "windowSaveToNewProfile_action";
}
namespace deleteProfile {
const char* actionName    = "windowDeleteProfile_action";
}
namespace showFullscreen {
const QKeySequence accel(Qt::Key_F5);
const char* actionName    = "windowFullscreen_action";
}
namespace arrangementMode {
const QKeySequence accel;
const char* actionName    = "windowArrangementMode_action";

namespace manual {
const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_M);
const char* actionName    = "windowArrangementManual_action";
}
namespace autoTileHorizontal {
const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_H);
const char* actionName    = "windowAutoTileHorizontal_action";
}
namespace autoTileVertical {
const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_G);
const char* actionName    = "windowAutoTileVertical_action";
}
namespace autoTile {
const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_I);
const char* actionName    = "windowAutoTile_action";
}
namespace autoTabbed {
const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_T);
const char* actionName    = "windowAutoTabbed_action";
}
namespace autoCascade {
const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_J);
const char* actionName    = "windowAutoCascade_action";
}
}
namespace tileHorizontal {
const QKeySequence accel(Qt::CTRL + Qt::Key_H);
const char* actionName    = "windowTileHorizontal_action";
}
namespace tileVertical {
const QKeySequence accel(Qt::CTRL + Qt::Key_G);
const char* actionName    = "windowTileVertical_action";
}
namespace tile {
const QKeySequence accel(Qt::CTRL + Qt::Key_I);
const char* actionName    = "windowTile_action";
}
namespace cascade {
const QKeySequence accel(Qt::CTRL + Qt::Key_J);
const char* actionName    = "windowCascade_action";
}
namespace close {
const QKeySequence accel(Qt::CTRL + Qt::Key_W);
const char* actionName    = "windowClose_action";
}
namespace closeAll {
const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_W);
const char* actionName    = "windowCloseAll_action";
}
}//mainMenu::window

namespace settings { //Main menu->Settings
namespace swordSetupDialog {
const QKeySequence accel(Qt::Key_F4);
const char* actionName    = "options_sword_setup";
}

}//mainMenu::settings

namespace help { //Main menu->Help
namespace handbook {
const QKeySequence accel(Qt::Key_F1);
const char* actionName    = "helpHandbook_action";
}
namespace bibleStudyHowTo {
const QKeySequence accel(Qt::Key_F2);
const char* actionName    = "helpHowTo_action";
}
namespace tipOfTheDay {
const QKeySequence accel(Qt::Key_F3);
const char* actionName    = "tipOfTheDay_action";
}
}//mainMenu::help
}  //end of mainMenu

namespace displaywindows {
namespace general {
namespace search {
const QKeySequence accel(Qt::CTRL + Qt::Key_N);
const char* actionName    = "window_search_action";
}

namespace backInHistory {
const QKeySequence accel(Qt::ALT + Qt::Key_Left);
const char* actionName    = "window_history_back_action";
}
namespace forwardInHistory {
const QKeySequence accel(Qt::ALT + Qt::Key_Right);
const char* actionName    = "window_history_forward_action";
}
namespace findStrongs {
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
}
namespace commentaryWindow {
namespace syncWindow {
const QKeySequence accel(Qt::SHIFT + Qt::Key_S);
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
}

namespace writeWindow {
namespace saveText {
const QKeySequence accel;
const char* actionName  = "writeWindow_saveText";
}
namespace restoreText {
const QKeySequence accel;
const char* actionName  = "writeWindow_restoreText";
}
namespace deleteEntry {
const QKeySequence accel;
const char* actionName  = "writeWindow_deleteEntry";
}

//formatting buttons
namespace boldText {
const QKeySequence accel;
const char* actionName  = "writeWindow_boldText";
}
namespace italicText {
const QKeySequence accel;
const char* actionName  = "writeWindow_italicText";
}
namespace underlinedText {
const QKeySequence accel;
const char* actionName  = "writeWindow_underlineText";
}

namespace alignLeft {
const QKeySequence accel;
const char* actionName  = "writeWindow_alignLeft";
}
namespace alignCenter {
const QKeySequence accel;
const char* actionName  = "writeWindow_alignCenter";
}
namespace alignRight {
const QKeySequence accel;
const char* actionName  = "writeWindow_alignRight";
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

namespace mainIndex { // Bookshelf view
namespace search {
const QKeySequence accel(Qt::CTRL + Qt::ALT + Qt::Key_M);
const char* actionName    = "GMsearch_action";
}
}//mainIndex

} // namespace CResMgr
