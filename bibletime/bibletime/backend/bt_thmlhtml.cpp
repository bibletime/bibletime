/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/




//BibleTime includes
#include "backend/bt_thmlhtml.h"
#include "backend/clanguagemgr.h"
#include "backend/cswordmoduleinfo.h"
#include "backend/creferencemanager.h"

#include "frontend/cbtconfig.h"

#include "util/cpointers.h"
#include "util/scoped_resource.h"

#include <iostream>

//Sword includes
#include <swmodule.h>
#include <utilxml.h>
#include "versekey.h"

//Qt includes
#include <qstring.h>
#include <qregexp.h>

//System includes
#include <stdlib.h>

using namespace Filters;

BT_ThMLHTML::BT_ThMLHTML() {
	setEscapeStringCaseSensitive(true);
	setPassThruUnknownEscapeString(true); //the HTML widget will render the HTML escape codes

	setTokenStart("<");
	setTokenEnd(">");
	setTokenCaseSensitive(true);

	addTokenSubstitute("/foreign", "</span>");

	removeTokenSubstitute("note");
	removeTokenSubstitute("/note");
}

char BT_ThMLHTML::processText(sword::SWBuf& buf, const sword::SWKey* key, const sword::SWModule* module) {
	ThMLHTML::processText(buf, key, module);

	CSwordModuleInfo* m = CPointers::backend()->findModuleByName( module->Name() );

	if (m && !(m->has(CSwordModuleInfo::lemmas) || m->has(CSwordModuleInfo::strongNumbers))) { //only parse if the module has strongs or lemmas
		return 1;
	}

	QString result;

	QString t = QString::fromUtf8(buf.c_str());
	QRegExp tag("([.,;]?<sync[^>]+(type|value)=\"([^\"]+)\"[^>]+(type|value)=\"([^\"]+)\"([^<]*)>)+");

	QStringList list;
	int lastMatchEnd = 0;
	int pos = tag.search(t,0);

	if (pos == -1) { //no strong or morph code found in this text
		return 1; //WARNING: Return alread here
	}

	while (pos != -1) {
		list.append(t.mid(lastMatchEnd, pos+tag.matchedLength()-lastMatchEnd));

		lastMatchEnd = pos+tag.matchedLength();
		pos = tag.search(t,pos+tag.matchedLength());
	}

	if (!t.right(t.length() - lastMatchEnd).isEmpty()) {
		list.append(t.right(t.length() - lastMatchEnd));
	}

	tag = QRegExp("<sync[^>]+(type|value|class)=\"([^\"]+)\"[^>]+(type|value|class)=\"([^\"]+)\"[^>]+((type|value|class)=\"([^\"]+)\")*([^<]*)>");

	for (QStringList::iterator it = list.begin(); it != list.end(); ++it) {
		QString e( *it );

		const bool textPresent = (e.stripWhiteSpace().remove(QRegExp("[.,;:]")).left(1) != "<");

		if (!textPresent) {
			continue;
		}


		bool hasLemmaAttr = false;
		bool hasMorphAttr = false;

		int pos = tag.search(e, 0);
		bool insertedTag = false;
		QString value;
		QString valueClass;

		while (pos != -1) {
			bool isMorph = false;
			bool isStrongs = false;
			value = QString::null;
			valueClass = QString::null;

			// check 3 attribute/value pairs

			for (int i = 1; i < 6; i += 2) {
				if (i > 4)
					i++;

				if (tag.cap(i) == "type") {
					isMorph   = (tag.cap(i+1) == "morph");
					isStrongs = (tag.cap(i+1) == "Strongs");
				}
				else if (tag.cap(i) == "value") {
					value = tag.cap(i+1);
				}
				else if (tag.cap(i) == "class") {
					valueClass = tag.cap(i+1);
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

				QString rep;
				rep.setLatin1("<span lemma=\"").append(value).append("\">");
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
					const int foundAttrPos = e.find(attrRegExp, pos);

					if (foundAttrPos != -1) {
						e.insert(foundAttrPos + attrRegExp.matchedLength(), QString("|").append(value));
						pos += value.length() + 1;

						hasLemmaAttr = !isMorph;
						hasMorphAttr = isMorph;
					}
				}
				else { //attribute was not yet inserted
					const int attrPos = e.find(QRegExp("morph=|lemma="), 0);

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
			pos = tag.search(e, pos);
		}

		result.append( e );
	}

	if (list.count()) {
		buf = (const char*)result.utf8();
	}

	return 1;
}


bool BT_ThMLHTML::handleToken(sword::SWBuf &buf, const char *token, sword::BasicFilterUserData *userData) {
	if (!substituteToken(buf, token) && !substituteEscapeString(buf, token)) {
		sword::XMLTag tag(token);
		BT_UserData* myUserData = dynamic_cast<BT_UserData*>(userData);
		sword::SWModule* myModule = const_cast<sword::SWModule*>(myUserData->module); //hack to be able to call stuff like Lang()

		if ( tag.getName() && !strcasecmp(tag.getName(), "foreign") ) { // a text part in another language, we have to set the right font

			if (tag.getAttribute("lang")) {
				const char* abbrev = tag.getAttribute("lang");
				//const CLanguageMgr::Language* const language = CPointers::languageMgr()->languageForAbbrev( QString::fromLatin1(abbrev) );

				buf.append("<span class=\"foreign\" lang=\"");
				buf.append(abbrev);
				buf.append("\">");
			}
		}
		else if (tag.getName() && !strcasecmp(tag.getName(), "sync")) { //lemmas, morph codes or strongs

			if (tag.getAttribute("type") && (!strcasecmp(tag.getAttribute("type"), "morph") || !strcasecmp(tag.getAttribute("type"), "Strongs") || !strcasecmp(tag.getAttribute("type"), "lemma"))) { // Morph or Strong
				buf.append('<');
				buf.append(token);
				buf.append('>');
			}
		}
		else if (tag.getName() && !strcasecmp(tag.getName(), "note")) { // <note> tag

			if (!tag.isEndTag() && !tag.isEmpty()) {
				//appending is faster than appendFormatted
				buf.append(" <span class=\"footnote\" note=\"");
				buf.append(myModule->Name());
				buf.append('/');
				buf.append(myUserData->key->getShortText());
				buf.append('/');
				buf.append( QString::number(myUserData->swordFootnote++).latin1() );
				buf.append("\">*</span> ");

				myUserData->suspendTextPassThru = true;
				myUserData->inFootnoteTag = true;
			}
			else if (tag.isEndTag() && !tag.isEmpty()) { //end tag
				//buf += ")</span>";
				myUserData->suspendTextPassThru = false;
				myUserData->inFootnoteTag = false;
			}
		}
		else if (tag.getName() && !strcasecmp(tag.getName(), "scripRef")) { // a scripRef
			//scrip refs which are embeded in footnotes may not be displayed!

			if (!myUserData->inFootnoteTag) {
				if (tag.isEndTag()) {
					if (myUserData->inscriptRef) { // like "<scripRef passage="John 3:16">See John 3:16</scripRef>"
						buf.append("</a></span>");

						myUserData->inscriptRef = false;
						myUserData->suspendTextPassThru = false;
					}
					else { // like "<scripRef>John 3:16</scripRef>"

						CSwordModuleInfo* mod = CBTConfig::get(CBTConfig::standardBible);
						Q_ASSERT(mod);
						if (mod) {
							CReferenceManager::ParseOptions options;
							options.refBase = QString::fromUtf8(myUserData->key->getText()); //current module key
							options.refDestinationModule = QString(mod->name());
							options.sourceLanguage = QString(myModule->Lang());
 							options.destinationLanguage = QString("en");

							//it's ok to split the reference, because to descriptive text is given
							bool insertSemicolon = false;
							buf.append("<span class=\"crossreference\">");
							QStringList refs = QStringList::split(";", QString::fromUtf8(myUserData->lastTextNode.c_str()));
							QString oldRef; //the previous reference to use as a base for the next refs
							for (QStringList::iterator it(refs.begin()); it != refs.end(); ++it) {

								if (! oldRef.isEmpty() ){
									options.refBase = oldRef; //use the last ref as a base, e.g. Rom 1,2-3, when the next ref is only 3:3-10
								}
								const QString completeRef( CReferenceManager::parseVerseReference((*it), options) );

								oldRef = completeRef; //use the parsed result as the base for the next ref.

 								if (insertSemicolon) { //prepend a ref divider if we're after the first one
									buf.append("; ");
 								}

								buf.append("<a href=\"");
								buf.append(
									CReferenceManager::encodeHyperlink(
										mod->name(),
										completeRef,
										CReferenceManager::typeFromModule(mod->type())
									).utf8()
								);

								buf.append("\" crossrefs=\"");
								buf.append((const char*)completeRef.utf8());
								buf.append("\">");

								buf.append((const char*)(*it).utf8());

								buf.append("</a>");

								insertSemicolon = true;
							}
							buf.append("</span>"); //crossref end
						}

						myUserData->suspendTextPassThru = false;
					}
				}
				else if (tag.getAttribute("passage") ) { //the passage was given as a parameter value
					myUserData->inscriptRef = true;
					myUserData->suspendTextPassThru = false;

					const char* ref = tag.getAttribute("passage");
					Q_ASSERT(ref);

					CSwordModuleInfo* mod = CBTConfig::get(CBTConfig::standardBible);
					Q_ASSERT(mod);

					CReferenceManager::ParseOptions options;
					options.refBase = QString::fromUtf8(myUserData->key->getText());
					options.refDestinationModule = QString(mod->name());
					options.sourceLanguage = myModule->Lang();
					options.destinationLanguage = QString("en");

					const QString completeRef = CReferenceManager::parseVerseReference(QString::fromUtf8(ref), options);

					if (mod) {
						buf.append("<span class=\"crossreference\">");
						buf.append("<a href=\"");
						buf.append(
 							CReferenceManager::encodeHyperlink(
 								mod->name(),
 								completeRef,
 								CReferenceManager::typeFromModule(mod->type())
							).utf8()
						);
						buf.append("\" crossrefs=\"");
						buf.append((const char*)completeRef.utf8());
						buf.append("\">");
					}
					else {
						buf.append("<span><a>");
					}
				}
				else if ( !tag.getAttribute("passage") ) { // we're starting a scripRef like "<scripRef>John 3:16</scripRef>"
					myUserData->inscriptRef = false;

					// let's stop text from going to output, the text get's added in the -tag handler
					myUserData->suspendTextPassThru = true;
				}
			}
		}
		else if (tag.getName() && !strcasecmp(tag.getName(), "div")) {
			if (tag.isEndTag()) {
				buf.append("</div>");
			}
			else if ( tag.getAttribute("class") && !strcasecmp(tag.getAttribute("class"),"sechead") ) {
				buf.append("<div class=\"sectiontitle\">");
			}
			else if (tag.getAttribute("class") && !strcasecmp(tag.getAttribute("class"), "title")) {
				buf.append("<div class=\"booktitle\">");
			}
		}
		else if (tag.getName() && !strcasecmp(tag.getName(), "img") && tag.getAttribute("src")) {
			const char* value = tag.getAttribute("src");

			if (value[0] == '/') {
				value++; //strip the first /
			}

			buf.append("<img src=\"file:");
			buf.append(myUserData->module->getConfigEntry("AbsoluteDataPath"));
			buf.append('/');
			buf.append(value);
			buf.append("\" />");
		}
		else { // let unknown token pass thru
			return sword::ThMLHTML::handleToken(buf, token, userData);
		}
	}

	return true;
}
