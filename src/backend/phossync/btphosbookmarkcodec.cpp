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

#include "btphosbookmarkcodec.h"

#include <array>
#include <QCryptographicHash>
#include <QJsonArray>
#include <QRegularExpression>
#include <string_view>

// Sword includes:
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsuggest-destructor-override"
#endif
#include <versekey.h>
#include <versificationmgr.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif


namespace {

constexpr std::array<std::string_view, 66> CANON{
    "Gen", "Exod", "Lev", "Num", "Deut", "Josh", "Judg", "Ruth", "1Sam",
    "2Sam", "1Kgs", "2Kgs", "1Chr", "2Chr", "Ezra", "Neh", "Esth", "Job",
    "Ps", "Prov", "Eccl", "Song", "Isa", "Jer", "Lam", "Ezek", "Dan", "Hos",
    "Joel", "Amos", "Obad", "Jonah", "Mic", "Nah", "Hab", "Zeph", "Hag",
    "Zech", "Mal",
    "Matt", "Mark", "Luke", "John", "Acts", "Rom", "1Cor", "2Cor", "Gal",
    "Eph", "Phil", "Col", "1Thess", "2Thess", "1Tim", "2Tim", "Titus",
    "Phlm", "Heb", "Jas", "1Pet", "2Pet", "1John", "2John", "3John", "Jude",
    "Rev"};

QString const DEFAULT_V11N = QStringLiteral("KJV");

QString str(QJsonObject const & o, QString const & k)
{ return o.value(k).isString() ? o.value(k).toString() : QString(); }

int integer(QJsonObject const & o, QString const & k, int d = 0)
{ return o.value(k).isDouble() ? o.value(k).toInt() : d; }

qint64 lng(QJsonObject const & o, QString const & k, qint64 d = 0)
{ return o.value(k).isDouble() ? o.value(k).toInteger() : d; }

sword::VersificationMgr::System const * system(QString const & v11n) {
    auto * const mgr = sword::VersificationMgr::getSystemVersificationMgr();
    auto const * sys = mgr->getVersificationSystem(
                (v11n.isEmpty() ? DEFAULT_V11N : v11n).toUtf8().constData());
    if (!sys)
        sys = mgr->getVersificationSystem(DEFAULT_V11N.toUtf8().constData());
    return sys;
}

QString versificationName(QString const & v11n) {
    auto const * const sys = system(v11n);
    return sys ? QString::fromUtf8(sys->getName()) : DEFAULT_V11N;
}

/** An English reference "Book 1:2" as BibleTime stores verse keys. */
bool looksLikeVerseKey(QString const & key) {
    static QRegularExpression const re(
                QStringLiteral("^\\S.* \\d+:\\d+(-.+)?$"));
    return re.match(key).hasMatch();
}

} // anonymous namespace


namespace BtPhosIds {

QString derivedUuidHex(QString const & ns, QString const & value) {
    QByteArray in = ns.toUtf8();
    in.append('\0');
    in.append(value.toUtf8());
    auto d = QCryptographicHash::hash(in, QCryptographicHash::Md5);
    d[6] = static_cast<char>((static_cast<unsigned char>(d[6]) & 0x0Fu) | 0x40u);
    d[8] = static_cast<char>((static_cast<unsigned char>(d[8]) & 0x3Fu) | 0x80u);
    return QString::fromLatin1(d.toHex());
}

bool isUuidHex(QString const & s) {
    static QRegularExpression const re(QStringLiteral("^[0-9a-fA-F]{32}$"));
    return re.match(s).hasMatch();
}

QString bookmarkUuidHex(QString const & phosId) {
    return isUuidHex(phosId)
            ? phosId.toLower()
            : derivedUuidHex(QStringLiteral("phos-bookmark"), phosId);
}

int canonIndex(QString const & osisBook) {
    auto const b = osisBook.toUtf8();
    for (std::size_t i = 0; i < CANON.size(); ++i)
        if (CANON[i] == std::string_view(b.constData(),
                                         static_cast<std::size_t>(b.size())))
            return static_cast<int>(i);
    return -1;
}

} // namespace BtPhosIds


