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

#include <QJsonArray>
#include <QTest>
#include "btphosbookmarkcodec.h"
#include "btphoshttpclient.h"


namespace C = BtPhosBookmarkCodec;

namespace {

C::ModuleInfo bible(QString const & v11n = QStringLiteral("KJV"))
{ return {true, true, v11n}; }

C::ModuleInfo lookup(QString const & name) {
    if (name == QStringLiteral("KJV"))
        return bible();
    if (name == QStringLiteral("GerLut1545"))
        return bible(QStringLiteral("Luther"));
    if (name == QStringLiteral("StrongsGreek"))
        return {true, false, {}};
    return {};
}

C::Bookmark verseBookmark() {
    C::Bookmark bm;
    bm.id = QStringLiteral("0123456789abcdef0123456789abcdef");
    bm.folder = {QStringLiteral("Studium"), QStringLiteral("Römer")};
    bm.module = QStringLiteral("KJV");
    bm.key = QStringLiteral("Romans 8:28");
    bm.description = QStringLiteral("Alles gut");
    bm.created = 1700000000;
    bm.modified = 1700000100;
    return bm;
}

} // anonymous namespace

class PhosCodecTest: public QObject {

    Q_OBJECT

private Q_SLOTS:

    void derivedIdsMatchTheOtherClients() {
        QCOMPARE(BtPhosIds::derivedUuidHex(QStringLiteral("phos-label"),
                                           QStringLiteral("Gebet")),
                 QStringLiteral("7e3591012fdf4cacbc08deacbc07bd51"));
        QCOMPARE(BtPhosIds::bookmarkUuidHex(QStringLiteral("6A168692-F30E62E0")),
                 QStringLiteral("244349bdbc34450a83dc66bb6ef728a4"));
        QCOMPARE(BtPhosIds::bookmarkUuidHex(
                     QStringLiteral("0123456789ABCDEF0123456789ABCDEF")),
                 QStringLiteral("0123456789abcdef0123456789abcdef"));
    }

    void canonIndex() {
        QCOMPARE(BtPhosIds::canonIndex(QStringLiteral("Gen")), 0);
        QCOMPARE(BtPhosIds::canonIndex(QStringLiteral("Mal")), 38);
        QCOMPARE(BtPhosIds::canonIndex(QStringLiteral("Matt")), 39);
        QCOMPARE(BtPhosIds::canonIndex(QStringLiteral("Rev")), 65);
        QCOMPARE(BtPhosIds::canonIndex(QStringLiteral("Tob")), -1);
    }

    void folderIsALabel() {
        QCOMPARE(C::labelName({QStringLiteral("Studium"), QStringLiteral("Römer")}),
                 QStringLiteral("Studium/Römer"));
        QCOMPARE(C::folderOfLabel(QStringLiteral("Studium/Römer")),
                 (QStringList{QStringLiteral("Studium"), QStringLiteral("Römer")}));
        QCOMPARE(C::folderOfLabel(QStringLiteral("Gebet")),
                 QStringList{QStringLiteral("Gebet")});
        QVERIFY(C::folderOfLabel(QStringLiteral("/")).isEmpty());
    }

    void verseKeys() {
        auto const loc = C::verseLocation(QStringLiteral("1 Corinthians 13:4"),
                                          QStringLiteral("KJV"));
        QVERIFY(loc);
        QCOMPARE(loc->bookOsis, QStringLiteral("1Cor"));
        QCOMPARE(loc->chap, 13);
        QCOMPARE(loc->verse, 4);
        QVERIFY(loc->endBookOsis.isEmpty());

        auto const range = C::verseLocation(QStringLiteral("John 3:16-John 3:18"),
                                            QStringLiteral("KJV"));
        QVERIFY(range);
        QCOMPARE(range->endVerse, 18);

        QCOMPARE(C::englishKey(QStringLiteral("1Cor"), 13, 4, QStringLiteral("KJV")),
                 std::optional<QString>(QStringLiteral("I Corinthians 13:4")));
        QVERIFY(!C::englishKey(QStringLiteral("Gen"), 51, 1, QStringLiteral("KJV")));
        QVERIFY(!C::englishKey(QStringLiteral("Nope"), 1, 1, QStringLiteral("KJV")));

        // round trip through the stored key
        auto const key = C::englishKey(QStringLiteral("Ps"), 23, 1,
                                       QStringLiteral("Luther"));
        QVERIFY(key);
        auto const back = C::verseLocation(*key, QStringLiteral("Luther"));
        QVERIFY(back);
        QCOMPARE(back->bookOsis, QStringLiteral("Ps"));
        QCOMPARE(back->chap, 23);
        QCOMPARE(back->verse, 1);
    }

