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

#include "thmltohtml.h"

#include <QRegExp>
#include <QString>
#include <QTextCodec>
#include <QUrl>
#include "../../util/btassert.h"
#include "../config/btconfig.h"
#include "../drivers/cswordmoduleinfo.h"
#include "../managers/clanguagemgr.h"
#include "../managers/cswordbackend.h"
#include "../managers/referencemanager.h"

// Sword includes:
#include <swmodule.h>
#include <utilstr.h>
#include <utilxml.h>
#include <versekey.h>


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

    CSwordModuleInfo* m = CSwordBackend::instance()->findModuleByName( module->getName() );

    if (m && !(m->has(CSwordModuleInfo::lemmas) || m->has(CSwordModuleInfo::strongNumbers))) { //only parse if the module has strongs or lemmas
        return 1;
    }

    QString result;

    QString t = QString::fromUtf8(buf.c_str());
    QRegExp tag("([.,;]?<sync[^>]+(type|value)=\"([^\"]+)\"[^>]+(type|value)=\"([^\"]+)\"([^<]*)>)+");

    QStringList list;
    int lastMatchEnd = 0;
    int pos = tag.indexIn(t, 0);

    if (pos == -1) { //no strong or morph code found in this text
        return 1; //WARNING: Return already here
    }

    while (pos != -1) {
        list.append(t.mid(lastMatchEnd, pos + tag.matchedLength() - lastMatchEnd));

        lastMatchEnd = pos + tag.matchedLength();
        pos = tag.indexIn(t, pos + tag.matchedLength());
    }

    if (!t.right(t.length() - lastMatchEnd).isEmpty()) {
        list.append(t.right(t.length() - lastMatchEnd));
    }

    tag = QRegExp("<sync[^>]+(type|value|class)=\"([^\"]+)\"[^>]+(type|value|class)=\"([^\"]+)\"[^>]+((type|value|class)=\"([^\"]+)\")*([^<]*)>");

    for (QStringList::iterator it = list.begin(); it != list.end(); ++it) {
        QString e( *it );

        // pass text ahead of <sync> stright through
        int pos2 = tag.indexIn(e, 0);
        if (pos2 >= 0) {
            result.append(e.left(pos2));
            e = e.remove(0, pos2);
        }

        // parse <sync> and change to <span>
        bool hasLemmaAttr = false;
        bool hasMorphAttr = false;

        int pos = tag.indexIn(e, 0);
        bool insertedTag = false;
        QString value;
        QString valueClass;

        while (pos != -1) {
            bool isMorph = false;
            bool isStrongs = false;
            value = QString();
            valueClass = QString();

            // check 3 attribute/value pairs

            for (int i = 1; i < 6; i += 2) {
                if (i > 4)
                    i++;

                if (tag.cap(i) == "type") {
                    isMorph   = (tag.cap(i + 1) == "morph");
                    isStrongs = (tag.cap(i + 1) == "Strongs");
                }
                else if (tag.cap(i) == "value") {
                    value = tag.cap(i + 1);
                }
                else if (tag.cap(i) == "class") {
                    valueClass = tag.cap(i + 1);
                }
            }

            // prepend the class qualifier to the value
            if (!valueClass.isEmpty()) {
                value = valueClass + ":" + value;
                //     value.append(":").append(value);
            }

            if (value.isEmpty()) {
                break;
            }

            //insert the span
            if (!insertedTag) {
                e.replace(pos, tag.matchedLength(), "</span>");
                pos += 7;

                QString rep = QString("<span lemma=\"").append(value).append("\">");
                int startPos = 0;
                QChar c = e[startPos];

                while ((startPos < pos) && (c.isSpace() || c.isPunct())) {
                    ++startPos;
                    c = e[startPos];
                }

                hasLemmaAttr = isStrongs;
                hasMorphAttr = isMorph;

                e.insert( startPos, rep );
                pos += rep.length();
            }
            else { //add the attribute to the existing tag
                e.remove(pos, tag.matchedLength());

                if ((!isMorph && hasLemmaAttr) || (isMorph && hasMorphAttr)) { //we append another attribute value, e.g. 3000 gets 3000|5000
                    //search the existing attribute start
                    QRegExp attrRegExp( isMorph ? "morph=\".+(?=\")" : "lemma=\".+(?=\")" );
                    attrRegExp.setMinimal(true);
                    const int foundAttrPos = e.indexOf(attrRegExp, pos);

                    if (foundAttrPos != -1) {
                        e.insert(foundAttrPos + attrRegExp.matchedLength(), QString("|").append(value));
                        pos += value.length() + 1;

                        hasLemmaAttr = !isMorph;
                        hasMorphAttr = isMorph;
                    }
                }
                else { //attribute was not yet inserted
                    const int attrPos = e.indexOf(QRegExp("morph=|lemma="), 0);

                    if (attrPos >= 0) {
                        QString attr;
                        attr.append(isMorph ? "morph" : "lemma").append("=\"").append(value).append("\" ");
                        e.insert(attrPos, attr);

                        hasMorphAttr = isMorph;
                        hasLemmaAttr = !isMorph;

                        pos += attr.length();
                    }
                }
            }

            insertedTag = true;
            pos = tag.indexIn(e, pos);
        }

        result.append( e );
    }

    if (list.count())
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
                            ReferenceManager::ParseOptions options(
                                    mod->name(),
                                    // current module key:
                                    QString::fromUtf8(myUserData->key->getText()),
                                    myModule->getLanguage());

                            //it's ok to split the reference, because to descriptive text is given
                            bool insertSemicolon = false;
                            buf.append("<span class=\"crossreference\">");
                            QStringList const refs(
                                QString::fromUtf8(
                                    myUserData->lastTextNode.c_str()).split(";"));
                            QString oldRef; // the previous reference to use as a base for the next refs
                            for (QStringList::const_iterator it(refs.begin());
                                 it != refs.end();
                                 ++it)
                            {
                                if (!oldRef.isEmpty())
                                    options.refBase = oldRef; // Use the last ref as a base, e.g. Rom 1,2-3, when the next ref is only 3:3-10

                                // Use the parsed result as the base for the next ref:
                                oldRef = ReferenceManager::parseVerseReference(
                                                (*it),
                                                options);

                                // Prepend a ref divider if we're after the first one
                                if (insertSemicolon)
                                    buf.append("; ");

                                buf.append("<a href=\"")
                                   .append(
                                        ReferenceManager::encodeHyperlink(
                                            mod->name(),
                                            oldRef,
                                            ReferenceManager::typeFromModule(mod->type())
                                        ).toUtf8().constData()
                                    )
                                   .append("\" crossrefs=\"")
                                   .append(oldRef.toUtf8().constData())
                                   .append("\">")
                                   .append(it->toUtf8().constData())
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

                    CSwordModuleInfo * mod = btConfig().getDefaultSwordModuleByType("standardBible");
                    if (! mod)
                        mod = CSwordBackend::instance()->findFirstAvailableModule(CSwordModuleInfo::Bible);

                    if (mod)
                    {
                        ;
                        BT_ASSERT(tag.getAttribute("passage"));
                        QString const completeRef(
                                ReferenceManager::parseVerseReference(
                                    QString::fromUtf8(
                                            tag.getAttribute("passage")),
                                    ReferenceManager::ParseOptions(
                                        mod->name(),
                                        QString::fromUtf8(
                                                myUserData->key->getText()),
                                        myModule->getLanguage())));
                        buf.append("<span class=\"crossreference\">")
                           .append("<a href=\"")
                           .append(
                                ReferenceManager::encodeHyperlink(
                                    mod->name(),
                                    completeRef,
                                    ReferenceManager::typeFromModule(
                                        mod->type())
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

            buf.append("<img src=\"")
               .append(
                    QUrl::fromLocalFile(
                        QTextCodec::codecForLocale()->toUnicode(
                            myUserData->module->getConfigEntry(
                                "AbsoluteDataPath")
                        ).append('/').append(QString::fromUtf8(value))
                    ).toString().toUtf8().constData())
               .append("\" />");
        } else { // Let unknown token pass thru:
            return sword::ThMLHTML::handleToken(buf, token, userData);
        }
    }
    return true;
}

} // namespace Filtes
