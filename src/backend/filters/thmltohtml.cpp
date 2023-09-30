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

#include "thmltohtml.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QUrl>
#include <utility>
#include "../../util/btassert.h"
#include "../config/btconfig.h"
#include "../drivers/cswordmoduleinfo.h"
#include "../managers/cswordbackend.h"
#include "../managers/referencemanager.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsuggest-destructor-override"
#endif
#include <swmodule.h>
#include <utilstr.h>
#include <utilxml.h>
#include <versekey.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#pragma GCC diagnostic pop


namespace Filters {

ThmlToHtml::ThmlToHtml() {
    setEscapeStringCaseSensitive(true);
    setPassThruUnknownEscapeString(true); //the HTML widget will render the HTML escape codes

    setTokenStart("<");
    setTokenEnd(">");
    setTokenCaseSensitive(true);

    addTokenSubstitute("/foreign", "</span>");

    removeTokenSubstitute("note");
    removeTokenSubstitute("/note");
}

char ThmlToHtml::processText(sword::SWBuf &buf, const sword::SWKey *key,
                             const sword::SWModule *module)
{
    sword::ThMLHTML::processText(buf, key, module);

    if (auto * const m =
                CSwordBackend::instance().findModuleByName(module->getName()))
    {
        // only parse if the module has strongs or lemmas:
        if (!m->has(CSwordModuleInfo::lemmas)
            && !m->has(CSwordModuleInfo::strongNumbers))
            return 1;
    }

    QStringList list;
    {
        auto t = QString::fromUtf8(buf.c_str());
        {
            static QRegularExpression const tag(
                QStringLiteral(R"PCRE(([.,;]?<sync[^>]+(type|value)=)PCRE"
                               R"PCRE("([^"]+)"[^>]+(type|value)=)PCRE"
                               R"PCRE("([^"]+)"([^<]*)>)+)PCRE"));
            QRegularExpressionMatch match;
            auto pos = t.indexOf(tag, 0, &match);
            if (pos == -1) //no strong or morph code found in this text
                return 1; //WARNING: Return already here
            do {
                auto const partLength = pos + match.capturedLength();
                list.append(t.left(partLength));
                t.remove(0, partLength);
                pos = t.indexOf(tag, 0, &match);
            } while (pos != -1);
        }

        // Append the trailing text to the list:
        if (!t.isEmpty())
            list.append(std::move(t));
    }

    static QRegularExpression const tag(
        QStringLiteral(R"PCRE(<sync[^>]+(type|value|class)="([^"]+)"[^>]+)PCRE"
                       R"PCRE((type|value|class)="([^"]+)"[^>]+)PCRE"
                       R"PCRE(((type|value|class)="([^"]+)")*([^<]*)>)PCRE"));
    QString result;
    for (auto & e : list) {

        // pass text ahead of <sync> stright through
        if (auto const pos = e.indexOf(tag); pos > 0) {
            result.append(e.left(pos));
            e.remove(0, pos);
        }

        // parse <sync> and change to <span>
        bool hasLemmaAttr = false;
        bool hasMorphAttr = false;

        QRegularExpressionMatch match;
        auto pos = e.indexOf(tag, 0, &match);
        bool insertedTag = false;

        while (pos != -1) {
            bool isMorph = false;
            bool isStrongs = false;
            QString value;
            QString valueClass;

            // check 3 attribute/value pairs

            for (int i = 1; i < 6; i += 2) {
                if (i > 4)
                    i++;

                auto const attrName = match.captured(i);
                auto const attrValue = match.captured(i + 1);
                if (attrName == QStringLiteral("type")) {
                    isMorph   = (attrValue == QStringLiteral("morph"));
                    isStrongs = (attrValue == QStringLiteral("Strongs"));
                } else if (attrName == QStringLiteral("value")) {
                    value = attrValue;
                } else if (attrName == QStringLiteral("class")) {
                    valueClass = attrValue;
                } else { // optional 3rd attribute pair is not present:
                    BT_ASSERT(attrName.isEmpty());
                }
            }

            // prepend the class qualifier to the value
            if (!valueClass.isEmpty())
                value = QStringLiteral("%1:%2").arg(valueClass, value);

            if (value.isEmpty()) {
                break;
            }

            //insert the span
            if (!insertedTag) {
                e.replace(pos, match.capturedLength(), QStringLiteral("</span>"));
                pos += 7;

                auto rep = QStringLiteral("<span lemma=\"%1\">").arg(value);
                int startPos = 0;
                QChar c = e[startPos];

                while ((startPos < pos) && (c.isSpace() || c.isPunct())) {
                    ++startPos;
                    c = e[startPos];
                }

                hasLemmaAttr = isStrongs;
                hasMorphAttr = isMorph;

                pos += rep.length();
                e.insert(startPos, std::move(rep));
            }
            else { //add the attribute to the existing tag
                e.remove(pos, match.capturedLength());

                if ((!isMorph && hasLemmaAttr) || (isMorph && hasMorphAttr)) { //we append another attribute value, e.g. 3000 gets 3000|5000
                    //search the existing attribute start
                    auto const & attrRegExp =
                        [isMorph]{
                            if (isMorph) {
                                static QRegularExpression const re(
                                    QStringLiteral("morph=\".+?(?=\")"));
                                return re;
                            } else {
                                static QRegularExpression const re(
                                    QStringLiteral("lemma=\".+?(?=\")"));
                                return re;
                            }
                        }();
                    QRegularExpressionMatch match;
                    const int foundAttrPos = e.indexOf(attrRegExp, pos, &match);

                    if (foundAttrPos != -1) {
                        e.insert(foundAttrPos + match.capturedLength(),
                                 QStringLiteral("|%1").arg(value));
                        pos += value.length() + 1;

                        hasLemmaAttr = !isMorph;
                        hasMorphAttr = isMorph;
                    }
                }
                else { //attribute was not yet inserted
                    static QRegularExpression const re(
                        QStringLiteral("morph=|lemma="));
                    const int attrPos = e.indexOf(re, 0);

                    if (attrPos >= 0) {
                        hasMorphAttr = isMorph;
                        hasLemmaAttr = !isMorph;

                        auto attr = QStringLiteral("%1=\"%2\" ")
                                    .arg(isMorph
                                         ? QStringLiteral("morph")
                                         : QStringLiteral("lemma"),
                                         value);
                        pos += attr.length();
                        e.insert(attrPos, std::move(attr)); /// \bug e.replace() instead?
                    }
                }
            }

            insertedTag = true;
            pos = e.indexOf(tag, pos, &match);
        }

        result.append(std::move(e));
    }

    if (!list.isEmpty())
        buf = result.toUtf8();

    return 1;
}


bool ThmlToHtml::handleToken(sword::SWBuf &buf, const char *token,
                             sword::BasicFilterUserData *userData)
{
    if (!substituteToken(buf, token) && !substituteEscapeString(buf, token)) {
        sword::XMLTag const tag(token);
        BT_ASSERT(dynamic_cast<UserData *>(userData));
        UserData * const myUserData = static_cast<UserData *>(userData);
        // Hack to be able to call stuff like Lang():
        sword::SWModule const * const myModule =
                const_cast<sword::SWModule *>(myUserData->module);
        char const * const tagName = tag.getName();
        if (!tagName) // unknown tag, pass through:
            return sword::ThMLHTML::handleToken(buf, token, userData);
        if (!sword::stricmp(tagName, "foreign")) {
            // A text part in another language, we have to set the right font

            if (const char * const tagLang = tag.getAttribute("lang"))
                buf.append("<span class=\"foreign\" lang=\"")
                   .append(tagLang)
                   .append("\">");
        } else if (!sword::stricmp(tagName, "sync")) {
            // If Morph or Strong or Lemma:
            if (const char * const tagType = tag.getAttribute("type"))
                if (!sword::stricmp(tagType, "morph")
                    || !sword::stricmp(tagType, "Strongs")
                    || !sword::stricmp(tagType, "lemma"))
                    buf.append('<').append(token).append('>');
        } else if (!sword::stricmp(tagName, "note")) { // <note> tag
            if (!tag.isEmpty()) {
                if (!tag.isEndTag()) {
                    buf.append(" <span class=\"footnote\" note=\"")
                       .append(myModule->getName())
                       .append('/')
                       .append(myUserData->key->getShortText())
                       .append('/')
                       .append(QString::number(myUserData->swordFootnote).toUtf8().constData())
                       .append("\">*</span> ");

                    myUserData->swordFootnote++;
                    myUserData->suspendTextPassThru = true;
                    myUserData->inFootnoteTag = true;
                } else if (tag.isEndTag()) { // end tag
                    // buf += ")</span>";
                    myUserData->suspendTextPassThru = false;
                    myUserData->inFootnoteTag = false;
                }
            }
        } else if (!sword::stricmp(tagName, "scripRef")) { // a scripRef
            // scrip refs which are embeded in footnotes may not be displayed!

            if (!myUserData->inFootnoteTag) {
                if (tag.isEndTag()) {
                    if (myUserData->inscriptRef) { // like "<scripRef passage="John 3:16">See John 3:16</scripRef>"
                        buf.append("</a></span>");

                        myUserData->inscriptRef = false;
                        myUserData->suspendTextPassThru = false;
                    } else { // like "<scripRef>John 3:16</scripRef>"
                        if (CSwordModuleInfo const * const mod =
                                btConfig().getDefaultSwordModuleByType(
                                        "standardBible"))
                        {
                            ReferenceManager::ParseOptions options{
                                    mod->name(),
                                    // current module key:
                                    QString::fromUtf8(myUserData->key->getText()),
                                    myModule->getLanguage()};

                            //it's ok to split the reference, because to descriptive text is given
                            bool insertSemicolon = false;
                            buf.append("<span class=\"crossreference\">");
                            QStringList const refs(
                                QString::fromUtf8(
                                    myUserData->lastTextNode.c_str()).split(
                                            ';'));
                            QString oldRef; // the previous reference to use as a base for the next refs
                            for (auto const & ref : refs) {
                                if (!oldRef.isEmpty())
                                    options.refBase = oldRef; // Use the last ref as a base, e.g. Rom 1,2-3, when the next ref is only 3:3-10

                                // Use the parsed result as the base for the next ref:
                                oldRef = ReferenceManager::parseVerseReference(
                                                ref,
                                                options);

                                // Prepend a ref divider if we're after the first one
                                if (insertSemicolon)
                                    buf.append("; ");

                                buf.append("<a href=\"")
                                   .append(
                                        ReferenceManager::encodeHyperlink(
                                            *mod,
                                            oldRef
                                        ).toUtf8().constData()
                                    )
                                   .append("\" crossrefs=\"")
                                   .append(oldRef.toUtf8().constData())
                                   .append("\">")
                                   .append(ref.toUtf8().constData())
                                   .append("</a>");
                                insertSemicolon = true;
                            }
                            buf.append("</span>"); //crossref end
                        }
                        myUserData->suspendTextPassThru = false;
                    }
                } else if (tag.getAttribute("passage") ) {
                    // The passage was given as a parameter value
                    myUserData->inscriptRef = true;
                    myUserData->suspendTextPassThru = false;

                    auto * mod = btConfig().getDefaultSwordModuleByType(
                                     QStringLiteral("standardBible"));
                    if (! mod)
                        mod = CSwordBackend::instance().findFirstAvailableModule(CSwordModuleInfo::Bible);

                    if (mod) {
                        BT_ASSERT(tag.getAttribute("passage"));
                        QString const completeRef(
                                ReferenceManager::parseVerseReference(
                                    QString::fromUtf8(
                                            tag.getAttribute("passage")),
                                    ReferenceManager::ParseOptions{
                                        mod->name(),
                                        QString::fromUtf8(
                                                myUserData->key->getText()),
                                        myModule->getLanguage()}));
                        buf.append("<span class=\"crossreference\">")
                           .append("<a href=\"")
                           .append(
                                ReferenceManager::encodeHyperlink(
                                    *mod,
                                    completeRef
                                ).toUtf8().constData()
                            )
                           .append("\" crossrefs=\"")
                           .append(completeRef.toUtf8().constData())
                           .append("\">");
                    } else {
                        buf.append("<span><a>");
                    }
                } else { // We're starting a scripRef like "<scripRef>John 3:16</scripRef>"
                    myUserData->inscriptRef = false;
                    /* Let's stop text from going to output, the text get's
                       added in the -tag handler: */
                    myUserData->suspendTextPassThru = true;
                }
            }
        } else if (!sword::stricmp(tagName, "div")) {
            if (tag.isEndTag()) {
                buf.append("</div>");
            } else if (char const * const tagClass = tag.getAttribute("class")){
                if (!sword::stricmp(tagClass, "sechead") ) {
                    buf.append("<div class=\"sectiontitle\">");
                } else if (!sword::stricmp(tagClass, "title")) {
                    buf.append("<div class=\"booktitle\">");
                }
            }
        } else if (!sword::stricmp(tagName, "img") && tag.getAttribute("src")) {
            const char * value = tag.getAttribute("src");

            if (value[0] == '/')
                value++; //strip the first /

            if (!myUserData->absolutePath.has_value()) {
                auto const * const absoluteDataPath =
                        myUserData->module->getConfigEntry("AbsoluteDataPath");
                myUserData->absolutePath.emplace(
                        myUserData->module->isUnicode()
                        ? QString::fromUtf8(absoluteDataPath)
                        : QString::fromLatin1(absoluteDataPath));
            }

            buf.append("<img src=\"")
               .append(
                    QUrl::fromLocalFile(
                        QStringLiteral("%1/%2").arg(
                            *myUserData->absolutePath,
                            QString::fromUtf8(value))
                    ).toString().toUtf8().constData())
               .append("\" />");
        } else { // Let unknown token pass thru:
            return sword::ThMLHTML::handleToken(buf, token, userData);
        }
    }
    return true;
}

} // namespace Filtes
