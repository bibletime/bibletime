/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/




//own includes
#include "cswordkey.h"
#include "cswordmoduleinfo.h"

#include "cswordversekey.h"
#include "cswordldkey.h"
#include "cswordtreekey.h"

#include "util/ctoolclass.h"

//Sword includes
#include <swmodule.h>
#include <swkey.h>
#include <versekey.h>
#include <treekey.h>
#include <treekeyidx.h>

//Qt includes
#include <qregexp.h>
#include <qstring.h>

CSwordKey::CSwordKey(CSwordModuleInfo* const module) : m_module(module) {}

CSwordKey::CSwordKey(const CSwordKey& k) {
	m_module = k.m_module;
}

const QString CSwordKey::rawText() {
	if (!m_module) {
		return QString::null;
	}

	if (dynamic_cast<sword::SWKey*>(this)) {
		m_module->module()->getKey()->setText( (const char*)key().utf8() );
	}

	if (key().isNull()) {
		return QString::null;
	}

// 	qWarning("rawText: %s", m_module->module()->getRawEntry());
	return QString::fromUtf8( m_module->module()->getRawEntry() );
}

const QString CSwordKey::renderedText( const CSwordKey::TextRenderType mode ) {
	Q_ASSERT(m_module);
	if (!m_module) {
		return QString::null;
	}

	using namespace sword;
	SWKey* const k = dynamic_cast<SWKey*>(this);

	if (k) {
		VerseKey* vk_mod = dynamic_cast<VerseKey*>(m_module->module()->getKey());

		if (vk_mod) {
			vk_mod->Headings(1);
		}

		m_module->module()->getKey()->setText( this->key().utf8() );

		if (m_module->type() == CSwordModuleInfo::Lexicon) {
			m_module->snap();
			/* In lexicons make sure that our key (e.g. 123) was successfully set to the module,
			i.e. the module key contains this key (e.g. 0123 contains 123) */

			if ( strcasecmp(m_module->module()->getKey()->getText(), (const char*)key().utf8())
					&& !strstr(m_module->module()->getKey()->getText(), (const char*)key().utf8())
			   ) {
				qDebug("return an empty key for %s", m_module->module()->getKey()->getText());
				return QString::null;
			}
		}
	}

	Q_ASSERT(!key().isNull());
	if (!key().isNull()) { //we have valid text
		QString text = QString::fromUtf8( m_module->module()->RenderText() );

		// This is yucky, but if we want strong lexicon refs we have to do it here.
		if (m_module->type() == CSwordModuleInfo::Lexicon) {
			QString t(text);
			QRegExp rx("(GREEK|HEBREW) for 0*([1-9]\\d*)");	// ignore 0's before number
			int pos = 0;
			while( (pos = rx.search(t, pos)) != -1 ) {
				QString language = rx.cap(1);
				QString langcode = QString(language.at(0));	// "G" or "H"
				QString number = rx.cap(2);
				QString paddednumber = number.rightJustify(5, '0');	// Form 00123

				text.replace(
						QRegExp( QString(
							"(>[^<>]+)"			// Avoid replacing inside tags
							"\\b(0*%1)\\b").arg(number) ),	// And span around 0's
						QString("\\1<span lemma=\"%1%2\"><a href=\"strongs://%3/%4\">\\2</a></span>")
							.arg(langcode, paddednumber, language, paddednumber)
					);
				pos += rx.matchedLength();
			}
		}

		if (mode == HTMLEscaped) {
			//we have to encode all UTF-8 in HTML escapes
			// go though every character and write down the escaped HTML unicode entity
			// form is &#<decimal unicode value here>;
			QString ret;
			QChar c;
			const unsigned int length = text.length();

			for (unsigned int i = 0; i < length; ++i) {
				c = text.at(i);

				if (c.latin1()) { //normal latin1 character
					ret.append(c);
				}
				else {//unicode character, needs to be escaped
					ret.append("&#")
					.append(c.unicode())
					.append(";");
				}
			}

			return ret;
		}
		else {
			return text;
		}
	}

	return QString::null;
}

const QString CSwordKey::strippedText() {
	if (!m_module) {
		return QString::null;
	}

	if (/*sword::SWKey* k =*/ dynamic_cast<sword::SWKey*>(this)) {
		//   m_module->module()->SetKey(k);
		m_module->module()->getKey()->setText( (const char*)key().utf8() );
	}

	return QString::fromUtf8( m_module->module()->StripText() );
}


/** This will create a proper key object from a given module */
CSwordKey* CSwordKey::createInstance( CSwordModuleInfo* const module ) {
	if (!module) {
		return 0;
	}

	switch( module->type() ) {

		case CSwordModuleInfo::Bible://fall through

		case CSwordModuleInfo::Commentary:
		return new CSwordVerseKey( (sword::VerseKey *) ( (sword::SWKey *)(*module->module()) ), module );

		case CSwordModuleInfo::Lexicon:
		return new CSwordLDKey( (sword::SWKey *)(*module->module()), module);

		case CSwordModuleInfo::GenericBook:
		return new CSwordTreeKey( (sword::TreeKeyIdx*)((sword::SWKey *)(*module->module())), module );

		default:
		return 0;
	}
}
