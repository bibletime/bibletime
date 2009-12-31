/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/filters/bt_teihtml.h"

#include <QString>
#include "backend/config/cbtconfig.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/clanguagemgr.h"
#include "backend/managers/referencemanager.h"
#include "util/cpointers.h"

// Sword includes:
#include <swbuf.h>
#include <swmodule.h>
#include <utilxml.h>



Filters::BT_TEIHTML::BT_TEIHTML() : sword::TEIHTMLHREF() {
    setPassThruUnknownEscapeString(true); //the HTML widget will render the HTML escape codes
}

bool Filters::BT_TEIHTML::handleToken(sword::SWBuf &buf, const char *token, sword::BasicFilterUserData *userData) {
    // manually process if it wasn't a simple substitution

    if (!substituteToken(buf, token)) {

        sword::XMLTag tag(token);

        if (0) {

        }
        else if (!strcmp(tag.getName(), "ref")) {

            if (!tag.isEndTag() && !tag.isEmpty()) {

                renderReference(tag.getAttribute("osisRef"), buf, userData);

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

void Filters::BT_TEIHTML::renderReference(const char *osisRef, sword::SWBuf &buf, sword::BasicFilterUserData *myUserData) {
    QString ref( osisRef );
    QString hrefRef( ref );

    if (!ref.isEmpty()) {
        //find out the mod, using the current module makes sense if it's a bible or commentary because the refs link into a bible by default.
        //If the osisRef is something like "ModuleID:key comes here" then the
        // modulename is given, so we'll use that one

        CSwordModuleInfo* mod = CBTConfig::get( CBTConfig::standardBible );

        // Q_ASSERT(mod); There's no necessarily a module or standard Bible

        //if the osisRef like "GerLut:key" contains a module, use that
        int pos = ref.indexOf(":");

        if ((pos >= 0) && ref.at(pos - 1).isLetter() && ref.at(pos + 1).isLetter()) {
            QString newModuleName = ref.left(pos);
            hrefRef = ref.mid(pos + 1);

            if (CPointers::backend()->findModuleByName(newModuleName)) {
                mod = CPointers::backend()->findModuleByName(newModuleName);
            }
        }

        if (mod) {
            ReferenceManager::ParseOptions options;
            options.refBase = QString::fromUtf8(myUserData->key->getText());
            options.refDestinationModule = QString(mod->name());
            options.sourceLanguage = QString(mod->module()->Lang());
            options.destinationLanguage = QString("en");

            buf.append("<a href=\"");
            buf.append( //create the hyperlink with key and mod
                ReferenceManager::encodeHyperlink(
                    mod->name(),
                    ReferenceManager::parseVerseReference(hrefRef, options),
                    ReferenceManager::typeFromModule(mod->type())
                ).toUtf8().constData()
            );
            buf.append("\" crossrefs=\"");
            buf.append((const char*)ReferenceManager::parseVerseReference(ref, options).toUtf8().constData()); //ref must contain the osisRef module marker if there was any
            buf.append("\">");
        }
        // should we add something if there were no referenced module available?
    }
}

