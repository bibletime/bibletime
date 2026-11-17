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

#pragma once

#include <functional>
#include <optional>
#include <QJsonObject>
#include <QSet>
#include <QString>
#include <QStringList>


/** Ids as the Phos clients write them: UUIDs as 32 lowercase hex digits. */
namespace BtPhosIds {

/** md5(namespace + NUL + value) with the version 4 and variant bits set. */
QString derivedUuidHex(QString const & ns, QString const & value);

bool isUuidHex(QString const & s);

/** The local id for a bookmark id of the server (the UUID itself, or derived). */
QString bookmarkUuidHex(QString const & phosId);

/** Index in the 66 book Protestant canon (Gen = 0, Rev = 65), else -1. */
int canonIndex(QString const & osisBook);

} // namespace BtPhosIds

/** Mapping between BibleTime bookmarks and the Phos record payloads. */
namespace BtPhosBookmarkCodec {

/** A BibleTime bookmark as the codec sees it. */
struct Bookmark {
    QString id;
    /** Folder names from the top level down. */
    QStringList folder;
    QString module;
    /** The key in English (a verse reference for Bibles and commentaries). */
    QString key;
    QString description;
    /** Empty when the bookmark shows its default title. */
    QString title;
    qint64 created = 0;  // seconds
    qint64 modified = 0; // seconds
};

/** What BibleTime knows about a module. */
struct ModuleInfo {
    bool installed = false;
    /** Bible or commentary: keys are verses. */
    bool verseKeyed = false;
    /** Versification system of a verse keyed module. */
    QString versification;
};

using ModuleLookup = std::function<ModuleInfo(QString const & moduleName)>;

struct VerseLocation {
    QString bookOsis;
    int chap = 0;
    int verse = 0;
    /** Set for a range. */
    QString endBookOsis;
    int endChap = 0;
    int endVerse = 0;
};

/** The label name of a folder path ("Study/Romans"). */
QString labelName(QStringList const & folder);
QStringList folderOfLabel(QString const & labelName);

/** Parses an English SWORD verse key (also a range) in the given versification. */
std::optional<VerseLocation> verseLocation(QString const & englishKey,
                                           QString const & versification);

/** The English SWORD key of a verse; none when it does not exist in the versification. */
std::optional<QString> englishKey(QString const & bookOsis,
                                  int chap,
                                  int verse,
                                  QString const & versification);

QJsonObject labelToJson(QStringList const & folder);

/** Fields a label record of BibleTime writes; the others stay as other clients set them. */
QSet<QString> const & labelFields();

/** Fields a bookmark record of BibleTime writes. */
QSet<QString> const & bookmarkFields();

/**
  The payload of a bookmark; none when its key can not be understood (it is
  then not synchronized).
*/
std::optional<QJsonObject> bookmarkToJson(Bookmark const & bm,
                                          ModuleInfo const & module);

/**
  The bookmark a payload describes, placed in the folder of its first label.
  None when BibleTime can not show it (no target, e.g. a study pad note).
  \param defaultBible used for verse bookmarks without (an installed) module.
*/
std::optional<Bookmark> bookmarkFromJson(QString const & id,
                                         QJsonObject const & payload,
                                         ModuleLookup const & lookup,
                                         QString const & defaultBible);

/**
  What BibleTime sends for a bookmark: its own fields over the server's copy.
  The sender's id stays, and as long as the target is the same, so do the
  server's module and target details (a verse bookmark without module, a
  range, a selection). Labels after the first (the folder) are kept.
*/
QJsonObject bookmarkOverlay(QJsonObject const * server, QJsonObject const & mine);

/** Plain text of a HTML note. */
QString htmlToText(QString const & html);

} // namespace BtPhosBookmarkCodec