namespace BtPhosBookmarkCodec {

QString labelName(QStringList const & folder)
{ return folder.join(u'/'); }

QStringList folderOfLabel(QString const & labelName) {
    QStringList out;
    for (auto const & part : labelName.split(u'/'))
        if (!part.trimmed().isEmpty())
            out.append(part);
    return out;
}

std::optional<VerseLocation> verseLocation(QString const & englishKey,
                                           QString const & versification)
{
    auto const key = englishKey.trimmed();
    if (key.isEmpty())
        return std::nullopt;
    // A range is "Genesis 1:1-Genesis 1:3" (or "Genesis 1:1-3").
    static QRegularExpression const rangeRe(
                QStringLiteral("^(.*?\\d+:\\d+)\\s*-\\s*(.+)$"));
    auto const m = rangeRe.match(key);
    auto const first = m.hasMatch() ? m.captured(1) : key;
    auto const v11n = versificationName(versification).toUtf8();

    auto parse = [&v11n](QString const & k)
            -> std::optional<std::tuple<QString, int, int>>
    {
        sword::VerseKey vk;
        vk.setLocale("en");
        vk.setVersificationSystem(v11n.constData());
        vk.setAutoNormalize(false);
        vk.setText(k.toUtf8().constData());
        if (vk.popError())
            return std::nullopt;
        if (vk.getChapter() < 1 || vk.getVerse() < 1)
            return std::nullopt;
        return std::make_tuple(QString::fromUtf8(vk.getOSISBookName()),
                               vk.getChapter(), vk.getVerse());
    };

    auto const start = parse(first);
    if (!start)
        return std::nullopt;
    VerseLocation loc;
    loc.bookOsis = std::get<0>(*start);
    loc.chap = std::get<1>(*start);
    loc.verse = std::get<2>(*start);
    if (m.hasMatch()) {
        auto end = m.captured(2).trimmed();
        if (!end.contains(u' ')) { // "3" or "4:2"
            auto const space = first.lastIndexOf(u' ');
            auto const colon = first.lastIndexOf(u':');
            end = end.contains(u':')
                  ? first.left(space + 1) + end
                  : first.left(colon + 1) + end;
        }
        if (auto const e = parse(end)) {
            loc.endBookOsis = std::get<0>(*e);
            loc.endChap = std::get<1>(*e);
            loc.endVerse = std::get<2>(*e);
        }
    }
    return loc;
}

std::optional<QString> englishKey(QString const & bookOsis,
                                  int const chap,
                                  int const verse,
                                  QString const & versification)
{
    auto const * const sys = system(versification);
    if (!sys || bookOsis.isEmpty() || chap < 1 || verse < 1)
        return std::nullopt;
    // Sword counts these from 1
    auto const book =
            sys->getBookNumberByOSISName(bookOsis.toUtf8().constData()) - 1;
    if (book < 0)
        return std::nullopt;
    auto const * const b = sys->getBook(book);
    if (!b || chap > b->getChapterMax() || verse > b->getVerseMax(chap))
        return std::nullopt;
    sword::VerseKey vk;
    vk.setLocale("en");
    vk.setVersificationSystem(sys->getName());
    auto const otMax = sys->getBMAX()[0];
    vk.setTestament(book < otMax ? 1 : 2);
    vk.setBook(static_cast<char>(book < otMax ? book + 1 : book - otMax + 1));
    vk.setChapter(chap);
    vk.setVerse(verse);
    return QString::fromUtf8(vk.getText());
}

QJsonObject labelToJson(QStringList const & folder)
{ return QJsonObject{{QStringLiteral("name"), labelName(folder)}}; }

QSet<QString> const & labelFields() {
    static QSet<QString> const fields{QStringLiteral("name")};
    return fields;
}

QSet<QString> const & bookmarkFields() {
    static QSet<QString> const fields{
        QStringLiteral("id"), QStringLiteral("kind"), QStringLiteral("module"),
        QStringLiteral("bookOsis"), QStringLiteral("bookIdx"),
        QStringLiteral("chap"), QStringLiteral("vStart"),
        QStringLiteral("vEnd"), QStringLiteral("entryKey"),
        QStringLiteral("labelNames"), QStringLiteral("noteMd"),
        QStringLiteral("created"), QStringLiteral("modified"),
        QStringLiteral("bt")};
    return fields;
}

std::optional<QJsonObject> bookmarkToJson(Bookmark const & bm,
                                          ModuleInfo const & module)
{
    QJsonObject o;
    QJsonObject bt;
    o.insert(QStringLiteral("id"), bm.id);
    o.insert(QStringLiteral("module"), bm.module);
    bool const verseKeyed = module.installed ? module.verseKeyed
                                             : looksLikeVerseKey(bm.key);
    if (verseKeyed) {
        auto const loc = verseLocation(bm.key, module.versification);
        if (!loc)
            return std::nullopt;
        bool const range = !loc->endBookOsis.isEmpty()
                && (loc->endBookOsis != loc->bookOsis
                    || loc->endChap != loc->chap
                    || loc->endVerse != loc->verse);
        bool const sameChapter = loc->endBookOsis == loc->bookOsis
                                 && loc->endChap == loc->chap;
        o.insert(QStringLiteral("kind"),
                 range ? QStringLiteral("range") : QStringLiteral("verse"));
        o.insert(QStringLiteral("bookOsis"), loc->bookOsis);
        o.insert(QStringLiteral("bookIdx"), BtPhosIds::canonIndex(loc->bookOsis));
        o.insert(QStringLiteral("chap"), loc->chap);
        o.insert(QStringLiteral("vStart"), loc->verse);
        o.insert(QStringLiteral("vEnd"),
                 (range && sameChapter) ? std::max(loc->endVerse, loc->verse)
                                        : loc->verse);
        if (range && !sameChapter)
            bt.insert(QStringLiteral("endKey"), bm.key);
    } else {
        if (bm.key.isEmpty())
            return std::nullopt;
        o.insert(QStringLiteral("kind"), QStringLiteral("entry"));
        o.insert(QStringLiteral("entryKey"), bm.key);
    }
    QJsonArray labels;
    if (!bm.folder.isEmpty())
        labels.append(labelName(bm.folder));
    o.insert(QStringLiteral("labelNames"), labels);
    if (!bm.description.isEmpty())
        o.insert(QStringLiteral("noteMd"), bm.description);
    o.insert(QStringLiteral("created"), bm.created);
    o.insert(QStringLiteral("modified"), bm.modified);
    if (!bm.title.isEmpty())
        bt.insert(QStringLiteral("title"), bm.title);
    if (!bt.isEmpty())
        o.insert(QStringLiteral("bt"), bt);
    return o;
}

std::optional<Bookmark> bookmarkFromJson(QString const & id,
                                         QJsonObject const & p,
                                         ModuleLookup const & lookup,
                                         QString const & defaultBible)
{
    Bookmark bm;
    bm.id = id;
    auto const bt = p.value(QStringLiteral("bt")).toObject();
    auto const kind = str(p, QStringLiteral("kind"));
    auto const bookOsis = str(p, QStringLiteral("bookOsis"));
    auto const entryKey = str(p, QStringLiteral("entryKey"));
    auto module = str(p, QStringLiteral("module"));

    if (!bookOsis.isEmpty()) {
        // a verse (also a range or a selection: BibleTime marks its first verse)
        auto info = module.isEmpty() ? ModuleInfo{} : lookup(module);
        if (module.isEmpty() || (info.installed && !info.verseKeyed)) {
            // no Bible named: shown in the standard Bible, while the record
            // keeps saying what it said
            if (defaultBible.isEmpty())
                return std::nullopt;
            module = defaultBible;
            info = lookup(module);
        }
        // a module not installed here keeps its name (and BibleTime's default
        // versification for the key)
        auto const v11n = info.installed ? info.versification : QString();
        auto key = englishKey(bookOsis,
                              integer(p, QStringLiteral("chap")),
                              integer(p, QStringLiteral("vStart"), 1),
                              v11n);
        if (!key)
            return std::nullopt;
        auto const endKey = str(bt, QStringLiteral("endKey"));
        bm.key = endKey.isEmpty() ? *key : endKey;
    } else if (!entryKey.isEmpty() && !module.isEmpty()
               && kind != QStringLiteral("verse"))
    {
        bm.key = entryKey;
    } else {
        // no target (e.g. a note of a study pad): nothing BibleTime can show
        return std::nullopt;
    }
    bm.module = module;

    auto note = str(p, QStringLiteral("noteMd"));
    if (note.isEmpty())
        note = htmlToText(str(p, QStringLiteral("noteHtml")));
    bm.description = note;
    auto const labels = p.value(QStringLiteral("labelNames")).toArray();
    for (auto const & l : labels) {
        if (l.isString() && !folderOfLabel(l.toString()).isEmpty()) {
            bm.folder = folderOfLabel(l.toString());
            break;
        }
    }
    bm.title = str(bt, QStringLiteral("title"));
    bm.created = lng(p, QStringLiteral("created"));
    bm.modified = lng(p, QStringLiteral("modified"), bm.created);
    return bm;
}

QJsonObject bookmarkOverlay(QJsonObject const * server, QJsonObject const & mine) {
    if (!server)
        return mine;
    QJsonObject out;
    auto const & owned = bookmarkFields();
    for (auto it = server->begin(); it != server->end(); ++it)
        if (!owned.contains(it.key()))
            out.insert(it.key(), it.value());
    for (auto it = mine.begin(); it != mine.end(); ++it)
        out.insert(it.key(), it.value());

    // the sender's id stays
    if (server->contains(QStringLiteral("id")))
        out.insert(QStringLiteral("id"), server->value(QStringLiteral("id")));

    // the same target: keep how the server describes it
    auto const sameVerse =
            !str(mine, QStringLiteral("bookOsis")).isEmpty()
            && str(*server, QStringLiteral("bookOsis"))
               == str(mine, QStringLiteral("bookOsis"))
            && integer(*server, QStringLiteral("chap"))
               == integer(mine, QStringLiteral("chap"))
            && integer(*server, QStringLiteral("vStart"))
               == integer(mine, QStringLiteral("vStart"));
    auto const sameEntry =
            !str(mine, QStringLiteral("entryKey")).isEmpty()
            && str(*server, QStringLiteral("entryKey"))
               == str(mine, QStringLiteral("entryKey"));
    if (sameVerse || sameEntry) {
        for (auto const & k : {QStringLiteral("module"), QStringLiteral("kind"),
                               QStringLiteral("vEnd"), QStringLiteral("bookIdx")})
        {
            if (server->contains(k)) {
                out.insert(k, server->value(k));
            } else {
                out.remove(k);
            }
        }
    }

    // the first label is the folder (moved: the old folder's label goes); the
    // others stay
    auto const serverLabels = server->value(QStringLiteral("labelNames")).toArray();
    auto const myLabels = mine.value(QStringLiteral("labelNames")).toArray();
    QJsonArray labels;
    if (!myLabels.isEmpty())
        labels.append(myLabels.first());
    for (qsizetype i = 1; i < serverLabels.size(); ++i)
        if (!labels.contains(serverLabels[i]))
            labels.append(serverLabels[i]);
    out.insert(QStringLiteral("labelNames"), labels);

    // the note changed here: a formatted copy of the old note no longer matches
    if (str(*server, QStringLiteral("noteMd")) != str(mine, QStringLiteral("noteMd")))
        out.remove(QStringLiteral("noteHtml"));
    return out;
}

QString htmlToText(QString const & html) {
    if (html.isEmpty())
        return {};
    auto s = html;
    s.remove(u'\r');
    using RE = QRegularExpression;
    static RE const br(QStringLiteral("<br\\s*/?>"), RE::CaseInsensitiveOption);
    static RE const block(QStringLiteral("</(p|div|h[1-6]|blockquote)>"),
                          RE::CaseInsensitiveOption);
    static RE const li(QStringLiteral("<li[^>]*>"), RE::CaseInsensitiveOption);
    static RE const liEnd(QStringLiteral("</li>"), RE::CaseInsensitiveOption);
    static RE const bold(QStringLiteral("</?(b|strong)>"), RE::CaseInsensitiveOption);
    static RE const ital(QStringLiteral("</?(i|em)>"), RE::CaseInsensitiveOption);
    static RE const tag(QStringLiteral("<[^>]*>"));
    static RE const numEntity(QStringLiteral("&#(\\d+);"));
    static RE const trailing(QStringLiteral("[ \\t]+\n"));
    static RE const blank(QStringLiteral("\n{3,}"));
    s.replace(br, QStringLiteral("\n"));
    s.replace(block, QStringLiteral("\n\n"));
    s.replace(li, QStringLiteral("- "));
    s.replace(liEnd, QStringLiteral("\n"));
    s.replace(bold, QStringLiteral("**"));
    s.replace(ital, QStringLiteral("*"));
    s.replace(tag, QString());
    s.replace(QStringLiteral("&nbsp;"), QStringLiteral(" "));
    s.replace(QStringLiteral("&lt;"), QStringLiteral("<"));
    s.replace(QStringLiteral("&gt;"), QStringLiteral(">"));
    s.replace(QStringLiteral("&quot;"), QStringLiteral("\""));
    s.replace(QStringLiteral("&#39;"), QStringLiteral("'"));
    s.replace(QStringLiteral("&apos;"), QStringLiteral("'"));
    for (auto m = numEntity.match(s); m.hasMatch(); m = numEntity.match(s)) {
        auto const code = m.captured(1).toUInt();
        s.replace(m.capturedStart(), m.capturedLength(),
                  code ? QString::fromUcs4(reinterpret_cast<char32_t const *>(&code), 1)
                       : QString());
    }
    s.replace(QStringLiteral("&amp;"), QStringLiteral("&"));
    s.replace(trailing, QStringLiteral("\n"));
    s.replace(blank, QStringLiteral("\n\n"));
    return s.trimmed();
}

} // namespace BtPhosBookmarkCodec
