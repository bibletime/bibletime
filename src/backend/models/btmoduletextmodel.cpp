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
#include "../managers/colormanager.h"
#include "../managers/cswordbackend.h"
#include "../rendering/ctextrendering.h"


namespace {

DisplayOptions const defaultDisplayOptions = []() noexcept {
    DisplayOptions opts;
    opts.lineBreaks = 1;
    opts.verseNumbers = 1;
    return opts;
}();

FilterOptions const defaultFilterOptions = []() noexcept {
    FilterOptions opts;
    opts.footnotes = 0;
    opts.greekAccents = 1;
    opts.headings = 1;
    opts.hebrewCantillation = 1;
    opts.hebrewPoints = 1;
    opts.lemmas = 0;
    opts.morphSegmentation = 1;
    opts.morphTags = 1;
    opts.redLetterWords = 1;
    opts.scriptureReferences = 0;
    opts.strongNumbers = 0;
    opts.textualVariants = 0;
    return opts;
}();

} // anonymous namespace

BtModuleTextFilter::~BtModuleTextFilter() {}

BtModuleTextModel::BtModuleTextModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_firstEntry(0)
    , m_maxEntries(0)
    , m_textFilter(nullptr)
    , m_displayRendering(defaultDisplayOptions, defaultFilterOptions)
{}

