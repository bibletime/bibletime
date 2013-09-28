/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2013 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "bibletextmodel.h"

#include "btglobal.h"
#include "backend/keys/cswordversekey.h"
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/managers/cswordbackend.h"

namespace btm {

struct BookEntry {
    enum TextRoles {
        ReferenceRole = Qt::UserRole + 1,
        TextRole = Qt::UserRole + 2
    };
};

BibleTextModelBuilder::BibleTextModelBuilder(RoleItemModel* model)
    : m_model(model) {
    QHash<int, QByteArray> roleNames;
    roleNames[BookEntry::ReferenceRole] =  "ref";
    roleNames[BookEntry::TextRole] = "line";
    m_model->setRoleNames(roleNames);
}

void BibleTextModelBuilder::updateModel(
        QList<const CSwordModuleInfo*> modules,
        const QString& keyName) {

    Q_ASSERT( modules.count() >= 1 );
    Q_ASSERT( !keyName.isEmpty() );

    FilterOptions options;
    options.footnotes = 0;
    options.strongNumbers = 0;
    options.headings = 1;
    options.morphTags = 0;
    options.lemmas = 0;
    options.hebrewPoints = 0;
    options.greekAccents = 0;
    options.textualVariants = 0;
    options.redLetterWords = 1;
    options.scriptureReferences = 0;
    options.morphSegmentation = 0;
    CSwordBackend::instance()->setFilterOptions(options);

    const CSwordModuleInfo *module = modules.first();

    if (modules.count() == 1)
        module->module()->setSkipConsecutiveLinks( true ); //skip empty, linked verses

    QString startKey = keyName;
    QString endKey = startKey;

    //check whether there's an intro we have to include
    Q_ASSERT((module->type() == CSwordModuleInfo::Bible));

    if (module->type() == CSwordModuleInfo::Bible) {
        ((sword::VerseKey*)(module->module()->getKey()))->Headings(1);

        Q_ASSERT(dynamic_cast<const CSwordBibleModuleInfo*>(module) != 0);
        const CSwordBibleModuleInfo* bible =
                static_cast<const CSwordBibleModuleInfo*>(module);

        CSwordVerseKey k1(module);
        k1.Headings(1);
        k1.setKey(keyName);

        if (k1.getChapter() == 1)
            k1.setChapter(0); // Chapter 1, start with 0:0, otherwise X:0

        k1.setVerse(0);

        startKey = k1.key();

        if (k1.getChapter() == 0)
            k1.setChapter(1);

        k1.setVerse(bible->verseCount(k1.book(), k1.getChapter()));
        endKey = k1.key();
    }

    updateKeyRange(modules, startKey, endKey, keyName);
}

void BibleTextModelBuilder::updateKeyRange(
        QList<const CSwordModuleInfo*> modules,
        const QString& startKey,
        const QString& endKey,
        const QString& highlightKey) {

    m_model->clear();

    const CSwordModuleInfo *module = modules.first();

    QSharedPointer<CSwordKey> currentKey( CSwordKey::createInstance(module) );
    currentKey->setKey(highlightKey);
    CSwordVerseKey* vk_current = dynamic_cast<CSwordVerseKey*>(currentKey.data());
    Q_ASSERT(vk_current);
    m_currentVerse = vk_current->getVerse();

    QSharedPointer<CSwordKey> lowerBound( CSwordKey::createInstance(module) );
    lowerBound->setKey(startKey);

    QSharedPointer<CSwordKey> upperBound( CSwordKey::createInstance(module) );
    upperBound->setKey(endKey);

    sword::SWKey* sw_start = dynamic_cast<sword::SWKey*>(lowerBound.data());
    sword::SWKey* sw_stop = dynamic_cast<sword::SWKey*>(upperBound.data());

    Q_ASSERT((*sw_start == *sw_stop) || (*sw_start < *sw_stop));

    //    if (*sw_start == *sw_stop) { //same key, render single key
    //        return renderSingleKey(lowerBound->key(), modules);
    //    }
    //    else if (*sw_start < *sw_stop) { // Render range
    //        KeyTree tree;
    //        KeyTreeItem::Settings settings = keySettings;

    CSwordVerseKey* vk_start = dynamic_cast<CSwordVerseKey*>(lowerBound.data());
    Q_ASSERT(vk_start);

    CSwordVerseKey* vk_stop = dynamic_cast<CSwordVerseKey*>(upperBound.data());
    Q_ASSERT(vk_stop);

    while ((*vk_start < *vk_stop) || (*vk_start == *vk_stop)) {

        //make sure the key given by highlightKey gets marked as current key
        //            settings.highlight = (!highlightKey.isEmpty() ? (vk_start->key() == highlightKey) : false);

        /**
                \todo We need to take care of linked verses if we render one or
                      (esp) more modules. If the verses 2,3,4,5 are linked to 1,
                      it should be displayed as one entry with the caption 1-5.
            */

        if (vk_start->getChapter() == 0) { // range was 0:0-1:x, render 0:0 first and jump to 1:0
            vk_start->setVerse(0);
            addVerseToModel(vk_start);
            vk_start->setChapter(1);
            vk_start->setVerse(0);
        }
        addVerseToModel(vk_start);
        if (!vk_start->next(CSwordVerseKey::UseVerse)) {
            /// \todo Notify the user about this failure.
            break;
        }
    }
}

void BibleTextModelBuilder::addVerseToModel(CSwordVerseKey* key) {
    int verseNumber = key->getVerse();
    QString verse;
    if (verseNumber != 0)
        verse.setNum(verseNumber);
    QString text = key->renderedText();
    if (text.isEmpty())
        return;
    text.replace("<br /><br />", "<br />");
    QStandardItem* item = new QStandardItem();
    item->setData(verse, BookEntry::ReferenceRole);
    item->setData(text, BookEntry::TextRole);
    m_model->appendRow(item);

    if (m_currentVerse == verseNumber)
        m_currentModelIndex = m_model->rowCount() - 1;
}

int BibleTextModelBuilder::getCurrentModelIndex() const {
    return m_currentModelIndex;
}

}
