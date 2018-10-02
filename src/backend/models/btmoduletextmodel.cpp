/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

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
#include "../managers/cswordbackend.h"
#include "../rendering/ctextrendering.h"

// Static so all models use the same colors
static QColor s_linkColor = QColor(0,191,255);
static QColor s_highlightColor = QColor(255,255,0);
static QColor s_jesusWordsColor = QColor(255,0,0);
static bool s_replaceColors = false;

/*static*/ QColor BtModuleTextModel::getJesusWordsColor() {
    return s_jesusWordsColor;
}

/*static*/ void BtModuleTextModel::setLinkColor(const QColor& color) {
    s_replaceColors = true;
    s_linkColor = color;
}

/*static*/ void BtModuleTextModel::setHighlightColor(const QColor& color) {
    s_replaceColors = true;
    s_highlightColor = color;
}

/*static*/ void BtModuleTextModel::setJesusWordsColor(const QColor& color) {
    s_replaceColors = true;
    s_jesusWordsColor = color;
}

BtModuleTextFilter::~BtModuleTextFilter() {}



BtModuleTextModel::BtModuleTextModel(QObject *parent)
    : QAbstractListModel(parent),
      m_firstEntry(0),
      m_maxEntries(0),
      m_textFilter(nullptr) {

    m_findState.enabled = false;
    QHash<int, QByteArray> roleNames;
    roleNames[ModuleEntry::ReferenceRole] =  "keyName";
    roleNames[ModuleEntry::TextRole] = "line";
    roleNames[ModuleEntry::Text1Role] = "text1";
    roleNames[ModuleEntry::Text2Role] = "text2";
    roleNames[ModuleEntry::Text3Role] = "text3";
    roleNames[ModuleEntry::Text4Role] = "text4";
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

void BtModuleTextModel::setModules(const BtConstModuleList &modules) {

    m_moduleInfoList = modules;
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
    BtConstModuleList moduleList;
    for (int i = 0; i < modules.count(); ++i) {
        QString moduleName = modules.at(i);
        CSwordModuleInfo* module = CSwordBackend::instance()->findModuleByName(moduleName);
        moduleList.append(module);
    }
    setModules(moduleList);
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
    if (m_textFilter)
        text = m_textFilter->processText(text);
    QString t = CSwordModuleSearch::highlightSearchedText(text, m_highlightWords);
    if (m_findState.enabled && index.row() == m_findState.index)
        t = highlightFindPreviousNextField(t);
    return QVariant(t);
}

QString BtModuleTextModel::lexiconData(const QModelIndex & index, int role) const {
    int row = index.row();

    const CSwordLexiconModuleInfo *lexiconModule = qobject_cast<const CSwordLexiconModuleInfo*>(m_moduleInfoList.at(0));
    BtConstModuleList moduleList;
    moduleList << lexiconModule;
    QString keyName = lexiconModule->entries()[row];

    if (role == ModuleEntry::TextRole ||
            role == ModuleEntry::Text1Role) {
        Rendering::CEntryDisplay entryDisplay;
        QString text = entryDisplay.text(moduleList, keyName,
                                         m_displayOptions, m_filterOptions);
        text.replace("#CHAPTERTITLE#", "");
        text = replaceColors(text);
        return text;
    }
    else if (role == ModuleEntry::ReferenceRole){
        return keyName;
    }
    return QString();
}

QString BtModuleTextModel::bookData(const QModelIndex & index, int role) const {
    if (role == ModuleEntry::TextRole ||
            role == ModuleEntry::Text1Role) {
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

QString BtModuleTextModel::verseData(const QModelIndex & index, int role) const {
    int row = index.row();
    CSwordVerseKey key = indexToVerseKey(row);
    int verse = key.getVerse();
    if (role == ModuleEntry::TextRole ||
            role == ModuleEntry::Text1Role ||
            role == ModuleEntry::Text2Role ||
            role == ModuleEntry::Text3Role ||
            role == ModuleEntry::Text4Role) {
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
            int column = role - ModuleEntry::Text1Role;
            CSwordModuleInfo const * module;
            if ((column + 1) > m_moduleInfoList.count())
                module = m_moduleInfoList.at(0);
            else
                module = m_moduleInfoList.at(column);
            modules.append(module);
            CSwordVerseKey mKey(module);
            mKey.setKey(key.key());

            // Personal commentary
            if (module->isWritable()) {
                QString text = mKey.key() + "  " + mKey.rawText();
                return CSwordModuleSearch::highlightSearchedText(text, m_highlightWords);
            }
        }

        text += Rendering::CEntryDisplay().textKeyRendering(modules,
                                                            key.key(), m_displayOptions, m_filterOptions,
                                                            m_displayOptions.verseNumbers ?
                                                            Rendering::CTextRendering::KeyTreeItem::Settings::SimpleKey :
                                                            Rendering::CTextRendering::KeyTreeItem::Settings::NoKey);

        text.replace("#CHAPTERTITLE#", chapterTitle);
        text = replaceColors(text);
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

QString BtModuleTextModel::replaceColors(const QString& text) const {
    if (! s_replaceColors)
        return text;
    QString newText = text;
    newText.replace("#JESUS_WORDS_COLOR#", s_jesusWordsColor.name());
    newText.replace("#LINK_COLOR#", s_linkColor.name());
    newText.replace("#HIGHLIGHT_COLOR#", s_highlightColor.name());
    return newText;
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
    m_displayOptions = displayOptions;
}

void BtModuleTextModel::setFilterOptions(FilterOptions filterOptions) {
    beginResetModel();
    m_filterOptions = filterOptions;
    endResetModel();
}

void BtModuleTextModel::setTextFilter(BtModuleTextFilter * textFilter) {
    BT_ASSERT(m_textFilter == nullptr);
    m_textFilter = textFilter;
}

static int convertRoleToColumn(int role) {
    int column = 0;
    if (role == ModuleEntry::Text2Role)
        column = 1;
    if (role == ModuleEntry::Text3Role)
        column = 2;
    if (role == ModuleEntry::Text4Role)
        column = 3;
    return column;
}

bool BtModuleTextModel::setData(
        const QModelIndex &index,
        const QVariant &value,
        int role) {
    CSwordVerseKey key = indexToVerseKey(index.row());
    int column = convertRoleToColumn(role);
    const CSwordModuleInfo* module = m_moduleInfoList.at(column);
    CSwordVerseKey mKey(module);
    mKey.setKey(key);
    const_cast<CSwordModuleInfo*>(module)->write(&mKey, value.toString());
    emit dataChanged(index, index);
    return true;
}
