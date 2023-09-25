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

#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QString>
#include <string>
#include "../../util/btassert.h"
#include "../drivers/cswordmoduleinfo.h"

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
#include <utilstr.h>
#include <versekey.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#pragma GCC diagnostic pop


CSwordKey::~CSwordKey() noexcept = default;

QString CSwordKey::normalizedKey() const { return key(); }

QString CSwordKey::rawText() {
    if (!m_module)
        return QString();

    auto & m = m_module->swordModule();
    m.getKey()->setText( rawKey() );

    if (key().isNull())
        return QString();

    return QString::fromUtf8(m.getRawEntry());
}

QString CSwordKey::renderedText(const CSwordKey::TextRenderType mode) {
    BT_ASSERT(m_module);

    auto & m = m_module->swordModule();
    if (auto * const vk_mod = dynamic_cast<sword::VerseKey *>(m.getKey()))
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
    auto text = QString::fromUtf8(m.renderText(nullptr, -1, DoRender).c_str());
    if (!DoRender)
        return QString();

    // This is yucky, but if we want strong lexicon refs we have to do it here.
    if (m_module->type() == CSwordModuleInfo::Lexicon) {
        const QString t(text);
        static QRegularExpression const rx(
            QStringLiteral(R"PCRE((GREEK|HEBREW) for 0*([1-9]\d*))PCRE"));
        QRegularExpressionMatch match;
        int pos = 0;
        while ((pos = t.indexOf(rx, pos, &match)) != -1) {
            auto language = match.captured(1);
            auto langcode = language.at(0); // "G" or "H"
            auto number = match.captured(2);
            auto paddednumber = number.rightJustified(5, '0'); // Form 00123

            text.replace(
                QRegularExpression(
                    QStringLiteral(
                        R"PCRE((>[^<>]+))PCRE" // Avoid replacing inside tags
                        R"PCRE(\\b(0*%1)\\b)PCRE") // And span around 0's
                    .arg(std::move(number))),
                QStringLiteral("\\1<span lemma=\"%1%2\">"
                               "<a href=\"strongs://%3/%2\">\\2</a></span>")
                    .arg(std::move(langcode),
                         std::move(paddednumber),
                         std::move(language)));
            pos += match.capturedLength();
        }
    }
    return text;
}

QString CSwordKey::strippedText() {
    if (!m_module)
        return QString();

    auto & m = m_module->swordModule();
    m.getKey()->setText(std::string(rawKey()).c_str());

    return QString::fromUtf8(m.stripText());
}
