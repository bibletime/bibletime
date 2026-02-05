/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2026 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "cresmgr.h"

#include <Qt>


namespace CResMgr {

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

//formatting buttons
namespace boldText {
const QKeySequence accel;
}
namespace italicText {
const QKeySequence accel;
}
namespace underlinedText {
const QKeySequence accel;
}

namespace alignLeft {
const QKeySequence accel;
}
namespace alignCenter {
const QKeySequence accel;
}
namespace alignRight {
const QKeySequence accel;
}
namespace fontFamily {
const QKeySequence accel;
}
namespace fontSize {
const QKeySequence accel;
}
}
}//displayWindows

namespace mainIndex { // Bookshelf view
namespace search {
const QKeySequence accel(Qt::CTRL | Qt::ALT | Qt::Key_M);
}
}//mainIndex

} // namespace CResMgr
