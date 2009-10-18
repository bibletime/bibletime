/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/filters/bt_teihtml.h"

#include <QString>
#include "backend/config/cbtconfig.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/clanguagemgr.h"
#include "backend/managers/creferencemanager.h"
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
    	if (0) {

    	}
        else { //all tokens handled by OSISHTMLHref will run through the filter now
            return sword::TEIHTMLHREF::handleToken(buf, token, userData);
        }
    }

    return false;
}

void Filters::BT_TEIHTML::renderReference(const char *osisRef, sword::SWBuf &buf, sword::SWModule *myModule, sword::BasicFilterUserData *myUserData) {
    QString ref( osisRef );
    QString hrefRef( ref );
    //Q_ASSERT(!ref.isEmpty()); checked later

    if (!ref.isEmpty()) {
        //find out the mod, using the current module makes sense if it's a bible or commentary because the refs link into a bible by default.
        //If the osisRef is something like "ModuleID:key comes here" then the
        // modulename is given, so we'll use that one

        CSwordModuleInfo* mod = CPointers::backend()->findSwordModuleByPointer(myModule);
        //Q_ASSERT(mod); checked later
        if (!mod || (mod->type() != CSwordModuleInfo::Bible
                     && mod->type() != CSwordModuleInfo::Commentary)) {

            mod = CBTConfig::get( CBTConfig::standardBible );
        }

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
            CReferenceManager::ParseOptions options;
            options.refBase = QString::fromUtf8(myUserData->key->getText());
            options.refDestinationModule = QString(mod->name());
            options.sourceLanguage = QString(myModule->Lang());
            options.destinationLanguage = QString("en");

            buf.append("<a href=\"");
            buf.append( //create the hyperlink with key and mod
                CReferenceManager::encodeHyperlink(
                    mod->name(),
                    CReferenceManager::parseVerseReference(hrefRef, options),
                    CReferenceManager::typeFromModule(mod->type())
                ).toUtf8().constData()
            );
            buf.append("\" crossrefs=\"");
            buf.append((const char*)CReferenceManager::parseVerseReference(ref, options).toUtf8().constData()); //ref must contain the osisRef module marker if there was any
            buf.append("\">");
        }
        // should we add something if there were no referenced module available?
    }
}

