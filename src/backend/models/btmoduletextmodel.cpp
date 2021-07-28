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
#include <QTextEdit>
#include "btmoduletextmodel.h"

#include "../../util/btassert.h"
#include "../drivers/cswordmoduleinfo.h"
#include "../drivers/cswordbiblemoduleinfo.h"
#include "../drivers/cswordbookmoduleinfo.h"
#include "../drivers/cswordlexiconmoduleinfo.h"
#include "../cswordmodulesearch.h"
#include "../keys/cswordtreekey.h"
#include "../keys/cswordversekey.h"
#include "../keys/cswordldkey.h"
#include "../managers/colormanager.h"
#include "../managers/cswordbackend.h"
#include "../rendering/ctextrendering.h"

BtModuleTextFilter::~BtModuleTextFilter() {}


BtModuleTextModel::BtModuleTextModel(QObject *parent)
    : QAbstractListModel(parent),
      m_firstEntry(0),
      m_maxEntries(0),
      m_textFilter(nullptr) {

    m_findState.enabled = false;
    QHash<int, QByteArray> roleNames;
    roleNames[ModuleEntry::ReferenceRole] =  "keyName";             // reference
    roleNames[ModuleEntry::TextRole] = "line";                      // not used

    roleNames[ModuleEntry::Text0Role] = "text0";                    // text in column 0
    roleNames[ModuleEntry::Text1Role] = "text1";                    // text in column 1
    roleNames[ModuleEntry::Text2Role] = "text2";                    // text in column 2
    roleNames[ModuleEntry::Text3Role] = "text3";                    // text in column 3
    roleNames[ModuleEntry::Text4Role] = "text4";                    // text in column 4
    roleNames[ModuleEntry::Text5Role] = "text5";                    // text in column 5
    roleNames[ModuleEntry::Text6Role] = "text6";                    // text in column 6
    roleNames[ModuleEntry::Text7Role] = "text7";                    // text in column 7
    roleNames[ModuleEntry::Text8Role] = "text8";                    // text in column 8
    roleNames[ModuleEntry::Text9Role] = "text9";                    // text in column 9

    roleNames[ModuleEntry::Title0Role] = "title0";                  // title in column 0
    roleNames[ModuleEntry::Title1Role] = "title1";                  // title in column 1
    roleNames[ModuleEntry::Title2Role] = "title2";                  // title in column 2
    roleNames[ModuleEntry::Title3Role] = "title3";                  // title in column 3
    roleNames[ModuleEntry::Title4Role] = "title4";                  // title in column 4
    roleNames[ModuleEntry::Title5Role] = "title5";                  // title in column 5
    roleNames[ModuleEntry::Title6Role] = "title6";                  // title in column 6
    roleNames[ModuleEntry::Title7Role] = "title7";                  // title in column 7
    roleNames[ModuleEntry::Title8Role] = "title8";                  // title in column 8
    roleNames[ModuleEntry::Title9Role] = "title9";                  // title in column 9

    setRoleNames(roleNames);
    m_displayOptions.verseNumbers = 1;
    m_displayOptions.lineBreaks = 1;
    m_filterOptions.footnotes = 0;
    m_filterOptions.greekAccents = 1;
    m_filterOptions.headings = 1;
    m_filterOptions.hebrewCantillation = 1;
    m_filterOptions.hebrewPoints = 1;
    m_filterOptions.lemmas = 0;
    m_filterOptions.morphSegmentation = 1;
    m_filterOptions.morphTags = 1;
    m_filterOptions.redLetterWords = 1;
    m_filterOptions.scriptureReferences = 0;
    m_filterOptions.strongNumbers = 0;
    m_filterOptions.textualVariants = 0;
}

