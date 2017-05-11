/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "chooserinterface.h"

#include "backend/managers/cswordbackend.h"
#include "backend/keys/cswordversekey.h"
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/drivers/cswordmoduleinfo.h"

namespace btm {


ChooserInterface::ChooserInterface() : QObject() {
}

QStringList ChooserInterface::getBooks(const QString& moduleName) const {
    QStringList books;
    CSwordModuleInfo* module = CSwordBackend::instance()->findModuleByName(moduleName);
    const CSwordBibleModuleInfo* bibleModule = qobject_cast<const CSwordBibleModuleInfo*>(module);
    if (bibleModule)
        books = *bibleModule->books();
    return books;
}

QStringList ChooserInterface::getChapters(const QString& moduleName, const QString& book) const {
    QStringList chapters;
    CSwordModuleInfo* m = CSwordBackend::instance()->findModuleByName(moduleName);
    const CSwordBibleModuleInfo* module = qobject_cast<const CSwordBibleModuleInfo*>(m);
    if (module) {
        CSwordKey * key = CSwordKey::createInstance(m);
        CSwordVerseKey* verseKey = dynamic_cast<CSwordVerseKey*>(key);
        if (verseKey) {
            int count = module->chapterCount(book);
            for (int i = 1; i <= count; i++) {
                chapters << QString::number(i);
            }
        }
    }
    return chapters;
}

QStringList ChooserInterface::getVerses(const QString& moduleName, const QString& book, const QString& strChapter) const {
    QStringList verses;
    CSwordModuleInfo* m = CSwordBackend::instance()->findModuleByName(moduleName);
    const CSwordBibleModuleInfo* module = qobject_cast<const CSwordBibleModuleInfo*>(m);
    if (module) {
        int chapter = strChapter.toInt();
        int count = module->verseCount(book,chapter);
        for (int i = 1; i <= count; i++) {
            verses << QString::number(i);
        }
    }
    return verses;
}

}

