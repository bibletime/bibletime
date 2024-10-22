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
namespace showFullscreen {
QString const actionName = QStringLiteral("windowFullscreen_action");
}
namespace showMainIndex {
QString const actionName = QStringLiteral("viewMainIndex_action");
}
namespace showInfoDisplay {
QString const actionName = QStringLiteral("viewInfoDisplay_action");
}
} // namespace view {

namespace mainIndex {
namespace search {
QString const actionName = QStringLiteral("mainindex_search_action");
}
namespace searchdefaultbible {
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
QString const actionName = QStringLiteral("windowSaveToNewProfile_action");
}
namespace deleteProfile {
QString const actionName = QStringLiteral("windowDeleteProfile_action");
}
namespace arrangementMode {
QString const actionName = QStringLiteral("windowArrangementMode_action");

namespace manual {
QString const actionName = QStringLiteral("windowArrangementManual_action");
}
namespace autoTileHorizontal {
QString const actionName = QStringLiteral("windowAutoTileHorizontal_action");
}
namespace autoTileVertical {
QString const actionName = QStringLiteral("windowAutoTileVertical_action");
}
namespace autoTile {
QString const actionName = QStringLiteral("windowAutoTile_action");
}
namespace autoTabbed {
QString const actionName = QStringLiteral("windowAutoTabbed_action");
}
namespace autoCascade {
QString const actionName = QStringLiteral("windowAutoCascade_action");
}
}
namespace tileHorizontal {
QString const actionName = QStringLiteral("windowTileHorizontal_action");
}
namespace tileVertical {
QString const actionName = QStringLiteral("windowTileVertical_action");
}
namespace tile {
QString const actionName = QStringLiteral("windowTile_action");
}
namespace cascade {
QString const actionName = QStringLiteral("windowCascade_action");
}
namespace close {
QString const actionName = QStringLiteral("windowClose_action");
}
namespace closeAll {
QString const actionName = QStringLiteral("windowCloseAll_action");
}
}//mainMenu::window

namespace settings { //Main menu->Settings
namespace swordSetupDialog {
QString const actionName = QStringLiteral("options_sword_setup");
}

}//mainMenu::settings

namespace help { //Main menu->Help
namespace handbook {
QString const actionName = QStringLiteral("helpHandbook_action");
}
namespace bibleStudyHowTo {
QString const actionName = QStringLiteral("helpHowTo_action");
}
namespace tipOfTheDay {
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