void BtModuleTextModel::reloadModules() {

    m_moduleInfoList.clear();
    for (int i = 0; i < m_modules.count(); ++i) {
        QString moduleName = m_modules.at(i);
        CSwordModuleInfo* module = CSwordBackend::instance()->findModuleByName(moduleName);
        m_moduleInfoList.append(module);
    }

    beginResetModel();
    const CSwordModuleInfo* firstModule = m_moduleInfoList.at(0);
    if (isBible() || isCommentary()) {
        auto & m = firstModule->module();
        m.setPosition(sword::TOP);
        m_firstEntry = m.getIndex();
        m.setPosition(sword::BOTTOM);
        m_maxEntries = m.getIndex() - m_firstEntry + 1;
    } else if(isLexicon()) {
        m_maxEntries =
                static_cast<CSwordLexiconModuleInfo const *>(firstModule)
                ->entries().size();
    } else if(isBook()) {
        sword::TreeKeyIdx tk(
                    *static_cast<CSwordBookModuleInfo const *>(firstModule)
                    ->tree());
        tk.root();
        tk.firstChild();
        BT_ASSERT(tk.getOffset() == 4);
        tk.setPosition(sword::BOTTOM);
        m_maxEntries = tk.getOffset() / 4;
    }

    endResetModel();
}

void BtModuleTextModel::setModules(const QStringList& modules) {
    m_modules = modules;
    reloadModules();
}


QVariant BtModuleTextModel::data(const QModelIndex & index, int role) const {

    QString text;
    if (isBible() || isCommentary())
        text = verseData(index, role);
    else if (isBook())
        text = bookData(index, role);
    else if (isLexicon())
        text = lexiconData(index, role);
    else
        text = "invalid";

    if (m_textFilter) {
        text = m_textFilter->processText(text);
    }

    if ( ! m_highlightWords.isEmpty()) {
        QString t = CSwordModuleSearch::highlightSearchedText(text, m_highlightWords);
        if (m_findState.enabled && index.row() == m_findState.index)
            t = highlightFindPreviousNextField(t);
        return QVariant(t);
    }

    return QVariant(text);
}

QString BtModuleTextModel::lexiconData(const QModelIndex & index, int role) const {
    int row = index.row();

    const CSwordLexiconModuleInfo *lexiconModule = qobject_cast<const CSwordLexiconModuleInfo*>(m_moduleInfoList.at(0));
    BtConstModuleList moduleList;
    moduleList << lexiconModule;
    QString keyName = lexiconModule->entries()[row];

    if (role == ModuleEntry::TextRole ||
            role == ModuleEntry::Text0Role) {
        Rendering::CEntryDisplay entryDisplay;
        QString text = entryDisplay.text(moduleList, keyName,
                                         m_displayOptions, m_filterOptions);
        text.replace("#CHAPTERTITLE#", "");
        text = ColorManager::instance().replaceColors(text);
        return text;
    }
    else if (role == ModuleEntry::ReferenceRole){
        return keyName;
    }
    return QString();
}

QString BtModuleTextModel::bookData(const QModelIndex & index, int role) const {
    if (role == ModuleEntry::TextRole ||
            role == ModuleEntry::Text0Role) {
        const CSwordBookModuleInfo *bookModule = qobject_cast<const CSwordBookModuleInfo*>(m_moduleInfoList.at(0));
        CSwordTreeKey key(bookModule->tree(), bookModule);
        int bookIndex = index.row() * 4;
        key.setIndex(bookIndex);
        Rendering::CEntryDisplay entryDisplay;
        BtConstModuleList moduleList;
        moduleList << bookModule;
        QString text = entryDisplay.textKeyRendering(moduleList, key.key(),
                                                     m_displayOptions, m_filterOptions,
                                                     m_displayOptions.verseNumbers ?
                                                     Rendering::CTextRendering::KeyTreeItem::Settings::SimpleKey :
                                                     Rendering::CTextRendering::KeyTreeItem::Settings::NoKey);
        text.replace("#CHAPTERTITLE#", "");
        return text;
    }
    return QString();
}

static int getColumnFromRole(int role) {
    if (role >= ModuleEntry::Text0Role && role <= ModuleEntry::Text9Role)
        return role - ModuleEntry::Text0Role;
    if (role >= ModuleEntry::Title0Role && role <= ModuleEntry::Title9Role)
        return role - ModuleEntry::Title0Role;
    return 0;
}

