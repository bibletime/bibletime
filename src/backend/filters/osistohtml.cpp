/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "osistohtml.h"

#include <QString>
#include <swordxx/swmodule.h>
#include <swordxx/utilxml.h>
#include "../../util/btassert.h"
#include "../config/btconfig.h"
#include "../drivers/cswordmoduleinfo.h"
#include "../managers/clanguagemgr.h"
#include "../managers/cswordbackend.h"
#include "../managers/referencemanager.h"


Filters::OsisToHtml::OsisToHtml() : swordxx::OSISHTMLHREF() {
    setPassThruUnknownEscapeString(true); //the HTML widget will render the HTML escape codes

    addTokenSubstitute("inscription", "<span class=\"inscription\">");
    addTokenSubstitute("/inscription", "</span>");

    addTokenSubstitute("mentioned", "<span class=\"mentioned\">");
    addTokenSubstitute("/mentioned", "</span>");

//    addTokenSubstitute("divineName", "<span class=\"name\"><span class=\"divine\">");
//    addTokenSubstitute("/divineName", "</span></span>");

    /// \todo Move that down to the real tag handling, segs without the type morph would generate incorrect markup, as the end span is always inserted
//    addTokenSubstitute("seg type=\"morph\"", "<span class=\"morphSegmentation\">");
//    addTokenSubstitute("/seg", "</span>");

    // OSIS tables
    addTokenSubstitute("table", "<table>");
    addTokenSubstitute("/table", "</table>");
    addTokenSubstitute("row", "<tr>");
    addTokenSubstitute("/row", "</tr>");
    addTokenSubstitute("cell", "<td>");
    addTokenSubstitute("/cell", "</td>");

}

