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

#include "teitohtml.h"

#include <QString>
#include <string_view>
#include "../config/btconfig.h"
#include "../drivers/cswordmoduleinfo.h"
#include "../managers/cswordbackend.h"
#include "../managers/referencemanager.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <swbuf.h>
#include <swmodule.h>
#include <utilxml.h>
#pragma GCC diagnostic pop


namespace Filters {

TeiToHtml::TeiToHtml()
    : sword::TEIHTMLHREF()
{
    setPassThruUnknownEscapeString(true); // the HTML widget will render the HTML escape codes
}

bool TeiToHtml::handleToken(sword::SWBuf &buf, const char *token,
                            sword::BasicFilterUserData *userData)
{
    using namespace std::literals;
    // manually process if it wasn't a simple substitution

    if (!substituteToken(buf, token)) {

        sword::XMLTag const tag(token);
        std::string_view const tagName(tag.getName());

        if (tagName == "ref"sv) {

            if (!tag.isEndTag() && !tag.isEmpty()) {

                const char * attribute = tag.getAttribute("osisRef");
                if (attribute != nullptr)
                    renderReference(attribute, buf, userData);
                else {
                    attribute = tag.getAttribute("target");
                    renderTargetReference(attribute, buf, userData);
                }

            }
            else if (tag.isEndTag()) {
                buf.append("</a>");
            }
            else { // empty reference marker
                // -- what should we do?  nothing for now.
            }
        }
        else if (tagName == "hi"sv) { // <hi> highlighted text
            const sword::SWBuf type = tag.getAttribute("rend");

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
        else { //all tokens handled by OSISHTMLHref will run through the filter now
            return sword::TEIHTMLHREF::handleToken(buf, token, userData);
        }
    }

    return false;
}

void TeiToHtml::renderReference(const char *osisRef, sword::SWBuf &buf,
                                sword::BasicFilterUserData *myUserData)
{

    if (QString ref = osisRef; !ref.isEmpty()) {
        //find out the mod, using the current module makes sense if it's a bible or commentary because the refs link into a bible by default.
        //If the osisRef is something like "ModuleID:key comes here" then the
        // modulename is given, so we'll use that one

        CSwordModuleInfo * mod;
        QString hrefRef;

        //if the osisRef like "GerLut:key" contains a module, use that
        if (auto const pos = ref.indexOf(':');
            (pos > 0)
            && (pos < ref.size() - 1)
            && ref.at(pos - 1).isLetter()
            && ref.at(pos + 1).isLetter())
        {
            hrefRef = ref.mid(pos + 1);
            mod = CSwordBackend::instance().findModuleByName(ref.left(pos));
            if (!mod)
                mod = btConfig().getDefaultSwordModuleByType(
                          QStringLiteral("standardBible"));
        } else {
            hrefRef = ref;
            mod = btConfig().getDefaultSwordModuleByType(
                      QStringLiteral("standardBible"));
        }
        if (!mod)
            mod = CSwordBackend::instance().findFirstAvailableModule(
                      CSwordModuleInfo::Bible);

        if (mod) {
            ReferenceManager::ParseOptions const options{
                    mod->name(),
                    QString::fromUtf8(myUserData->key->getText()),
                    mod->swordModule().getLanguage()};

            buf.append("<a href=\"")
               .append( // create the hyperlink with key and mod
                    ReferenceManager::encodeHyperlink(
                        *mod,
                        ReferenceManager::parseVerseReference(hrefRef, options)
                    ).toUtf8().constData()
                )
               .append("\" crossrefs=\"")
                // ref must contain the osisRef module marker if there was any:
               .append(ReferenceManager::parseVerseReference(ref, options).toUtf8().constData())
               .append("\">");
        }
        // should we add something if there were no referenced module available?
    }
}

void TeiToHtml::renderTargetReference(const char *osisRef, sword::SWBuf &buf,
                                sword::BasicFilterUserData *myUserData)
{
    if (QString ref = osisRef; !ref.isEmpty()) {
        //find out the mod, using the current module makes sense if it's a bible or commentary because the refs link into a bible by default.
        //If the target is something like "ModuleID:key comes here" then the
        // modulename is given, so we'll use that one

        CSwordModuleInfo * mod;
        auto const & backend = CSwordBackend::instance();

        //if the target like "GerLut:key" contains a module, use that
        if (auto const pos = ref.indexOf(':'); pos >= 0) {
            QString newModuleName = ref.left(pos);
            ref.remove(0, pos + 1);

            mod = backend.findModuleByName(newModuleName);
            if (!mod)
                mod = backend.findModuleByName(myUserData->module->getName());
        } else {
            mod = backend.findModuleByName(myUserData->module->getName());
        }

        if (mod) {
            ReferenceManager::ParseOptions const options{
                    mod->name(),
                    QString::fromUtf8(myUserData->key->getText()),
                    mod->swordModule().getLanguage()};

            buf.append("<a class=\"crossreference\" href=\"")
               .append( // create the hyperlink with key and mod
                    ReferenceManager::encodeHyperlink(
                        *mod,
                        ref.toUtf8().constData()
                    ).toUtf8().constData()
                )
               .append("\">");
        }
    }
}

} // namespace Filters