QString BtModuleTextModel::verseData(const QModelIndex & index, int role) const {
    int row = index.row();
    CSwordVerseKey key = indexToVerseKey(row);
    int verse = key.getVerse();

    if (role >= ModuleEntry::TextRole && role <= ModuleEntry::Title9Role) {
        if (verse == 0)
            return QString();
        QString text;

        QString chapterTitle;
        if (verse == 1)
            chapterTitle = key.book() + " " + QString::number(key.getChapter());

        BtConstModuleList modules;
        if ( role == ModuleEntry::TextRole) {
            modules = m_moduleInfoList;
        } else {
            int column = getColumnFromRole(role);
            CSwordModuleInfo const * module;
            if ((column + 1) > m_moduleInfoList.count())
                return "";
            else
                module = m_moduleInfoList.at(column);
            modules.append(module);
            CSwordVerseKey mKey(module);
            mKey.setKey(key.key());

            // Title only for verse 1 of Personal commentary
            if (role >= ModuleEntry::Title0Role && role <= ModuleEntry::Title9Role) {
                if (module->isWritable() && verse == 1)
                    return "<center><h3>" + chapterTitle + "</h3></center>";
                return "";
            }

            // Personal commentary
            if (module->isWritable()) {
                QString text;
                QString rawText = mKey.rawText();
                if (rawText.isEmpty())
                    rawText = "<span style=\"color:gray\"><small>" + tr("Click to edit") + "</small></span>";
                text += QString::number(verse) + "  " + rawText;

                text = ColorManager::instance().replaceColors(text);
                return CSwordModuleSearch::highlightSearchedText(text, m_highlightWords);
            }
        }

        text += Rendering::CEntryDisplay().textKeyRendering(modules,
                                                            key.key(), m_displayOptions, m_filterOptions,
                                                            m_displayOptions.verseNumbers ?
                                                            Rendering::CTextRendering::KeyTreeItem::Settings::SimpleKey :
                                                            Rendering::CTextRendering::KeyTreeItem::Settings::NoKey);

        text.replace("#CHAPTERTITLE#", chapterTitle);
        text = ColorManager::instance().replaceColors(text);
        return text;
    }
    return QString();
}

QString BtModuleTextModel::highlightFindPreviousNextField(const QString& text) const {
    QString t = text;
    int from = 0;
    for (int i = 0; i < m_findState.subIndex; ++i) {
        int pos = t.indexOf("\"highlightwords\"", from);
        if (pos == -1)
            return t;
        else {
            from = pos + 1;
        }
    }
    int position = from + 14; // highlightwords = 14, quote was already added
    t.insert(position, "2");
    return t;
}

int BtModuleTextModel::columnCount(const QModelIndex & /*parent*/) const {
    return 1;
}

int BtModuleTextModel::rowCount(const QModelIndex & /*parent*/) const {
    return m_maxEntries;
}

QHash<int, QByteArray> BtModuleTextModel::roleNames() const {
    return m_roleNames;
}

void BtModuleTextModel::setRoleNames(const QHash<int, QByteArray> &roleNames) {
    m_roleNames = roleNames;
}

bool BtModuleTextModel::isBible() const {
    const CSwordModuleInfo* module = m_moduleInfoList.at(0);
    if (module == nullptr)
        return false;
    return module->type() == CSwordModuleInfo::Bible;
}

bool BtModuleTextModel::isBook() const {
    const CSwordModuleInfo* module = m_moduleInfoList.at(0);
    if (module == nullptr)
        return false;
    return module->type() == CSwordModuleInfo::GenericBook;
}

bool BtModuleTextModel::isCommentary() const {
    const CSwordModuleInfo* module = m_moduleInfoList.at(0);
    if (module == nullptr)
        return false;
    return module->type() == CSwordModuleInfo::Commentary;
}

bool BtModuleTextModel::isLexicon() const {
    const CSwordModuleInfo* module = m_moduleInfoList.at(0);
    if (module == nullptr)
        return false;
    return module->type() == CSwordModuleInfo::Lexicon;
}

// Function is valid for Bibles, Commentaries, and Books
int BtModuleTextModel::keyToIndex(const CSwordKey* key) const {
    int index = 0;
    const CSwordTreeKey* treeKey = dynamic_cast<const CSwordTreeKey*>(key);
    if (treeKey) {
        index = treeKey->getIndex()/4;
    } else {
        const CSwordVerseKey* verseKey = dynamic_cast<const CSwordVerseKey*>(key);
        if (verseKey) {
            index = verseKey->getIndex();
        }
    }
    return index;
}