    void verseBookmarkToJson() {
        auto const json = C::bookmarkToJson(verseBookmark(), bible());
        QVERIFY(json);
        QCOMPARE(json->value(QStringLiteral("kind")).toString(), QStringLiteral("verse"));
        QCOMPARE(json->value(QStringLiteral("bookOsis")).toString(), QStringLiteral("Rom"));
        QCOMPARE(json->value(QStringLiteral("bookIdx")).toInt(), 44);
        QCOMPARE(json->value(QStringLiteral("chap")).toInt(), 8);
        QCOMPARE(json->value(QStringLiteral("vStart")).toInt(), 28);
        QCOMPARE(json->value(QStringLiteral("vEnd")).toInt(), 28);
        QCOMPARE(json->value(QStringLiteral("labelNames")).toArray(),
                 QJsonArray{QStringLiteral("Studium/Römer")});
        QCOMPARE(json->value(QStringLiteral("noteMd")).toString(),
                 QStringLiteral("Alles gut"));
        QCOMPARE(json->value(QStringLiteral("created")).toInteger(), 1700000000);
        QVERIFY(!json->contains(QStringLiteral("bt")));
    }

    void entryBookmarkToJson() {
        C::Bookmark bm;
        bm.id = QStringLiteral("1123456789abcdef0123456789abcdef");
        bm.module = QStringLiteral("StrongsGreek");
        bm.key = QStringLiteral("00025");
        bm.title = QStringLiteral("agapao");
        auto const json = C::bookmarkToJson(bm, lookup(bm.module));
        QVERIFY(json);
        QCOMPARE(json->value(QStringLiteral("kind")).toString(), QStringLiteral("entry"));
        QCOMPARE(json->value(QStringLiteral("entryKey")).toString(), QStringLiteral("00025"));
        QCOMPARE(json->value(QStringLiteral("labelNames")).toArray(), QJsonArray());
        QCOMPARE(json->value(QStringLiteral("bt")).toObject()
                 .value(QStringLiteral("title")).toString(),
                 QStringLiteral("agapao"));
        QVERIFY(!json->contains(QStringLiteral("noteMd")));
    }

    void roundTrip() {
        auto const json = C::bookmarkToJson(verseBookmark(), bible());
        auto const bm = C::bookmarkFromJson(QStringLiteral("0123456789abcdef0123456789abcdef"),
                                            *json, lookup, QStringLiteral("KJV"));
        QVERIFY(bm);
        QCOMPARE(bm->key, QStringLiteral("Romans 8:28"));
        QCOMPARE(bm->module, QStringLiteral("KJV"));
        QCOMPARE(bm->folder, verseBookmark().folder);
        QCOMPARE(bm->description, QStringLiteral("Alles gut"));
        QCOMPARE(bm->modified, 1700000100);
        // what BibleTime renders again is what it received
        QCOMPARE(*C::bookmarkToJson(*bm, bible()), *json);
    }

