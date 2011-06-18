/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/drivers/cswordlexiconmoduleinfo.h"

#include <QFile>
#include <QDataStream>
#include <QTextCodec>
#include <QDebug>

#include "util/directory.h"

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

    // Initialize cache:
    //Check for buggy modules! They will not be loaded any more.
    if ( name() == QString("ZhEnglish")) {
        qWarning() << "Module ZhEnglish is buggy and will not be loaded.";
        return m_entries;
    }
    if ( name() == QString("EReo_en")) {
        qWarning() << "Module EReo_en is buggy and will not be loaded.";
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

    module()->setSkipConsecutiveLinks(true);
    module()->setPosition(sword::TOP);
    snap(); //snap to top entry

    do {
        if ( isUnicode() ) {
            m_entries.append(QString::fromUtf8(module()->getKeyText()));
        }
        else {
            //for latin1 modules use fromLatin1 because of speed
            QTextCodec* codec = QTextCodec::codecForName("Windows-1252");
            m_entries.append(codec->toUnicode(module()->getKeyText()));
        }

        module()->increment();
    } while (!module()->Error());

    module()->setPosition(sword::TOP); // back to the first entry
    module()->setSkipConsecutiveLinks(false);

    if (m_entries.count()) {
        m_entries.first().simplified();

        if (m_entries.first().trimmed().isEmpty()) {
            m_entries.erase( m_entries.begin() );
        }
    }

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