int BtModuleTextModel::verseKeyToIndex(const CSwordVerseKey& key) const {
    int index = key.getIndex() - m_firstEntry;
    return index;
}

CSwordVerseKey BtModuleTextModel::indexToVerseKey(int index) const
{
    const CSwordModuleInfo* module = m_moduleInfoList.at(0);
    CSwordVerseKey key(module);

    key.setIntros(true);
    key.setIndex(index + m_firstEntry);
    return key;
}

int BtModuleTextModel::indexToVerse(int index) const
{
    const CSwordModuleInfo* module = m_moduleInfoList.at(0);
    CSwordVerseKey key(module);

    key.setIntros(true);
    key.setIndex(index + m_firstEntry);
    return key.getVerse();
}

CSwordKey* BtModuleTextModel::indexToKey(int index, int moduleNum) const
{
    const CSwordModuleInfo* module = m_moduleInfoList.at(moduleNum);
    CSwordKey* key = CSwordKey::createInstance(module);
    QString keyName = indexToKeyName(index);
    key->setKey(keyName);
    return key;
}

CSwordTreeKey BtModuleTextModel::indexToBookKey(int index) const
{
    const CSwordModuleInfo* module = m_moduleInfoList.at(0);
    const CSwordBookModuleInfo *bookModule = qobject_cast<const CSwordBookModuleInfo*>(module);
    CSwordTreeKey key(bookModule->tree(), bookModule);
    int bookIndex = index * 4;
    key.setIndex(bookIndex);
    return key;
}

QString BtModuleTextModel::indexToKeyName(int index) const {
    if (index < 0)
        return "";
    QString keyName = "???";
    if (isBible() || isCommentary()) {
        CSwordVerseKey key = indexToVerseKey(index);
        keyName = key.key();
    }
    else if (isBook()) {
        CSwordTreeKey key = indexToBookKey(index);
        keyName = key.key();
    }
    else if (isLexicon()) {
        const CSwordLexiconModuleInfo *lexiconModule = qobject_cast<const CSwordLexiconModuleInfo*>(m_moduleInfoList.at(0));
        keyName = lexiconModule->entries()[index];
    }
    return keyName;
}

int BtModuleTextModel::getFirstEntryIndex() const {
    return m_firstEntry;
}

void BtModuleTextModel::setFindState(const FindState& findState) {
    if (m_findState.enabled && m_findState.index != findState.index) {
        QModelIndex oldIndexToClear = index(m_findState.index, 0);
        m_findState  = findState;
        emit dataChanged(oldIndexToClear, oldIndexToClear);
    } else {
        m_findState  = findState;
    }
    if (findState.enabled) {
        QModelIndex index = this->index(findState.index, 0);
        emit dataChanged(index, index);
    }
}
void BtModuleTextModel::setHighlightWords(
        const QString& highlightWords, bool /* caseSensitive */) {
    beginResetModel();
    m_highlightWords = highlightWords;
    endResetModel();
}

void BtModuleTextModel::setDisplayOptions(const DisplayOptions & displayOptions) {
    if (m_displayOptions.displayOptionsAreEqual(displayOptions))
        return;
    beginResetModel();
    m_displayOptions = displayOptions;
    endResetModel();
}

void BtModuleTextModel::setFilterOptions(FilterOptions filterOptions) {
    if (m_filterOptions.filterOptionsAreEqual(filterOptions))
            return;
    beginResetModel();
    m_filterOptions = filterOptions;
    endResetModel();
}

void BtModuleTextModel::setTextFilter(BtModuleTextFilter * textFilter) {
    BT_ASSERT(m_textFilter == nullptr);
    m_textFilter = textFilter;
}

bool BtModuleTextModel::setData(
        const QModelIndex &index,
        const QVariant &value,
        int role) {
    CSwordVerseKey key = indexToVerseKey(index.row());
    int column = getColumnFromRole(role);
    const CSwordModuleInfo* module = m_moduleInfoList.at(column);
    CSwordVerseKey mKey(module);
    mKey.setKey(key);
    const_cast<CSwordModuleInfo*>(module)->write(&mKey, value.toString());
    emit dataChanged(index, index);
    return true;
}
