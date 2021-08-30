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

#include "teitohtml.h"

#include <QString>
#include "../config/btconfig.h"
#include "../drivers/cswordmoduleinfo.h"
#include "../managers/clanguagemgr.h"
#include "../managers/cswordbackend.h"
#include "../managers/referencemanager.h"

// Sword includes:
#include <swbuf.h>
#include <swmodule.h>
#include <utilxml.h>


namespace Filters {

TeiToHtml::TeiToHtml()
    : sword::TEIHTMLHREF()
{
    setPassThruUnknownEscapeString(true); // the HTML widget will render the HTML escape codes
}

bool TeiToHtml::handleToken(sword::SWBuf &buf, const char *token,
                            sword::BasicFilterUserData *userData)
{
    // manually process if it wasn't a simple substitution

    if (!substituteToken(buf, token)) {

        sword::XMLTag tag(token);

        if (0) {

        }
        else if (!strcmp(tag.getName(), "ref")) {

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
        // <hi> highlighted text
        else if (!strcmp(tag.getName(), "hi")) {
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
    QString ref( osisRef );
    QString hrefRef( ref );

    if (!ref.isEmpty()) {
        //find out the mod, using the current module makes sense if it's a bible or commentary because the refs link into a bible by default.
        //If the osisRef is something like "ModuleID:key comes here" then the
        // modulename is given, so we'll use that one

        CSwordModuleInfo* mod = btConfig().getDefaultSwordModuleByType( "standardBible" );
        if (! mod)
            mod = CSwordBackend::instance()->findFirstAvailableModule(CSwordModuleInfo::Bible);

        // BT_ASSERT(mod); There's no necessarily a module or standard Bible

        //if the osisRef like "GerLut:key" contains a module, use that
        int pos = ref.indexOf(":");

        if ((pos > 0)
            && (pos < ref.size() - 1)
            && ref.at(pos - 1).isLetter()
            && ref.at(pos + 1).isLetter())
        {
            QString newModuleName = ref.left(pos);
            hrefRef = ref.mid(pos + 1);

            if (CSwordBackend::instance()->findModuleByName(newModuleName)) {
                mod = CSwordBackend::instance()->findModuleByName(newModuleName);
            }
        }

        if (mod) {
            ReferenceManager::ParseOptions const options(
                    mod->name(),
                    QString::fromUtf8(myUserData->key->getText()),
                    mod->module().getLanguage());

            buf.append("<a href=\"")
               .append( // create the hyperlink with key and mod
                    ReferenceManager::encodeHyperlink(
                        mod->name(),
                        ReferenceManager::parseVerseReference(hrefRef, options),
                        ReferenceManager::typeFromModule(mod->type())
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
    QString ref( osisRef );
    QString hrefRef( ref );

    if (!ref.isEmpty()) {
        //find out the mod, using the current module makes sense if it's a bible or commentary because the refs link into a bible by default.
        //If the target is something like "ModuleID:key comes here" then the
        // modulename is given, so we'll use that one

        const char * currentModuleName = myUserData->module->getName();
        CSwordModuleInfo* mod = CSwordBackend::instance()->findModuleByName(currentModuleName);

        //if the target like "GerLut:key" contains a module, use that
        int pos = ref.indexOf(":");

        if (pos >= 0) {
            QString newModuleName = ref.left(pos);
            hrefRef = ref.mid(pos + 1);

            if (CSwordBackend::instance()->findModuleByName(newModuleName)) {
                mod = CSwordBackend::instance()->findModuleByName(newModuleName);
            }
        }

        if (mod) {
            ReferenceManager::ParseOptions const options(
                    mod->name(),
                    QString::fromUtf8(myUserData->key->getText()),
                    mod->module().getLanguage());

            buf.append("<a class=\"crossreference\" href=\"")
               .append( // create the hyperlink with key and mod
                    ReferenceManager::encodeHyperlink(
                        mod->name(),
                        hrefRef.toUtf8().constData(),
                        ReferenceManager::typeFromModule(mod->type())
                    ).toUtf8().constData()
                )
               .append("\">");
        }
    }
}

} // namespace Filters