bool Filters::OsisToHtml::handleToken(std::string &buf, const char *token, swordxx::BasicFilterUserData *userData) {
    // manually process if it wasn't a simple substitution

    if (!substituteToken(buf, token)) {
        UserData* myUserData = dynamic_cast<UserData*>(userData);
        swordxx::SWModule* myModule = const_cast<swordxx::SWModule*>(myUserData->module); //hack

        swordxx::XMLTag tag(token);
        //     qWarning("found %s", token);
        const bool osisQToTick = ((!userData->module->getConfigEntry("OSISqToTick")) || (strcmp(userData->module->getConfigEntry("OSISqToTick"), "false")));

        if (tag.name().empty()) {
            return false;
        }

        // <div> tag
        if (tag.name() == "div") {
            if (tag.isEndTag()) {
                buf.append("</div>");
            } else {
                std::string type(tag.attribute("type"));
                if (type == "introduction") {
                    if (!tag.isEmpty())
                        buf.append("<div class=\"introduction\">");
                } else if (type == "chapter") {
                    if (!tag.isEmpty())
                        buf.append("<div class=\"chapter\" ></div>"); //don't open a div here, that would lead to a broken XML structure
                } else if (type == "x-p") {
                    buf.append("<br/>");
                } else if (type == "paragraph") {
                    if (!tag.attribute("sID").empty())
                        buf.append("<p>");
                    else if (!tag.attribute("eID").empty())
                        buf.append("</p>");
                } else {
                    buf.append("<div>");
                }
            }
        }
        else if (tag.name() == "w") {
            if ((!tag.isEmpty()) && (!tag.isEndTag())) { //start tag

                swordxx::XMLTag outTag("span");

                static auto const withoutFirstComponent =
                        [](std::string const & s) {
                            auto const val = std::strchr(s.c_str(), ':');
                            return val ? std::string(val + 1) : s;
                        };

                {
                    auto const attrib(tag.attribute("xlit"));
                    if (!attrib.empty())
                        outTag.setAttribute(
                                    "xlit",
                                    withoutFirstComponent(attrib).c_str());
                }

                {
                    auto const attrib(tag.attribute("gloss"));
                    if (!attrib.empty())
                        outTag.setAttribute(
                                    "gloss",
                                    withoutFirstComponent(attrib).c_str());
                }

                if (!tag.attribute("lemma").empty()) {
                    char splitChar = '|';
                    const int countSplit1 = tag.attributePartCount("lemma", '|');
                    const int countSplit2 = tag.attributePartCount("lemma", ' '); /// \todo not allowed, remove soon
                    int count = 0;

                    if (countSplit1 > countSplit2) { //| split char
                        splitChar = '|'; /// \todo not allowed, remove soon
                        count = countSplit1;
                    }
                    else {
                        splitChar = ' ';
                        count = countSplit2;
                    }

                    int i = (count > 1) ? 0 : -1;  // -1 for whole value cuz it's faster, but does the same thing as 0
                    std::string attrValue;

                    do {
                        if (!attrValue.empty())
                            attrValue.push_back('|');

                        auto const attrib(tag.attribute("lemma", i, splitChar));

                        if (i < 0) { // to handle our -1 condition
                            i = 0;
                        }

                        attrValue.append(withoutFirstComponent(attrib));
                    }
                    while (++i < count);

                    if (!attrValue.empty())
                        outTag.setAttribute("lemma", attrValue.c_str());
                }

                if (!tag.attribute("morph").empty()) {
                    char splitChar = '|';
                    const int countSplit1 = tag.attributePartCount("morph", '|');
                    const int countSplit2 = tag.attributePartCount("morph", ' '); /// \todo not allowed, remove soon
                    int count = 0;

                    if (countSplit1 > countSplit2) { //| split char
                        splitChar = '|';
                        count = countSplit1;
                    }
                    else {
                        splitChar = ' ';
                        count = countSplit2;
                    }

                    int i = (count > 1) ? 0 : -1;  // -1 for whole value cuz it's faster, but does the same thing as 0

                    std::string attrValue;

                    do {
                        if (!attrValue.empty())
                            attrValue.push_back('|');

                        auto const attrib(tag.attribute("morph", i, splitChar));

                        if (i < 0) {
                            i = 0; // to handle our -1 condition
                        }

                        auto val = std::strchr(attrib.c_str(), ':');

                        if (val) { //the prefix gives the modulename
                            //check the prefix
                            if (!std::strncmp("robinson:", attrib.c_str(), 9)) { //robinson
                                attrValue.append( "Robinson:" ); //work is not the same as Sword's module name
                                attrValue.append( val + 1 );
                            }
                            //strongs is handled by BibleTime
                            /*else if (!strncmp("strongs", attrib, val-atrrib)) {
                                attrValue.append( !strncmp(attrib, "x-", 2) ? attrib+2 : attrib );
                            }*/
                            else {
                                attrValue.append( !std::strncmp(attrib.c_str(), "x-", 2) ? attrib.c_str() + 2 : attrib.c_str());
                            }
                        }
                        else { //no prefix given
                            val = attrib.c_str();
                            const bool skipFirst = ((val[0] == 'T') && ((val[1] == 'H') || (val[1] == 'G')));
                            attrValue.append( skipFirst ? val + 1 : val );
                        }
                    }
                    while (++i < count);

                    if (!attrValue.empty())
                        outTag.setAttribute("morph", attrValue.c_str());
                }

                {
                    auto const attrib(tag.attribute("POS"));
                    if (!attrib.empty())
                        outTag.setAttribute(
                                    "pos",
                                    withoutFirstComponent(attrib).c_str());
                }

                buf.append( outTag.toString() );
            }
            else if (tag.isEndTag()) { // end or empty <w> tag
                buf.append("</span>");
            }
        }

        // <note> tag
        else if (tag.name() == "note") {
            if (!tag.isEndTag()) { //start tag
                auto const type(tag.attribute("type"));

                if (type == "crossReference") { //note containing cross references
                    myUserData->inCrossrefNote = true;
                    myUserData->noteType = UserData::CrossReference;

                    /*
                     * Do not count crossrefs as footnotes if they are displayed in the text. This will cause problems
                     * with footnote numbering when crossrefs are turned on/off.
                     * When accessing footnotes, crossrefs must be turned off in the filter so that they are not in the entry
                     * attributes of Sword.
                     *
                     * //myUserData->swordFootnote++; // cross refs count as notes, too
                     */

                    buf.append("<span class=\"crossreference\">");
                    auto const footnoteNumber(tag.attribute("swordFootnote"));
                    auto const footnoteBody(myUserData->entryAttributes["Footnote"][footnoteNumber]["body"]);
                    buf += myModule->renderText(footnoteBody.c_str());
                }

                /* else if (type == "explanation") {
                     }
                     */
                else if ((type == "strongsMarkup") || (type == "x-strongsMarkup")) {
                    /**
                    * leave strong's markup notes out, in the future we'll probably have
                    * different option filters to turn different note types on or off
                    */

                    myUserData->suspendTextPassThru = true;
                    myUserData->noteType = UserData::StrongsMarkup;
                }

                else {
                    //           qWarning("found note in %s", myUserData->key->getShortText());
                    buf.append(" <span class=\"footnote\" note=\"");
                    buf.append(myModule->getName());
                    buf.append(1u, '/');
                    buf.append(myUserData->key->getShortText());
                    buf.append(1u, '/');
                    buf.append( QString::number(myUserData->swordFootnote++).toUtf8().constData() ); //inefficient

                    auto const n(tag.attribute("n"));

                    buf.append("\">");
                    buf.append( (n.length() > 0) ? n.c_str() : "*" );
                    buf.append("</span> ");

                    myUserData->noteType = UserData::Footnote;
                    myUserData->suspendTextPassThru = true;
                }
            }
            else { //if (tag.isEndTag()) {
                BT_ASSERT(myUserData->noteType != UserData::Unknown);

                if (myUserData->noteType == UserData::CrossReference) {
                    buf.append("</span> ");
//                     myUserData->suspendTextPassThru = false;
                    myUserData->inCrossrefNote = false;
                }

                myUserData->noteType = UserData::Unknown;
                myUserData->suspendTextPassThru = false;
            }
        }
        else if (tag.name() == "reference") {
            if (!tag.isEndTag() && !tag.isEmpty()) {

                renderReference(tag.attribute("osisRef").c_str(), buf, myModule, myUserData);

            }
            else if (tag.isEndTag()) {
                buf.append("</a>");
            }
            else { // empty reference marker
                // -- what should we do?  nothing for now.
            }
        }

        // <l> is handled by OSISHTMLHref
        // <title>
        else if (tag.name() == "title") {
            if (!tag.isEndTag() && !tag.isEmpty()) {
                buf.append("<div class=\"sectiontitle\">");
            }
            else if (tag.isEndTag()) {
                buf.append("</div>");
            }
            else { // empty title marker
                // what to do?  is this even valid?
                buf.append("<br/>");
            }
        }

        // <hi> highlighted text
        else if (tag.name() == "hi") {
            auto const type(tag.attribute("type"));

            if ((!tag.isEndTag()) && (!tag.isEmpty())) {
                if (type == "bold") {
                    buf.append("<span class=\"bold\">");
                }
                else if (type == "illuminated") {
                    buf.append("<span class=\"illuminated\">");
                }
                else if (type == "italic") {
                    buf.append("<span class=\"italic\">");
                }
                else if (type == "line-through") {
                    buf.append("<span class=\"line-through\">");
                }
                else if (type == "normal") {
                    buf.append("<span class=\"normal\">");
                }
                else if (type == "small-caps") {
                    buf.append("<span class=\"small-caps\">");
                }
                else if (type == "underline") {
                    buf.append("<span class=\"underline\">");
                }
                else {
                    buf.append("<span>"); //don't break markup, </span> is inserted later
                }
            }
            else if (tag.isEndTag()) { //all hi replacements are html spans
                buf.append("</span>");
            }
        }

        //name
        else if (tag.name() == "name") {
            auto const type(tag.attribute("type"));

            if ((!tag.isEndTag()) && (!tag.isEmpty())) {
                if (type == "geographic") {
                    buf.append("<span class=\"name\"><span class=\"geographic\">");
                }
                else if (type == "holiday") {
                    buf.append("<span class=\"name\"><span class=\"holiday\">");
                }
                else if (type == "nonhuman") {
                    buf.append("<span class=\"name\"><span class=\"nonhuman\">");
                }
                else if (type == "person") {
                    buf.append("<span class=\"name\"><span class=\"person\">");
                }
                else if (type == "ritual") {
                    buf.append("<span class=\"name\"><span class=\"ritual\">");
                }
                else {
                    buf.append("<span class=\"name\"><span>");
                }
            }
            else if (tag.isEndTag()) { //all hi replacements are html spans
                buf.append("</span></span> ");
            }
        }
        else if (tag.name() == "transChange") {
            auto type(tag.attribute("type"));

            if (type.empty())
                type = tag.attribute("changeType");

            if ((!tag.isEndTag()) && (!tag.isEmpty())) {
                if (type == "added") {
                    buf.append("<span class=\"transchange\" title=\"");
                    buf.append(QObject::tr("Added text").toUtf8().constData());
                    buf.append("\"><span class=\"added\">");
                }
                else if (type == "amplified") {
                    buf.append("<span class=\"transchange\"><span class=\"amplified\">");
                }
                else if (type == "changed") {
                    buf.append("<span class=\"transchange\"><span class=\"changed\">");
                }
                else if (type == "deleted") {
                    buf.append("<span class=\"transchange\"><span class=\"deleted\">");
                }
                else if (type == "moved") {
                    buf.append("<span class=\"transchange\"><span class=\"moved\">");
                }
                else if (type == "tenseChange") {
                    buf.append("<span class=\"transchange\" title=\"");
                    buf.append(QObject::tr("Verb tense changed").toUtf8().constData());
                    buf.append("\"><span class=\"tenseChange\">");
                }
                else {
                    buf.append("<span class=\"transchange\"><span>");
                }
            }
            else if (tag.isEndTag()) { //all hi replacements are html spans
                buf.append("</span></span>");
            }
        }
        else if (tag.name() == "p") {
            if (tag.isEndTag())
                buf.append("</p>");
            else
                buf.append("<p>");

        }

        // <q> quote
        else if (tag.name() == "q") {
            //std::string type = tag.getAttribute("type");
            auto const who(tag.attribute("who"));
            auto const lev(tag.attribute("level"));
            int level = !lev.empty() ? std::atoi(lev.c_str()) : 1;
            auto const quoteMarker(tag.attribute("marker"));

            if ((!tag.isEndTag())) {
                if (!tag.isEmpty()) {
                    myUserData->quote.who = who;
                }

                if (!quoteMarker.empty()) {
                    buf.append(quoteMarker);
                }
                else if (osisQToTick) //alternate " and '
                    buf.push_back((level % 2) ? '\"' : '\'');

                if (who == "Jesus") {
                    buf.append("<span class=\"jesuswords\">");
                }
            }
            else if (tag.isEndTag()) {
                if (myUserData->quote.who == "Jesus") {
                    buf.append("</span>");
                }
                if (!quoteMarker.empty()) {
                    buf.append(quoteMarker);
                }
                else if (osisQToTick) { //alternate " and '
                    buf.push_back((level % 2) ? '\"' : '\'');
                }

                myUserData->quote.who = "";
            }
        }

        // abbr tag
        else if (tag.name() == "abbr") {
            if (!tag.isEndTag() && !tag.isEmpty()) {
                buf.append("<span class=\"abbreviation\" expansion=\"");
                buf.append(tag.attribute("expansion"));
                buf.append("\">");
            }
            else if (tag.isEndTag()) {
                buf.append("</span>");
            }
        }

        // <milestone> tag
        else if (tag.name() == "milestone") {
            auto const type(tag.attribute("type"));

            if ((type == "screen") || (type == "line")) {//line break
                buf.append("<br/>");
                userData->supressAdjacentWhitespace = true;
            }
            else if (type == "x-p") { //e.g. occurs in the KJV2006 module
                //buf.append("<br/>");
                buf.append(tag.attribute("marker"));
            }
        }
        //seg tag
        else if (tag.name() == "seg") {
            if (!tag.isEndTag() && !tag.isEmpty()) {
                if (tag.attribute("type") == "morph") {//line break
                    //This code is for WLC and MORPH (WHI)
                    swordxx::XMLTag outTag("span");
                    outTag.setAttribute("class", "morphSegmentation");
                    //Transfer the values to the span
                    //Problem: the data is in hebrew/aramaic, how to encode in HTML/BibleTime?
                    {
                        auto const attr(tag.attribute("lemma"));
                        if (!attr.empty())
                            outTag.setAttribute("lemma", attr.c_str());
                    }
                    {
                        auto const attr(tag.attribute("morph"));
                        if (!attr.empty())
                            outTag.setAttribute("morph", attr.c_str());
                    }
                    {
                        auto const attr(tag.attribute("homonym"));
                        if (!attr.empty())
                            outTag.setAttribute("homonym", attr.c_str());
                    }

                    buf.append(outTag.toString());
                    //buf.append("<span class=\"morphSegmentation\">");
                }
                else {
                    buf.append("<span>");
                }
            }
            else { // seg end tag
                buf.append("</span>");
            }
            //qWarning(QString("handled <seg> token. result: %1").arg(buf.c_str()).latin1());
        }

        //divine name, don't use simple tag replacing because it may have attributes
        else if (tag.name() == "divineName") {
            if (!tag.isEndTag()) {
                buf.append("<span class=\"name\"><span class=\"divine\">");
            }
            else { //all hi replacements are html spans
                buf.append("</span></span>");
            }
        }

        else { //all tokens handled by OSISHTMLHref will run through the filter now
            return swordxx::OSISHTMLHREF::handleToken(buf, token, userData);
        }
    }

    return false;
}

