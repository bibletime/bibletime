/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CRESMGR_H
#define CRESMGR_H

#include <QKeySequence>
#include <QString>

namespace CResMgr {

namespace mainWindow {
extern const QString icon;
}
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
}
namespace lexicon {
extern const QString icon_unlocked;
extern const QString icon_locked;
extern const QString icon_add;
}
namespace book {
extern const QString icon_unlocked;
extern const QString icon_locked;
extern const QString icon_add;
}
}

namespace categories {
namespace bibles {
extern const QString icon;
}
namespace commentaries {
extern const QString icon;
}
namespace lexicons {
extern const QString icon;
}
namespace dailydevotional {
extern const QString icon;
}
namespace books {
extern const QString icon;
}
namespace glossary {
extern const QString icon;
}
namespace images {
extern const QString icon;
}
namespace cults {
extern const QString icon;
}
}

namespace mainMenu { //Main menu

namespace view { //Main menu->View
namespace showBookshelf {
extern const QString icon;
}
namespace showBookmarks {
extern const QString icon;
}
namespace showMag {
extern const QString icon;
}
namespace showMainIndex {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace showInfoDisplay {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
}

namespace mainIndex { //configuration for the main index and the view->search menu
namespace search {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace searchdefaultbible {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
}

namespace window { //Main menu->Window
namespace loadProfile {
extern const QString icon;
extern const char* actionName;
}
namespace saveProfile {
extern const QString icon;
extern const char* actionName;
}
namespace saveToNewProfile {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace deleteProfile {
extern const QString icon;
extern const char* actionName;
}
namespace showFullscreen {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace arrangementMode {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;

namespace manual {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace autoTileVertical {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace autoTileHorizontal {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace autoTile {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace autoTabbed {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace autoCascade {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
}
namespace tileVertical {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace tileHorizontal {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace tile {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace cascade {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace close {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace closeAll {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
}

namespace settings { //Main menu->Settings
namespace editToolBar { // available as KStdAction
}
namespace optionsDialog { // available as KStdAction
}
namespace swordSetupDialog {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
}

namespace help { //Main menu->Help
namespace handbook {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace bibleStudyHowTo {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace tipOfTheDay {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
}
}  //end of main menu

namespace searchdialog {
extern const QString icon;
extern const QString close_icon;
extern const QString help_icon;
extern const QString chooseworks_icon;
extern const QString setupscope_icon;

namespace result {
namespace moduleList {

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
extern const QString removemoduleicon;
extern const QString addmoduleicon;
extern const QString replacemoduleicon;
namespace search {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}

namespace backInHistory {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace forwardInHistory {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}

namespace findStrongs {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
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
extern const QKeySequence accel;
extern const char* actionName;
}

}

namespace lexiconWindow {
namespace nextEntry {
extern const QKeySequence accel;
}
namespace previousEntry {
extern const QKeySequence accel;
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
extern const QKeySequence accel;
}
}


namespace writeWindow {
namespace saveText {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace restoreText {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace deleteEntry {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}

//formatting buttons
namespace boldText {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace italicText {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace underlinedText {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}

namespace alignLeft {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace alignCenter {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace alignRight {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}
namespace alignJustify {
extern const QString icon;
extern const QKeySequence accel;
extern const char* actionName;
}

namespace fontFamily {
extern const QKeySequence accel;
extern const char* actionName;
}
namespace fontSize {
extern const QKeySequence accel;
extern const char* actionName;
}
namespace fontColor {
}

}
}

namespace settings {
namespace startup {
extern const QString icon;
}
namespace fonts {
extern const QString icon;
}
namespace languages {
extern const QString icon;
}
namespace profiles {
extern const QString icon;
}
namespace sword {
extern const QString icon;

}
namespace keys {
extern const QString icon;
}
}

namespace mainIndex { //configuration for the main index and the view->search menu
namespace search {
extern const QString icon;
extern const QKeySequence accel;
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
namespace editBookmark {
extern const QString icon;
}
namespace sortFolderBookmarks {
extern const QString icon;
}
namespace sortAllBookmarks {
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
namespace grouping {
extern const QString icon;
}
}

namespace bookshelfmgr {
namespace installpage {
extern const QString icon;
extern const QString refresh_icon;
extern const QString delete_icon;
extern const QString add_icon;
extern const QString install_icon;
extern const QString path_icon;
}
namespace removepage {
extern const QString icon;
extern const QString remove_icon;
}
namespace indexpage {
extern const QString icon;
extern const QString create_icon;
extern const QString delete_icon;
}
namespace paths {
extern const QString add_icon;
extern const QString edit_icon;
extern const QString remove_icon;
}
}
}

#endif
