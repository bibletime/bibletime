/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/keys/cswordkey.h"

#include <QRegExp>
#include <QString>
#include <QTextCodec>
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordldkey.h"
#include "backend/keys/cswordtreekey.h"
#include "backend/keys/cswordversekey.h"

// Sword includes:
#include <swkey.h>
#include <swmodule.h>
#include <treekey.h>
#include <treekeyidx.h>
#include <utilstr.h>
#include <versekey.h>


const QTextCodec * CSwordKey::m_cp1252Codec = QTextCodec::codecForName("Windows-1252");

QString CSwordKey::rawText() {
    if (!m_module)
        return QString::null;

    if (dynamic_cast<sword::SWKey*>(this))
        m_module->module()->getKey()->setText( rawKey() );

    if (key().isNull())
        return QString::null;

    return QString::fromUtf8(m_module->module()->getRawEntry());
}

QString CSwordKey::renderedText(const CSwordKey::TextRenderType mode) {
    Q_ASSERT(m_module);

    sword::SWKey * const k = dynamic_cast<sword::SWKey *>(this);

    if (k) {
        sword::VerseKey * vk_mod = dynamic_cast<sword::VerseKey *>(m_module->module()->getKey());
        if (vk_mod)
            vk_mod->setIntros(true);

        m_module->module()->getKey()->setText(rawKey());

        if (m_module->type() == CSwordModuleInfo::Lexicon) {
            m_module->snap();
            /* In lexicons make sure that our key (e.g. 123) was successfully set to the module,
            i.e. the module key contains this key (e.g. 0123 contains 123) */

            if (sword::stricmp(m_module->module()->getKey()->getText(), rawKey())
                && !strstr(m_module->module()->getKey()->getText(), rawKey()))
            {
                qDebug("return an empty key for %s", m_module->module()->getKey()->getText());
                return QString::null;
            }
        }
    }

    if (key().isNull())
        return QString::null;

    bool DoRender = mode != ProcessEntryAttributesOnly;
    QString text = QString::fromUtf8(m_module->module()->RenderText(0, -1, DoRender));
    if (!DoRender)
        return QString::null;

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

        for (const QChar * c = text.constBegin(); c != text.constEnd(); c++) {
            if (c->toLatin1()) {
                ret.append(*c);
            } else {
                ret.append("&#").append(c->unicode()).append(";");
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
        return QString::null;

    if (dynamic_cast<sword::SWKey*>(this)) {
        char * buffer = new char[strlen(rawKey()) + 1];
        strcpy(buffer, rawKey());
        m_module->module()->getKey()->setText(buffer);
        delete [] buffer;
    }

    return QString::fromUtf8(m_module->module()->StripText());
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
        return 0;

    sword::SWKey * const key = module->module()->getKey();

    switch (module->type()) {

        case CSwordModuleInfo::Bible: // Fall through
        case CSwordModuleInfo::Commentary:

            Q_ASSERT(dynamic_cast<sword::VerseKey *>(key) != 0);
            return new CSwordVerseKey(static_cast<sword::VerseKey *>(key),
                                      module);

        case CSwordModuleInfo::Lexicon:

            return new CSwordLDKey(key, module);

        case CSwordModuleInfo::GenericBook:

            Q_ASSERT(dynamic_cast<sword::TreeKeyIdx *>(key) != 0);
            return new CSwordTreeKey(dynamic_cast<sword::TreeKeyIdx *>(key),
                                     module );

        default:

            return 0;

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