void Filters::OsisToHtml::renderReference(const char *osisRef, std::string &buf, swordxx::SWModule *myModule, UserData *myUserData) {
    QString ref( osisRef );
    QString hrefRef( ref );
    //BT_ASSERT(!ref.isEmpty()); checked later

    if (!ref.isEmpty()) {
        //find out the mod, using the current module makes sense if it's a bible or commentary because the refs link into a bible by default.
        //If the osisRef is something like "ModuleID:key comes here" then the
        // modulename is given, so we'll use that one

        CSwordModuleInfo* mod = CSwordBackend::instance()->findSwordModuleByPointer(myModule);
        //BT_ASSERT(mod); checked later
        if (!mod || (mod->type() != CSwordModuleInfo::Bible
                     && mod->type() != CSwordModuleInfo::Commentary)) {

            mod = btConfig().getDefaultSwordModuleByType("standardBible");
        }

        // BT_ASSERT(mod); There's no necessarily a module or standard Bible

        //if the osisRef like "GerLut:key" contains a module, use that
        int pos = ref.indexOf(":");

        if ((pos >= 0) && ref.at(pos - 1).isLetter() && ref.at(pos + 1).isLetter()) {
            QString newModuleName = ref.left(pos);
            hrefRef = ref.mid(pos + 1);

            if (CSwordBackend::instance()->findModuleByName(newModuleName)) {
                mod = CSwordBackend::instance()->findModuleByName(newModuleName);
            }
        }

        if (mod) {
            ReferenceManager::ParseOptions const options(
                    mod->name(),
                    QString::fromStdString(myUserData->key->getText()),
                    QString::fromStdString(myModule->getLanguage()));

            buf.append("<a href=\"")
               .append( //create the hyperlink with key and mod
                    ReferenceManager::encodeHyperlink(
                        mod->name(),
                        ReferenceManager::parseVerseReference(hrefRef, options),
                        ReferenceManager::typeFromModule(mod->type())
                    ).toUtf8().constData()
                )
               .append("\" crossrefs=\"")
               .append(ReferenceManager::parseVerseReference(ref, options).toUtf8().constData()) //ref must contain the osisRef module marker if there was any
               .append("\">");
        }
        // should we add something if there were no referenced module available?
    }
}

