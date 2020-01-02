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

#include "cswordlexiconmoduleinfo.h"

#include <QFile>
#include <QDataStream>
#include <QRegExp>
#include <QTextCodec>
#include <QDebug>

#include "../../util/directory.h"

// Sword includes:
#include <swmodule.h>


//Change it once the format changed to make all systems rebuild their caches
#define CACHE_FORMAT "3"

const QStringList &CSwordLexiconModuleInfo::entries() const {
    namespace DU = util::directory;

    // If cache is ok, just return it:
    if (!m_entries.empty()) {
        return m_entries;
    }

    QString dir(DU::getUserCacheDir().absolutePath());
    QFile f1( QString(dir).append("/").append(name()));

    /*
     * Try the module's cache
     */
    if ( f1.open( QIODevice::ReadOnly ) ) {
        QDataStream s( &f1 );
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

            f1.close();
            qDebug() << "Read" << m_entries.count() << "entries from lexicon cache for module" << name();
            return m_entries;
        }

        f1.close();
    }

    /*
     * Ok, no cache or invalid.
     */
    qDebug() << "Read all entries of lexicon" << name();

    auto & m = module();
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
        QString dir(DU::getUserCacheDir().absolutePath());
        QFile f2( QString(dir).append("/").append(name()) );

        if (f2.open( QIODevice::WriteOnly )) {
            QDataStream s( &f2 );
            s << config(CSwordModuleInfo::ModuleVersion) //store module version
            << QString(CACHE_FORMAT) //store BT version -- format may change
            << QString::number(s.version()) //store QDataStream version -- format may change
            << m_entries;
            f2.close();
        }
    }

    return m_entries;
}

void CSwordLexiconModuleInfo::testForStrongsKeys() {
    auto & m = module();
    m.setPosition(sword::TOP);
    m.increment();
    QString key = QString::fromUtf8(m.getKeyText());
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
    return;
}

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

