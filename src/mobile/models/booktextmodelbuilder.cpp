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

#include "booktextmodelbuilder.h"

#include "btglobal.h"
#include "backend/keys/cswordtreekey.h"
#include "backend/drivers/cswordbookmoduleinfo.h"
#include "backend/managers/cswordbackend.h"

namespace btm {

struct BookEntry {
    enum TextRoles {
        ReferenceRole = Qt::UserRole + 1,
        TextRole = Qt::UserRole + 2
    };
};

BookTextModelBuilder::BookTextModelBuilder(RoleItemModel* model)
    : m_model(model) {
    QHash<int, QByteArray> roleNames;
    roleNames[BookEntry::ReferenceRole] =  "ref";
    roleNames[BookEntry::TextRole] = "line";
    m_model->setRoleNames(roleNames);
}

void BookTextModelBuilder::updateModel(
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

    const CSwordBookModuleInfo* book = dynamic_cast<const CSwordBookModuleInfo*>(module);
    Q_ASSERT(book);

    QSharedPointer<CSwordTreeKey> key (
        dynamic_cast<CSwordTreeKey*>( CSwordKey::createInstance(book) )
    );
    key->setKey(keyName); //set the key to position we'd like to get

    const unsigned long offset = key->getOffset();

    //check whether there's an intro we have to include
    Q_ASSERT((module->type() == CSwordModuleInfo::GenericBook));

    if (module->type() == CSwordModuleInfo::GenericBook) {
        CSwordTreeKey* swKey = dynamic_cast<CSwordTreeKey*>(key.data());
        addVerseToModel(swKey);
    }
}

void BookTextModelBuilder::addVerseToModel(CSwordTreeKey* key) {
//    int verseNumber = key->getVerse();
    QString verse;
//    if (verseNumber != 0)
//        verse.setNum(verseNumber);
    QString text = key->renderedText(); // TODO - fix
    if (text.isEmpty())
        return;
//    text.replace("<br /><br />", "<br />");
    QStandardItem* item = new QStandardItem();
    item->setData(verse, BookEntry::ReferenceRole);
    item->setData(text, BookEntry::TextRole);
    m_model->appendRow(item);

//    if (m_currentVerse == verseNumber)
//        m_currentModelIndex = m_model->rowCount() - 1;
}

int BookTextModelBuilder::getCurrentModelIndex() const {
    return m_currentModelIndex;
}

}
