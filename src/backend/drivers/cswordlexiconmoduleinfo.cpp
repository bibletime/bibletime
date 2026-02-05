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

#include "cswordlexiconmoduleinfo.h"

#include <QChar>
#include <QDataStream>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include "../../util/cp1252.h"
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


namespace {

// Change it once the format changed to make all systems rebuild their caches
constexpr quint8 const cacheFormat = 4;

} // Anonymous namespace

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
    QRegularExpression rx1(QStringLiteral("^[GH][0-9]+$"));
    if (rx1.match(key).hasMatch()) {
        m_hasStrongsKeys = true;
        m_hasLeadingStrongsLetter = true;
        m_strongsDigitsLength = key.length() - 1;
    } else {
        QRegularExpression rx2(QStringLiteral("^[0-9]+$"));
        if (rx2.match(key).hasMatch()) {
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

    QFile cacheFile(QStringLiteral("%1/%2")
                    .arg(DU::getUserCacheDir().absolutePath(), name()));

    QString const moduleVersion = config(CSwordModuleInfo::ModuleVersion);

    auto const writeCache = [this,&cacheFile,&moduleVersion]{
        qDebug() << "Writing cache file" << cacheFile.fileName();
        if (cacheFile.open(QIODevice::WriteOnly)) {
            QDataStream s(&cacheFile);
            s.setVersion(QDataStream::Qt_5_15);
            s << moduleVersion
              << QString::number(cacheFormat)
              << QString::number(QDataStream::Qt_DefaultCompiledVersion);
            s.setVersion(QDataStream::Qt_DefaultCompiledVersion);
            s << m_entries;
            cacheFile.close();

            if (s.status() == QDataStream::Ok) {
                qDebug() << "Cache file written successfully!";
            } else {
                qDebug() << "Failed to write cache file! Attempting to remove.";
                if (cacheFile.remove()) {
                    qDebug() << "Removed potentially corrupt cache.";
                } else {
                    qDebug() << "Failed to remove potentially corrupt cache!";
                }
            }
        } else {
            qDebug() << "Failed to open cache file for writing!";
        }
    };

    /*
     * Try the module's cache
     */
    if (cacheFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Reading lexicon cache for module" << name() << "...";
        QDataStream s(&cacheFile);
        s.setVersion(QDataStream::Qt_5_15);
        QString str;
        s >> str;
        qDebug() << "  module version:" << str;
        if (str == moduleVersion) {
            s >> str;
            qDebug() << "  cache version:" << str;
            bool isNumber = false;
            auto const cacheVersion = str.toInt(&isNumber);
            if (isNumber && cacheVersion <= cacheFormat) {
                s >> str;
                qDebug() << "  QDataStream version:" << str;
                auto const dataStreamVersion = str.toInt(&isNumber);
                if (isNumber
                    && dataStreamVersion
                        <= QDataStream::Qt_DefaultCompiledVersion)
                {
                    auto const resetVersion = cacheVersion >= 4;
                    qDebug() << "  QDataStream version reset:" << resetVersion;
                    if (resetVersion)
                        s.setVersion(dataStreamVersion);
                    s >> m_entries;
                    if (s.status() == QDataStream::Ok) {
                        qDebug() << "  entries read:" << m_entries.size();
                        cacheFile.close();

                        // Update cache format:
                        if (cacheVersion != cacheFormat)
                            writeCache();

                        return m_entries;
                    }
                    m_entries.clear();
                }
            }
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

    if (isUnicode()) {
        do {
            m_entries.append(QString::fromUtf8(m.getKeyText()));
            m.increment();
        } while (!m.popError());
    } else {
        do {
            m_entries.append(util::cp1252::toUnicode(m.getKeyText()));
            m.increment();
        } while (!m.popError());
    }

    m.setPosition(sword::TOP); // back to the first entry
    m.setSkipConsecutiveLinks(false);

    /// \todo Document why the following code is here:
    if (!m_entries.empty() && m_entries.front().simplified().isEmpty())
        m_entries.pop_front();

    writeCache();

    return m_entries;
}

bool CSwordLexiconModuleInfo::snap() const
{ return swordModule().getRawEntry(); }

bool CSwordLexiconModuleInfo:: hasStrongsKeys() const {
    return m_hasStrongsKeys;
}

QString CSwordLexiconModuleInfo::normalizeStrongsKey(const QString &key) const {
    if (auto const match =
                QRegularExpression(QStringLiteral("^([GH]?)0*([0-9]+?)$"))
                    .match(key);
        match.hasMatch())
    {
        auto const lang = match.capturedView(1);
        auto const digits = match.capturedView(2);

        qsizetype size = (digits.size() > m_strongsDigitsLength)
                          ? digits.size()
                          : m_strongsDigitsLength;
        auto numPaddingZeroesRequired = size - digits.size();
        if (m_hasLeadingStrongsLetter)
            size += lang.size();
        QString normalized;
        normalized.reserve(size);
        if (m_hasLeadingStrongsLetter)
            normalized.append(lang);
        while (numPaddingZeroesRequired--)
            normalized.append('0');
        normalized.append(digits);
        return normalized;
    }
    return key;
}

CSwordKey * CSwordLexiconModuleInfo::createKey() const
{ return new CSwordLDKey(swordModule().getKey(), this); }