    void phosVerseBookmarkWithoutModuleUsesTheStandardBible() {
        QJsonObject const p{{QStringLiteral("id"), QStringLiteral("6A168692-F30E62E0")},
                            {QStringLiteral("kind"), QStringLiteral("selection")},
                            {QStringLiteral("bookOsis"), QStringLiteral("John")},
                            {QStringLiteral("bookIdx"), 42},
                            {QStringLiteral("chap"), 3},
                            {QStringLiteral("vStart"), 16},
                            {QStringLiteral("vEnd"), 16},
                            {QStringLiteral("snippetText"), QStringLiteral("so loved")},
                            {QStringLiteral("labelNames"),
                             QJsonArray{QStringLiteral("Gebet"), QStringLiteral("Lesen")}},
                            {QStringLiteral("noteHtml"),
                             QStringLiteral("<p>Gott <b>liebt</b></p>")}};
        auto const bm = C::bookmarkFromJson(QStringLiteral("244349bdbc34450a83dc66bb6ef728a4"),
                                            p, lookup, QStringLiteral("KJV"));
        QVERIFY(bm);
        QCOMPARE(bm->module, QStringLiteral("KJV"));
        QCOMPARE(bm->key, QStringLiteral("John 3:16"));
        QCOMPARE(bm->folder, QStringList{QStringLiteral("Gebet")});
        QCOMPARE(bm->description, QStringLiteral("Gott **liebt**"));

        // an edit of the note here keeps what BibleTime does not own
        auto mine = *C::bookmarkToJson(*bm, bible());
        mine.insert(QStringLiteral("noteMd"), QStringLiteral("neu"));
        auto const out = C::bookmarkOverlay(&p, mine);
        QCOMPARE(out.value(QStringLiteral("id")).toString(), QStringLiteral("6A168692-F30E62E0"));
        QVERIFY(!out.contains(QStringLiteral("module")));
        QCOMPARE(out.value(QStringLiteral("kind")).toString(), QStringLiteral("selection"));
        QCOMPARE(out.value(QStringLiteral("snippetText")).toString(), QStringLiteral("so loved"));
        QCOMPARE(out.value(QStringLiteral("labelNames")).toArray(),
                 (QJsonArray{QStringLiteral("Gebet"), QStringLiteral("Lesen")}));
        QVERIFY(!out.contains(QStringLiteral("noteHtml"))); // the note changed
        QCOMPARE(out.value(QStringLiteral("noteMd")).toString(), QStringLiteral("neu"));
    }

    void movedToAnotherFolder() {
        QJsonObject const server{{QStringLiteral("labelNames"),
                                  QJsonArray{QStringLiteral("Gebet"), QStringLiteral("Lesen")}},
                                 {QStringLiteral("ab"), QJsonObject{{QStringLiteral("v11n"),
                                                                     QStringLiteral("KJV")}}}};
        QJsonObject const mine{{QStringLiteral("labelNames"),
                                QJsonArray{QStringLiteral("Archiv")}}};
        auto const out = C::bookmarkOverlay(&server, mine);
        QCOMPARE(out.value(QStringLiteral("labelNames")).toArray(),
                 (QJsonArray{QStringLiteral("Archiv"), QStringLiteral("Lesen")}));
        QVERIFY(out.contains(QStringLiteral("ab")));
    }

    void moduleNotInstalledHereKeepsItsName() {
        auto json = *C::bookmarkToJson(verseBookmark(), bible());
        json.insert(QStringLiteral("module"), QStringLiteral("GerElb1905"));
        auto const bm = C::bookmarkFromJson(QStringLiteral("x"), json, lookup,
                                            QStringLiteral("KJV"));
        QVERIFY(bm);
        QCOMPARE(bm->module, QStringLiteral("GerElb1905"));
        QCOMPARE(bm->key, QStringLiteral("Romans 8:28"));
    }

    void studyPadNoteIsNotShown() {
        QJsonObject const p{{QStringLiteral("kind"), QStringLiteral("entry")},
                            {QStringLiteral("labelNames"), QJsonArray{QStringLiteral("Gebet")}},
                            {QStringLiteral("noteMd"), QStringLiteral("Eigene Notiz")}};
        QVERIFY(!C::bookmarkFromJson(QStringLiteral("x"), p, lookup, QStringLiteral("KJV")));
    }

    void htmlToText() {
        QCOMPARE(C::htmlToText(QStringLiteral("a<br>b")), QStringLiteral("a\nb"));
        QCOMPARE(C::htmlToText(QStringLiteral("<ul><li>x</li><li>y</li></ul>")),
                 QStringLiteral("- x\n- y"));
        QCOMPARE(C::htmlToText(QStringLiteral("1 &lt; 2 &amp; 3")),
                 QStringLiteral("1 < 2 & 3"));
        QCOMPARE(C::htmlToText(QStringLiteral("&#228;")), QStringLiteral("ä"));
    }

    void appPasswords() {
        QVERIFY(BtPhosHttpClient::looksLikeAppPassword(
                    QStringLiteral("abcde-fghij-klmno-pqrst-uvwxy")));
        QVERIFY(BtPhosHttpClient::looksLikeAppPassword(QString(72, u'a')));
        QVERIFY(!BtPhosHttpClient::looksLikeAppPassword(QStringLiteral("geheim")));
        QCOMPARE(BtPhosHttpClient::normalizeServerUrl(QStringLiteral(" cloud.example.org/ ")),
                 QStringLiteral("https://cloud.example.org"));
    }

};

QTEST_GUILESS_MAIN(PhosCodecTest)
#include "test_phoscodec.moc"
