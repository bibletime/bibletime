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

#include "cswordkey.h"

#include <QRegExp>
#include <QString>
#include <QTextCodec>
#include <string>
#include "../../util/btassert.h"
#include "../drivers/cswordmoduleinfo.h"
#include "cswordldkey.h"
#include "cswordtreekey.h"
#include "cswordversekey.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <swkey.h>
#include <swmodule.h>
#include <treekey.h>
#include <treekeyidx.h>
#include <utilstr.h>
#include <versekey.h>
#pragma GCC diagnostic pop


const QTextCodec * CSwordKey::m_cp1252Codec = QTextCodec::codecForName("Windows-1252");

CSwordKey::~CSwordKey() noexcept {
    delete m_beforeChangedSignaller.data();
    delete m_afterChangedSignaller.data();
}

QString CSwordKey::normalizedKey() const { return key(); }

void CSwordKey::setLocale(char const * const locale)
{ asSwordKey().setLocale(locale); }

QString CSwordKey::rawText() {
    if (!m_module)
        return QString();

    auto & m = m_module->module();
    m.getKey()->setText( rawKey() );

    if (key().isNull())
        return QString();

    return QString::fromUtf8(m.getRawEntry());
}

QString CSwordKey::renderedText(const CSwordKey::TextRenderType mode) {
    BT_ASSERT(m_module);

    auto & m = m_module->module();
    sword::VerseKey * vk_mod = dynamic_cast<sword::VerseKey *>(m.getKey());
    if (vk_mod)
        vk_mod->setIntros(true);

    m.getKey()->setText(rawKey());

    if (m_module->type() == CSwordModuleInfo::Lexicon) {
        m_module->snap();
        /* In lexicons make sure that our key (e.g. 123) was successfully set to the module,
        i.e. the module key contains this key (e.g. 0123 contains 123) */

        if (sword::stricmp(m.getKey()->getText(), rawKey())
            && !strstr(m.getKey()->getText(), rawKey()))
        {
            qDebug("return an empty key for %s", m.getKey()->getText());
            return QString();
        }
    }

    if (key().isNull())
        return QString();

    bool DoRender = mode != ProcessEntryAttributesOnly;
    QString text = QString::fromUtf8(m.renderText(nullptr, -1, DoRender));
    if (!DoRender)
        return QString();

    // This is yucky, but if we want strong lexicon refs we have to do it here.
    if (m_module->type() == CSwordModuleInfo::Lexicon) {
        const QString t(text);
        const QRegExp rx("(GREEK|HEBREW) for 0*([1-9]\\d*)");    // ignore 0's before number
        int pos = 0;
        while ((pos = rx.indexIn(t, pos)) != -1) {
            const QString language = rx.cap(1);
            const QString langcode = QString(language.at(0));    // "G" or "H"
            const QString number = rx.cap(2);
            const QString paddednumber = number.rightJustified(5, '0');    // Form 00123

            text.replace(
                QRegExp(QString(
                            "(>[^<>]+)"            // Avoid replacing inside tags
                            "\\b(0*%1)\\b").arg(number)),    // And span around 0's
                QString("\\1<span lemma=\"%1%2\"><a href=\"strongs://%3/%4\">\\2</a></span>")
                    .arg(langcode, paddednumber, language, paddednumber)
            );
            pos += rx.matchedLength();
        }
    }

    if (mode == HTMLEscaped) {
        /*
          Here we encode all non-latin1 characters as HTML unicode entities
          in the form &#<decimal unicode value here>;
        */
        QString ret;

        // Reserve characters to reduce number of memory allocations:
        ret.reserve(text.size());

        for (auto const & c : text) {
            if (c.toLatin1()) {
                ret.append(c);
            } else {
                ret.append("&#").append(c.unicode()).append(";");
            }
        }

        return ret;
    }
    else {
        return text;
    }
}

QString CSwordKey::strippedText() {
    if (!m_module)
        return QString();

    auto & m = m_module->module();
    m.getKey()->setText(std::string(rawKey()).c_str());

    return QString::fromUtf8(m.stripText());
}

void CSwordKey::emitBeforeChanged() {
    if (!m_beforeChangedSignaller.isNull())
        m_beforeChangedSignaller->emitSignal();
}

void CSwordKey::emitAfterChanged() {
    if (!m_afterChangedSignaller.isNull())
        m_afterChangedSignaller->emitSignal();
}

CSwordKey * CSwordKey::createInstance(const CSwordModuleInfo * module) {
    if (!module)
        return nullptr;

    sword::SWKey * const key = module->module().getKey();

    switch (module->type()) {

        case CSwordModuleInfo::Bible: // Fall through
        case CSwordModuleInfo::Commentary:

            BT_ASSERT(dynamic_cast<sword::VerseKey *>(key));
            return new CSwordVerseKey(static_cast<sword::VerseKey *>(key),
                                      module);

        case CSwordModuleInfo::Lexicon:

            return new CSwordLDKey(key, module);

        case CSwordModuleInfo::GenericBook:

            BT_ASSERT(dynamic_cast<sword::TreeKeyIdx *>(key));
            return new CSwordTreeKey(dynamic_cast<sword::TreeKeyIdx *>(key),
                                     module );

        default:

            return nullptr;

    }
}

const BtSignal * CSwordKey::beforeChangedSignaller() {
    if (m_beforeChangedSignaller.isNull())
        m_beforeChangedSignaller = new BtSignal();

    return m_beforeChangedSignaller;
}

const BtSignal * CSwordKey::afterChangedSignaller() {
    if (m_afterChangedSignaller.isNull())
        m_afterChangedSignaller = new BtSignal();

    return m_afterChangedSignaller;
}