void BtModuleTextModel::reloadModules() {
    m_moduleInfoList.clear();
    for (auto const & moduleName : m_modules)
        m_moduleInfoList.append(
                    CSwordBackend::instance().findModuleByName(
                        moduleName));

    beginResetModel();
    const CSwordModuleInfo* firstModule = m_moduleInfoList.at(0);
    if (isBible() || isCommentary()) {
        CSwordBibleModuleInfo const * const m =
            static_cast<const CSwordBibleModuleInfo *>(firstModule);
        m_firstEntry = m->lowerBound().index();
        m_maxEntries = m->upperBound().index() - m_firstEntry + 1;
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
        text = QStringLiteral("invalid");

    if (m_textFilter) {
        text = m_textFilter->processText(text);
    }

    if ( ! m_highlightWords.isEmpty()) {
        auto t = CSwordModuleSearch::highlightSearchedText(text,
                                                           m_highlightWords,
                                                           true);
        if (m_findState && index.row() == m_findState->index) {
            // t = highlightFindPreviousNextField(t); now inlined:
            int from = 0;
            for (int i = 0; i < m_findState->subIndex; ++i) {
                int pos = t.indexOf(QStringLiteral("\"highlightwords\""), from);
                if (pos == -1)
                    return t;
                else {
                    from = pos + 1;
                }
            }
            int position = from + 14; // highlightwords = 14, quote was already added
            t.insert(position, '2');
        }
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

    if (role == ModuleEntry::TextRole || role == ModuleEntry::Text0Role) {
        if (keyName.isEmpty())
            return {};
        auto text = m_displayRendering.renderDisplayEntry(moduleList, keyName);
        text.replace(QStringLiteral("#CHAPTERTITLE#"), QString());
        text.replace(QStringLiteral("#TEXT_ALIGN#"), QStringLiteral("left"));
        text = ColorManager::replaceColors(text);
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
        key.setOffset(bookIndex);
        BtConstModuleList moduleList;
        moduleList << bookModule;
        if (key.key().isEmpty())
            return {};
        auto text =
            m_displayRendering.renderDisplayEntry(
                moduleList,
                key.key(),
                m_displayRendering.displayOptions().verseNumbers
                ? Rendering::CTextRendering::KeyTreeItem::Settings::SimpleKey
                : Rendering::CTextRendering::KeyTreeItem::Settings::NoKey);
        text.replace(QStringLiteral("#CHAPTERTITLE#"), QString());
        text.replace(QStringLiteral("#TEXT_ALIGN#"), QStringLiteral("left"));
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
    int verse = key.verse();

    if (role >= ModuleEntry::TextRole && role <= ModuleEntry::Title9Role) {
        if (verse == 0)
            return QString();
        QString text;

        QString chapterTitle;
        if (verse == 1)
            chapterTitle =
                    QStringLiteral("%1 %2").arg(key.bookName(),
                                                QString::number(key.chapter()));

        BtConstModuleList modules;
        if ( role == ModuleEntry::TextRole) {
            modules = m_moduleInfoList;
        } else {
            int column = getColumnFromRole(role);
            CSwordModuleInfo const * module;
            if ((column + 1) > m_moduleInfoList.count())
                module = m_moduleInfoList.at(0);
            else
                module = m_moduleInfoList.at(column);
            modules.append(module);
            CSwordVerseKey mKey(module);
            mKey.setKey(key.key());

            // Title only for verse 1 of Personal commentary
            if (role >= ModuleEntry::Title0Role && role <= ModuleEntry::Title9Role) {
                if (module->isWritable() && verse == 1)
                    return QStringLiteral("<center><h3>%1</h3></center>")
                            .arg(chapterTitle);
                return {};
            }

            // Personal commentary
            if (module->isWritable()) {
                auto const & rawText = mKey.rawText();
                auto text =
                        QStringLiteral("%1 %2")
                        .arg(QString::number(verse),
                             rawText.isEmpty()
                             ? QStringLiteral("<span style=\"color:gray\">"
                                              "<small>%1</small></span>")
                               .arg(tr("Click to edit"))
                             : rawText);
                return CSwordModuleSearch::highlightSearchedText(
                            ColorManager::replaceColors(std::move(text)),
                            m_highlightWords);
            }
        }

        if (!key.key().isEmpty())
            text +=
                m_displayRendering.renderDisplayEntry(
                    modules,
                    key.key(),
                    m_displayRendering.displayOptions().verseNumbers
                    ? Rendering::CTextRendering::KeyTreeItem::Settings::SimpleKey
                    : Rendering::CTextRendering::KeyTreeItem::Settings::NoKey);

        text.replace(QStringLiteral("#CHAPTERTITLE#"), chapterTitle);
        text.replace(QStringLiteral("#TEXT_ALIGN#"), QStringLiteral("left"));
        text = ColorManager::replaceColors(text);
        return text;
    }
    return QString();
}

int BtModuleTextModel::columnCount(const QModelIndex & /*parent*/) const {
    return 1;
}

int BtModuleTextModel::rowCount(const QModelIndex & /*parent*/) const {
    return m_maxEntries;
}

QHash<int, QByteArray> BtModuleTextModel::roleNames() const {
    static auto const roleNames_ =
            []() {
                QHash<int, QByteArray> r;
                r[ModuleEntry::ReferenceRole] = "keyName"; // reference
                r[ModuleEntry::TextRole] = "line";         // not used
                r[ModuleEntry::Text0Role] = "text0";       // text in column 0
                r[ModuleEntry::Text1Role] = "text1";       // text in column 1
                r[ModuleEntry::Text2Role] = "text2";       // text in column 2
                r[ModuleEntry::Text3Role] = "text3";       // text in column 3
                r[ModuleEntry::Text4Role] = "text4";       // text in column 4
                r[ModuleEntry::Text5Role] = "text5";       // text in column 5
                r[ModuleEntry::Text6Role] = "text6";       // text in column 6
                r[ModuleEntry::Text7Role] = "text7";       // text in column 7
                r[ModuleEntry::Text8Role] = "text8";       // text in column 8
                r[ModuleEntry::Text9Role] = "text9";       // text in column 9
                r[ModuleEntry::Title0Role] = "title0";     // title in column 0
                r[ModuleEntry::Title1Role] = "title1";     // title in column 1
                r[ModuleEntry::Title2Role] = "title2";     // title in column 2
                r[ModuleEntry::Title3Role] = "title3";     // title in column 3
                r[ModuleEntry::Title4Role] = "title4";     // title in column 4
                r[ModuleEntry::Title5Role] = "title5";     // title in column 5
                r[ModuleEntry::Title6Role] = "title6";     // title in column 6
                r[ModuleEntry::Title7Role] = "title7";     // title in column 7
                r[ModuleEntry::Title8Role] = "title8";     // title in column 8
                r[ModuleEntry::Title9Role] = "title9";     // title in column 9
                return r;
            }();
    return roleNames_;
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
int BtModuleTextModel::keyToIndex(CSwordKey const & key) const {
    if (auto const * const treeKey = dynamic_cast<CSwordTreeKey const *>(&key))
        return treeKey->offset() / 4u;
    if (auto const * const vKey = dynamic_cast<CSwordVerseKey const *>(&key))
        return vKey->index();
    return 0;
}

int BtModuleTextModel::verseKeyToIndex(const CSwordVerseKey& key) const {
    int index = key.index() - m_firstEntry;
    return index;
}

CSwordVerseKey BtModuleTextModel::indexToVerseKey(int index) const
{ return indexToVerseKey(index, *m_moduleInfoList.front()); }

CSwordVerseKey
BtModuleTextModel::indexToVerseKey(int index,
                                   CSwordModuleInfo const & module) const
{
    CSwordVerseKey key(&module);
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
    return key.verse();
}

CSwordKey* BtModuleTextModel::indexToKey(int index, int moduleNum) const
{
    const CSwordModuleInfo* module = m_moduleInfoList.at(moduleNum);
    auto * const key = module->createKey();
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
    key.setOffset(bookIndex);
    return key;
}

QString BtModuleTextModel::indexToKeyName(int index) const {
    if (index < 0)
        return {};
    if (isBible() || isCommentary())
        return indexToVerseKey(index).key();
    if (isBook())
        return indexToBookKey(index).key();
    if (isLexicon())
        return qobject_cast<CSwordLexiconModuleInfo const *>(
                    m_moduleInfoList.at(0))->entries()[index];
    return QStringLiteral("???");
}

void BtModuleTextModel::setFindState(std::optional<FindState> findState) {
    if (m_findState
        && (!findState || (m_findState->index != findState->index)))
    {
        QModelIndex oldIndexToClear = index(m_findState->index, 0);
        m_findState = std::move(findState);
        Q_EMIT dataChanged(oldIndexToClear, oldIndexToClear);
    } else {
        m_findState = std::move(findState);
    }
    if (m_findState) {
        QModelIndex index = this->index(m_findState->index, 0);
        Q_EMIT dataChanged(index, index);
    }
}
void BtModuleTextModel::setHighlightWords(
        const QString& highlightWords, bool /* caseSensitive */) {
    beginResetModel();
    m_highlightWords = highlightWords;
    endResetModel();
}

void BtModuleTextModel::setDisplayOptions(const DisplayOptions & displayOptions) {
    if (m_displayRendering.displayOptions().displayOptionsAreEqual(displayOptions))
        return;
    beginResetModel();
    m_displayRendering.setDisplayOptions(displayOptions);
    endResetModel();
}

void BtModuleTextModel::setFilterOptions(FilterOptions filterOptions) {
    if (m_displayRendering.filterOptions().filterOptionsAreEqual(filterOptions))
            return;
    beginResetModel();
    m_displayRendering.setFilterOptions(filterOptions);
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
    auto const & module = *m_moduleInfoList.at(getColumnFromRole(role));
    CSwordVerseKey mKey(indexToVerseKey(index.row(), module));
    const_cast<CSwordModuleInfo &>(module).write(&mKey, value.toString());
    Q_EMIT dataChanged(index, index);
    return true;
}
