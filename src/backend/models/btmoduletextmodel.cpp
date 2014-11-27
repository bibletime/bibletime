/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btmoduletextmodel.h"

#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/drivers/cswordbookmoduleinfo.h"
#include "backend/drivers/cswordlexiconmoduleinfo.h"
#include "backend/cswordmodulesearch.h"
#include "backend/keys/cswordtreekey.h"
#include "backend/keys/cswordldkey.h"
#include "backend/managers/cswordbackend.h"
#include "backend/rendering/ctextrendering.h"


// Static so all models use the same colors
static QColor s_linkColor = QColor(0,191,255);
static QColor s_highlightColor = QColor(255,255,0);
static QColor s_jesusWordsColor = QColor(255,0,0);

/*static*/ void BtModuleTextModel::setLinkColor(const QColor& color) {
    s_linkColor = color;
}

/*static*/ void BtModuleTextModel::setHighlightColor(const QColor& color) {
    s_highlightColor = color;
}

/*static*/ void BtModuleTextModel::setJesusWordsColor(const QColor& color) {
    s_jesusWordsColor = color;
}



BtModuleTextModel::BtModuleTextModel(QObject *parent)
    : QAbstractListModel(parent), m_firstEntry(0), m_maxEntries(0) {
    QHash<int, QByteArray> roleNames;
    roleNames[ModuleEntry::ReferenceRole] =  "keyName";
    roleNames[ModuleEntry::TextRole] = "line";
    setRoleNames(roleNames);
    m_displayOptions.verseNumbers = 0;
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
    m_filterOptions.textualVariants = 1;
    m_filterOptions.textualVariants = 0;
}

void BtModuleTextModel::setModules(const QStringList& modules) {
    beginResetModel();

    m_moduleInfoList.clear();
    for (int i = 0; i < modules.count(); ++i) {
        QString moduleName = modules.at(i);
        CSwordModuleInfo* module = CSwordBackend::instance()->findModuleByName(moduleName);
        m_moduleInfoList.append(module);
    }

    const CSwordModuleInfo* firstModule = m_moduleInfoList.at(0);

    if (isBible() || isCommentary())
    {
        const CSwordBibleModuleInfo *bm = qobject_cast<const CSwordBibleModuleInfo*>(firstModule);
        m_firstEntry = bm->lowerBound().getIndex();
        m_maxEntries = bm->upperBound().getIndex() - m_firstEntry + 1;
    }

    else if(isLexicon())
    {
        const CSwordLexiconModuleInfo *lm = qobject_cast<const CSwordLexiconModuleInfo*>(firstModule);
        m_maxEntries = lm->entries().size();
    }

    else if(isBook())
    {
        const CSwordBookModuleInfo *bookModule = qobject_cast<const CSwordBookModuleInfo*>(firstModule);
        sword::TreeKeyIdx tk(*bookModule->tree());
        tk.root();
        tk.firstChild();
        Q_ASSERT(tk.getOffset() == 4);
        tk.setPosition(sword::BOTTOM);
        m_maxEntries = tk.getOffset() / 4;
    }

    endResetModel();
}

QVariant BtModuleTextModel::data(const QModelIndex & index, int role) const {

    if (isBible() || isCommentary())
        return verseData(index, role);
    else if (isBook())
        return bookData(index, role);
    else if (isLexicon())
        return lexiconData(index, role);
    return QVariant("invalid");
}

QVariant BtModuleTextModel::lexiconData(const QModelIndex & index, int role) const {
    int row = index.row();

    const CSwordLexiconModuleInfo *lexiconModule = qobject_cast<const CSwordLexiconModuleInfo*>(m_moduleInfoList.at(0));
    QList<const CSwordModuleInfo*> moduleList;
    moduleList << lexiconModule;
    QString keyName = lexiconModule->entries()[row];

    if (role == ModuleEntry::TextRole) {
        Rendering::CEntryDisplay entryDisplay;
        QString text = entryDisplay.text(moduleList, keyName,
            m_displayOptions, m_filterOptions);
        text.replace("#CHAPTERTITLE#", "");
        text = replaceColors(text);
        return CSwordModuleSearch::highlightSearchedText(text, m_highlightWords);
    }
    else if (role == ModuleEntry::ReferenceRole){
        return keyName;
    }
    return QString();
}

QVariant BtModuleTextModel::bookData(const QModelIndex & index, int role) const {
    if (role == ModuleEntry::TextRole) {
        const CSwordBookModuleInfo *bookModule = qobject_cast<const CSwordBookModuleInfo*>(m_moduleInfoList.at(0));
        CSwordTreeKey key(bookModule->tree(), bookModule);
        int bookIndex = index.row() * 4;
        key.setIndex(bookIndex);
        Rendering::CEntryDisplay entryDisplay;
        QList<const CSwordModuleInfo*> moduleList;
        moduleList << bookModule;
        QString text = entryDisplay.textKeyRendering(moduleList, key.key(),
                                                     m_displayOptions, m_filterOptions,
                                                     Rendering::CTextRendering::KeyTreeItem::Settings::SimpleKey);
        text.replace("#CHAPTERTITLE#", "");
        return CSwordModuleSearch::highlightSearchedText(text, m_highlightWords);
        return text;
    }
    return QString();
}

QVariant BtModuleTextModel::verseData(const QModelIndex & index, int role) const {
    int row = index.row();
    CSwordVerseKey key = indexToVerseKey(row);
    int verse = key.getVerse();
    if (role == ModuleEntry::TextRole) {
        if (verse == 0)
            return QString();
        QString text;

        QString chapterTitle;
        if (verse == 1)
            chapterTitle = key.book() + " " + QString::number(key.getChapter());

        text += Rendering::CEntryDisplay().textKeyRendering(m_moduleInfoList,
            key.key(), m_displayOptions, m_filterOptions,
            Rendering::CTextRendering::KeyTreeItem::Settings::SimpleKey);
        text.replace("#CHAPTERTITLE#", chapterTitle);
        text = replaceColors(text);
        return CSwordModuleSearch::highlightSearchedText(text, m_highlightWords);
    }
    return QString();
}

QString BtModuleTextModel::replaceColors(const QString& text) const {
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
    if (module == 0)
        return false;
    return module->type() == CSwordModuleInfo::Bible;
}

bool BtModuleTextModel::isBook() const {
    const CSwordModuleInfo* module = m_moduleInfoList.at(0);
    if (module == 0)
        return false;
    return module->type() == CSwordModuleInfo::GenericBook;
}

bool BtModuleTextModel::isCommentary() const {
    const CSwordModuleInfo* module = m_moduleInfoList.at(0);
    if (module == 0)
        return false;
    return module->type() == CSwordModuleInfo::Commentary;
}

bool BtModuleTextModel::isLexicon() const {
    const CSwordModuleInfo* module = m_moduleInfoList.at(0);
    if (module == 0)
        return false;
    return module->type() == CSwordModuleInfo::Lexicon;
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

void BtModuleTextModel::setHighlightWords(const QString& highlightWords) {
    beginResetModel();
    m_highlightWords = highlightWords;
    endResetModel();
}

