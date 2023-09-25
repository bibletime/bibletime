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

#include "cresmgr.h"

#include <Qt>


namespace CResMgr {

namespace mainMenu { // Main menu

namespace view { // Main menu->View
namespace showMainIndex {
const QKeySequence accel(Qt::Key_F9);
QString const actionName = QStringLiteral("viewMainIndex_action");
}
namespace showInfoDisplay {
const QKeySequence accel(Qt::Key_F8);
QString const actionName = QStringLiteral("viewInfoDisplay_action");
}
} // namespace view {

namespace mainIndex {
namespace search {
const QKeySequence accel(Qt::CTRL | Qt::Key_O);
QString const actionName = QStringLiteral("mainindex_search_action");
}
namespace searchdefaultbible {
const QKeySequence accel(Qt::CTRL | Qt::ALT | Qt::Key_F);
QString const actionName = QStringLiteral("mainindex_searchdefaultbible_action");
}
} // namespace mainIndex {

namespace window { //mainMenu::window
namespace loadProfile {
QString const actionName = QStringLiteral("windowLoadProfile_action");
}
namespace saveProfile {
QString const actionName = QStringLiteral("windowSaveProfile_action");
}
namespace saveToNewProfile {
const QKeySequence accel(Qt::CTRL | Qt::ALT | Qt::Key_S);
QString const actionName = QStringLiteral("windowSaveToNewProfile_action");
}
namespace deleteProfile {
QString const actionName = QStringLiteral("windowDeleteProfile_action");
}
namespace showFullscreen {
const QKeySequence accel(Qt::Key_F5);
QString const actionName = QStringLiteral("windowFullscreen_action");
}
namespace arrangementMode {
const QKeySequence accel;
QString const actionName = QStringLiteral("windowArrangementMode_action");

namespace manual {
const QKeySequence accel(Qt::CTRL | Qt::ALT | Qt::Key_M);
QString const actionName = QStringLiteral("windowArrangementManual_action");
}
namespace autoTileHorizontal {
const QKeySequence accel(Qt::CTRL | Qt::ALT | Qt::Key_H);
QString const actionName = QStringLiteral("windowAutoTileHorizontal_action");
}
namespace autoTileVertical {
const QKeySequence accel(Qt::CTRL | Qt::ALT | Qt::Key_G);
QString const actionName = QStringLiteral("windowAutoTileVertical_action");
}
namespace autoTile {
const QKeySequence accel(Qt::CTRL | Qt::ALT | Qt::Key_I);
QString const actionName = QStringLiteral("windowAutoTile_action");
}
namespace autoTabbed {
const QKeySequence accel(Qt::CTRL | Qt::ALT | Qt::Key_T);
QString const actionName = QStringLiteral("windowAutoTabbed_action");
}
namespace autoCascade {
const QKeySequence accel(Qt::CTRL | Qt::ALT | Qt::Key_J);
QString const actionName = QStringLiteral("windowAutoCascade_action");
}
}
namespace tileHorizontal {
const QKeySequence accel(Qt::CTRL | Qt::Key_H);
QString const actionName = QStringLiteral("windowTileHorizontal_action");
}
namespace tileVertical {
const QKeySequence accel(Qt::CTRL | Qt::Key_G);
QString const actionName = QStringLiteral("windowTileVertical_action");
}
namespace tile {
const QKeySequence accel(Qt::CTRL | Qt::Key_I);
QString const actionName = QStringLiteral("windowTile_action");
}
namespace cascade {
const QKeySequence accel(Qt::CTRL | Qt::Key_J);
QString const actionName = QStringLiteral("windowCascade_action");
}
namespace close {
const QKeySequence accel(Qt::CTRL | Qt::Key_W);
QString const actionName = QStringLiteral("windowClose_action");
}
namespace closeAll {
const QKeySequence accel(Qt::CTRL | Qt::ALT | Qt::Key_W);
QString const actionName = QStringLiteral("windowCloseAll_action");
}
}//mainMenu::window

namespace settings { //Main menu->Settings
namespace swordSetupDialog {
const QKeySequence accel(Qt::Key_F4);
QString const actionName = QStringLiteral("options_sword_setup");
}

}//mainMenu::settings

namespace help { //Main menu->Help
namespace handbook {
const QKeySequence accel(Qt::Key_F1);
QString const actionName = QStringLiteral("helpHandbook_action");
}
namespace bibleStudyHowTo {
const QKeySequence accel(Qt::Key_F2);
QString const actionName = QStringLiteral("helpHowTo_action");
}
namespace tipOfTheDay {
const QKeySequence accel(Qt::Key_F3);
QString const actionName = QStringLiteral("tipOfTheDay_action");
}
}//mainMenu::help
}  //end of mainMenu

namespace displaywindows {
namespace general {
namespace search {
const QKeySequence accel(Qt::CTRL | Qt::Key_N);
QString const actionName = QStringLiteral("window_search_action");
}

namespace backInHistory {
const QKeySequence accel(Qt::ALT | Qt::Key_Left);
QString const actionName = QStringLiteral("window_history_back_action");
}
namespace forwardInHistory {
const QKeySequence accel(Qt::ALT | Qt::Key_Right);
QString const actionName = QStringLiteral("window_history_forward_action");
}
namespace findStrongs {
const QKeySequence accel;
QString const actionName = QStringLiteral("window_find_strongs_action");
}

}
namespace bibleWindow {
namespace nextBook {
const QKeySequence accel(Qt::CTRL | Qt::Key_Y);
}
namespace previousBook {
const QKeySequence accel(Qt::CTRL | Qt::SHIFT | Qt::Key_Y);
}

namespace nextChapter {
const QKeySequence accel(Qt::CTRL | Qt::Key_X);
}
namespace previousChapter {
const QKeySequence accel(Qt::CTRL | Qt::SHIFT | Qt::Key_X);
}
namespace nextVerse {
const QKeySequence accel(Qt::CTRL | Qt::Key_V);
}
namespace previousVerse {
const QKeySequence accel(Qt::CTRL | Qt::SHIFT | Qt::Key_V);
}
}
namespace commentaryWindow {
namespace syncWindow {
const QKeySequence accel(Qt::SHIFT | Qt::Key_S);
QString const actionName = QStringLiteral("commentary_syncWindow");
}
}
namespace lexiconWindow {
namespace nextEntry {
const QKeySequence accel(Qt::CTRL | Qt::Key_V);
}
namespace previousEntry {
const QKeySequence accel(Qt::CTRL | Qt::SHIFT | Qt::Key_V);
}
}

namespace writeWindow {
namespace saveText {
const QKeySequence accel;
QString const actionName = QStringLiteral("writeWindow_saveText");
}
namespace restoreText {
const QKeySequence accel;
QString const actionName = QStringLiteral("writeWindow_restoreText");
}
namespace deleteEntry {
const QKeySequence accel;
QString const actionName = QStringLiteral("writeWindow_deleteEntry");
}

//formatting buttons
namespace boldText {
const QKeySequence accel;
QString const actionName = QStringLiteral("writeWindow_boldText");
}
namespace italicText {
const QKeySequence accel;
QString const actionName = QStringLiteral("writeWindow_italicText");
}
namespace underlinedText {
const QKeySequence accel;
QString const actionName = QStringLiteral("writeWindow_underlineText");
}

namespace alignLeft {
const QKeySequence accel;
QString const actionName = QStringLiteral("writeWindow_alignLeft");
}
namespace alignCenter {
const QKeySequence accel;
QString const actionName = QStringLiteral("writeWindow_alignCenter");
}
namespace alignRight {
const QKeySequence accel;
QString const actionName = QStringLiteral("writeWindow_alignRight");
}
namespace fontFamily {
const QKeySequence accel;
QString const actionName = QStringLiteral("writeWindow_fontFamily");
}
namespace fontSize {
const QKeySequence accel;
QString const actionName = QStringLiteral("writeWindow_fontSize");
}
}
}//displayWindows

namespace mainIndex { // Bookshelf view
namespace search {
const QKeySequence accel(Qt::CTRL | Qt::ALT | Qt::Key_M);
QString const actionName = QStringLiteral("GMsearch_action");
}
}//mainIndex

} // namespace CResMgr
