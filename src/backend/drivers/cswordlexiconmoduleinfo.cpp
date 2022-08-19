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

#include "cswordlexiconmoduleinfo.h"

#include <QChar>
#include <QDataStream>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QList>
#include <QRegExp>
#include <QTextCodec>
#include "../../util/directory.h"
#include "../keys/cswordldkey.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsuggest-destructor-override"
#endif
#include <swkey.h>
#include <swmodule.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#pragma GCC diagnostic pop


//Change it once the format changed to make all systems rebuild their caches
#define CACHE_FORMAT "3"

CSwordLexiconModuleInfo::CSwordLexiconModuleInfo(sword::SWModule & module,
                                                 CSwordBackend & backend)
        : CSwordModuleInfo(module, backend, Lexicon)
{
    /**
      See if module keys are consistent with Strong's references
      and determine if keys start with "G" or "H" and the number
      of digits in the keys.
    */
    module.setPosition(sword::TOP);
    module.increment();
    QString key = QString::fromUtf8(module.getKeyText());
    QRegExp rx1("[GH][0-9]+");
    if (rx1.exactMatch(key)) {
        m_hasStrongsKeys = true;
        m_hasLeadingStrongsLetter = true;
        m_strongsDigitsLength = key.length() - 1;
    } else {
        QRegExp rx2("[0-9]+");
        if (rx2.exactMatch(key)) {
            m_hasStrongsKeys = true;
            m_strongsDigitsLength = key.length();
        }
    }
}

const QStringList &CSwordLexiconModuleInfo::entries() const {
    namespace DU = util::directory;

    // If cache is ok, just return it:
    if (!m_entries.empty()) {
        return m_entries;
    }

    QString dir(DU::getUserCacheDir().absolutePath());
    QFile cacheFile( QString(dir).append("/").append(name()));

    /*
     * Try the module's cache
     */
    if (cacheFile.open(QIODevice::ReadOnly)) {
        QDataStream s(&cacheFile);
        QString ModuleVersion, CacheVersion, QDataStreamVersion;
        s >> ModuleVersion;
        s >> CacheVersion;
        s >> QDataStreamVersion;

        qDebug() << "Lexicon cache metadata"
        << "Name" << name()
        << "ModuleVersion" << ModuleVersion
        << "CacheVersion" << CacheVersion
        << "QDataStreamVersion" << QDataStreamVersion;

        // Check if cache is valid
        if (ModuleVersion == config(CSwordModuleInfo::ModuleVersion)
                && CacheVersion == CACHE_FORMAT
                && QDataStreamVersion == QString::number(s.version())) {
            s >> m_entries;

            cacheFile.close();
            qDebug() << "Read" << m_entries.count() << "entries from lexicon cache for module" << name();
            return m_entries;
        }

        cacheFile.close();
    }

    /*
     * Ok, no cache or invalid.
     */
    qDebug() << "Read all entries of lexicon" << name();

    auto & m = swordModule();
    m.setSkipConsecutiveLinks(true);
    m.setPosition(sword::TOP);
    snap(); //snap to top entry

    do {
        if ( isUnicode() ) {
            m_entries.append(QString::fromUtf8(m.getKeyText()));
        }
        else {
            //for latin1 modules use fromLatin1 because of speed
            QTextCodec* codec = QTextCodec::codecForName("Windows-1252");
            m_entries.append(codec->toUnicode(m.getKeyText()));
        }

        m.increment();
    } while (!m.popError());

    m.setPosition(sword::TOP); // back to the first entry
    m.setSkipConsecutiveLinks(false);

    /// \todo Document why the following code is here:
    if (!m_entries.empty() && m_entries.front().simplified().isEmpty())
        m_entries.pop_front();

    qDebug() << "Writing cache file for lexicon module" << name();

    if (m_entries.count()) {
        //create cache
        if (cacheFile.open( QIODevice::WriteOnly )) {
            QDataStream s(&cacheFile);
            s << config(CSwordModuleInfo::ModuleVersion) //store module version
            << QString(CACHE_FORMAT) //store BT version -- format may change
            << QString::number(s.version()) //store QDataStream version -- format may change
            << m_entries;
            cacheFile.close();
        }
    }

    return m_entries;
}

bool CSwordLexiconModuleInfo::snap() const
{ return swordModule().getRawEntry(); }

bool CSwordLexiconModuleInfo:: hasStrongsKeys() const {
    return m_hasStrongsKeys;
}

QString CSwordLexiconModuleInfo::normalizeStrongsKey(const QString &key) const {

    QRegExp rx("([GH]*)([0-9]+)");
    if (! rx.exactMatch(key))
        return key;
    QString StrongsChar = rx.cap(1);
    QString digits = rx.cap(2);

    while (digits.length() < m_strongsDigitsLength)
        digits = "0" +digits;

    while (digits.length() > m_strongsDigitsLength && digits.at(0) == "0")
        digits = digits.right(digits.length()-1);

    QString newKey = digits;
    if (m_hasLeadingStrongsLetter)
        newKey = StrongsChar + digits;
    return newKey;
}

CSwordKey * CSwordLexiconModuleInfo::createKey() const
{ return new CSwordLDKey(swordModule().getKey(), this); }
