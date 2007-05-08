//
// C++ Implementation: cinfodisplay
//
// Description:
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

//BibleTime includes
#include "cinfodisplay.h"

#include "backend/cswordmoduleinfo.h"
#include "backend/cswordkey.h"
#include "backend/cswordversekey.h"
#include "backend/creferencemanager.h"
#include "backend/cdisplaytemplatemgr.h"

#include "frontend/cbtconfig.h"
#include "frontend/crossrefrendering.h"

#include "frontend/display/cdisplay.h"
#include "frontend/display/creaddisplay.h"
#include "frontend/display/chtmlreaddisplay.h"

#include "util/scoped_resource.h"

//Sword includes
#include <listkey.h>

//Qt includes
#include <qlayout.h>
#include <qlabel.h>
#include <qscrollview.h>
#include <qregexp.h>

//KDE includes
#include <klocale.h>
#include <kstdaction.h>


using namespace Rendering;
using namespace sword;

namespace InfoDisplay {

	CInfoDisplay::CInfoDisplay(QWidget *parent, const char *name)
: QWidget(parent, name) {
		QVBoxLayout* layout = new QVBoxLayout(this);
		QLabel* headingLabel = new QLabel(i18n("Mag (\"shift\" to lock)"),this);
		headingLabel->setMargin(5);

		m_htmlPart = CDisplay::createReadInstance(0, this);
		m_htmlPart->setMouseTracking(false); //we don't want strong/lemma/note mouse infos
		KStdAction::copy(m_htmlPart->connectionsProxy(), SLOT(copySelection()), 0, "copyMagSelection");

		connect(
			m_htmlPart->connectionsProxy(),
			SIGNAL(referenceClicked(const QString&, const QString&)),
			SLOT(lookup(const QString&, const QString&))
		);

		layout->addWidget(headingLabel);
		layout->addWidget(m_htmlPart->view());
	}


	CInfoDisplay::~CInfoDisplay() {}

	void CInfoDisplay::lookup(const QString &mod_name, const QString &key_text) {
		qWarning("%s %s", mod_name.ascii(), key_text.ascii());
		CSwordModuleInfo* m = CPointers::backend()->findModuleByName(mod_name);
		Q_ASSERT(m);
		if (!m)
			return;

		util::scoped_ptr<CSwordKey> key( CSwordKey::createInstance(m) );
		key->key( key_text ); 

		CDisplayTemplateMgr* mgr = CPointers::displayTemplateManager();
		CDisplayTemplateMgr::Settings settings;
		settings.pageCSS_ID = "infodisplay";
		//  settings.langAbbrev = "";
		QString content = mgr->fillTemplate(CBTConfig::get
					(CBTConfig::displayStyle), key->renderedText(), settings);

		//   qWarning("setting text:\n%s", content.latin1());

		m_htmlPart->setText(content);			// scroll to top
		CHTMLReadDisplay *d = dynamic_cast<CHTMLReadDisplay *>(m_htmlPart);
		d->view()->ensureVisible(0, 0);

	}

	void CInfoDisplay::setInfo(const InfoType type, const QString& data) {
		ListInfoData list;
		list.append( qMakePair(type, data) );

		setInfo(list);
	}


	void CInfoDisplay::setInfo(const ListInfoData& list) {
		//if the widget is hidden it would be inefficient to render and display the data
		if (!isVisible()) {
			return;
		}

		if (list.count() == 0) {
			m_htmlPart->setText("<html></html>");
			return;
		}

		QString text;

		ListInfoData::const_iterator end = list.end();
		for (ListInfoData::const_iterator it = list.begin(); it != end; ++it) {
			switch ( (*it).first ) {
				case Lemma:
				text.append( decodeStrongs( (*it).second ) );
				continue;
				case Morph:
				text.append( decodeMorph( (*it).second ) );
				continue;
				case CrossReference:
				text.append( decodeCrossReference( (*it).second ) );
				continue;
				case Footnote:
				text.append( decodeFootnote( (*it).second ) );
				continue;
				case WordTranslation:
				text.append( getWordTranslation( (*it).second ) );
				continue;
				case WordGloss:
				//text.append( getWordTranslation( (*it).second ) );
				continue;
				case Abbreviation:
				text.append( decodeAbbreviation( (*it).second ) );
				continue;
				default:
				continue;
			};
		}

		CDisplayTemplateMgr* mgr = CPointers::displayTemplateManager();
		CDisplayTemplateMgr::Settings settings;
		settings.pageCSS_ID = "infodisplay";
		//  settings.langAbbrev = "";
		QString content = mgr->fillTemplate(CBTConfig::get
												(CBTConfig::displayStyle), text, settings);

		//   qWarning("setting text:\n%s", content.latin1());

		m_htmlPart->setText(content);
	}


	const QString CInfoDisplay::decodeAbbreviation( const QString& data ) {
		//  QStringList strongs = QStringList::split("|", data);
		QString ret;
		QString text = data;

		ret.append(
			QString("<div class=\"abbreviation\"><h3>%1: %2</h3><p>%3</p></div>")
			.arg(i18n("Abbreviation"))
			.arg("text")
			.arg(text));

		return ret;
	}

	const QString CInfoDisplay::decodeCrossReference( const QString& data ) {
		Q_ASSERT(!data.isEmpty());
		if (data.isEmpty()) {
			return QString("<div class=\"crossrefinfo\"><h3>%1</h3></div>")
				   .arg(i18n("Cross references"));
		}

		//  qWarning("setting crossref %s", data.latin1());

		CSwordBackend::DisplayOptions dispOpts;
		dispOpts.lineBreaks  = false;
		dispOpts.verseNumbers = true;

		CSwordBackend::FilterOptions filterOpts;
		filterOpts.headings    = false;
		filterOpts.strongNumbers  = false;
		filterOpts.morphTags    = false;
		filterOpts.lemmas     = false;
		filterOpts.footnotes   = false;
		filterOpts.scriptureReferences = false;

		CrossRefRendering renderer(dispOpts, filterOpts);
		CTextRendering::KeyTree tree;

		//  const bool isBible = true;
		CSwordModuleInfo* module = CBTConfig::get
									   (CBTConfig::standardBible);

		//a prefixed module gives the module to look into
		QRegExp re("^[^ ]+:");
		//  re.setMinimal(true);
		int pos = re.search(data,0);
		if (pos != -1) {
			pos += re.matchedLength()-1;
		}

		if (pos > 0) {
			const QString moduleName = data.left(pos);
			//     qWarning("found module %s", moduleName.latin1());
			module = CPointers::backend()->findModuleByName(moduleName);
			if (!module) {
				module = CBTConfig::get
							 (CBTConfig::standardBible);
			}
			//   Q_ASSERT(module);
		}

		Q_ASSERT(module);
		CTextRendering::KeyTreeItem::Settings settings (
			false,
			CTextRendering::KeyTreeItem::Settings::CompleteShort
		);

		if (module && (module->type() == CSwordModuleInfo::Bible)) {
			VerseKey vk;
			sword::ListKey refs = vk.ParseVerseList((const char*)data.mid((pos == -1) ? 0 : pos+1).utf8(), "Gen 1:1", true);

			for (int i = 0; i < refs.Count(); ++i) {
				SWKey* key = refs.getElement(i);
				Q_ASSERT(key);
				VerseKey* vk = dynamic_cast<VerseKey*>(key);

				CTextRendering::KeyTreeItem* i = 0;
				if (vk && vk->isBoundSet()) { //render a range of keys
					i = new CTextRendering::KeyTreeItem(
							QString::fromUtf8(vk->LowerBound().getText()),
							QString::fromUtf8(vk->UpperBound().getText()),
							module,
							settings
						);
				}
				else {
					i = new CTextRendering::KeyTreeItem(
							QString::fromUtf8(key->getText()),
							QString::fromUtf8(key->getText()),
							module,
							settings
						);
				}

				Q_ASSERT(i);

				tree.append( i );
			}
		}
		else if (module) {
			CTextRendering::KeyTreeItem* i = new CTextRendering::KeyTreeItem(
												 data.mid((pos == -1) ? 0 : pos+1),
												 module,
												 settings
											 );
			tree.append( i );
		}

		//  qWarning("rendered the tree: %s", renderer.renderKeyTree(tree).latin1());
		//spanns containing rtl text need dir=rtl on their parent tag to be aligned properly
		return QString("<div class=\"crossrefinfo\"><h3>%1</h3><div class=\"para\" dir=\"%2\">%3</div></div>")
			   .arg(i18n("Cross references"))
	   .arg(module ? ((module->textDirection() == CSwordModuleInfo::LeftToRight) ? "ltr" : "rtl") : "")
			   .arg(renderer.renderKeyTree(tree));
	}

	/*!
	    \fn CInfoDisplay::decodeFootnote( const QString& data )
	 */
	const QString CInfoDisplay::decodeFootnote( const QString& data ) {
		QStringList list = QStringList::split("/", data);
		Q_ASSERT(list.count() >= 3);
		if (!list.count()) {
			return QString::null;
		}

		const QString modulename = list.first();
		const QString swordFootnote = list.last();

		// remove the first and the last and then rejoin it to get a key
		list.pop_back(); list.pop_front();
		const QString keyname = list.join("/");

		CSwordModuleInfo* module = CPointers::backend()->findModuleByName(modulename);
		if (!module) {
			return QString::null;
		}

		util::scoped_ptr<CSwordKey> key( CSwordKey::createInstance(module) );
		key->key(keyname);
		key->renderedText(); //force entryAttributes

		const char* note = module->module()->getEntryAttributes()["Footnote"][swordFootnote.latin1()]["body"].c_str();

		QString text = module->isUnicode() ? QString::fromUtf8(note) : QString(note);
		text = QString::fromUtf8(module->module()->RenderText(
									 module->isUnicode()
									 ? (const char*)text.utf8()
									 : (const char*)text.latin1()
								 ));

		return QString("<div class=\"footnoteinfo\"><h3>%1</h3><p>%2</p></div>")
			   .arg(i18n("Footnote"))
			   .arg(text);
	}

	const QString CInfoDisplay::decodeStrongs( const QString& data ) {
		QStringList strongs = QStringList::split("|", data);
		QString ret;

		QStringList::const_iterator end = strongs.end();
		for (QStringList::const_iterator it = strongs.begin(); it != end; ++it) {
			CSwordModuleInfo* const module = CBTConfig::get
												 (
													 ((*it).left(1) == QString("H")) ?
													 CBTConfig::standardHebrewStrongsLexicon :
													 CBTConfig::standardGreekStrongsLexicon
												 );

			QString text;
			if (module) {
				util::scoped_ptr<CSwordKey> key( CSwordKey::createInstance(module) );
				key->key( (*it).mid(1) ); //skip H or G (language sign), will have to change later if we have better modules
				text = key->renderedText();
			}
			//if the module could not be found just display an empty lemma info

			ret.append(
				QString("<div class=\"strongsinfo\"><h3>%1: %2</h3><p>%3</p></div>")
				.arg(i18n("Strongs"))
				.arg(*it)
				.arg(text)
			);
		}

		return ret;
	}

	const QString CInfoDisplay::decodeMorph( const QString& data ) {
		QStringList morphs = QStringList::split("|", data);
		QString ret;

		for (QStringList::iterator it = morphs.begin(); it != morphs.end(); ++it) {
			CSwordModuleInfo* module = 0;
			bool skipFirstChar = false;
			QString value = "";
			QString valueClass = "";

			int valStart = (*it).find(':');
			if (valStart > -1) {
				valueClass = (*it).mid(0, valStart);
				module = CPointers::backend()->findModuleByName( valueClass );
				Q_ASSERT(module);
			}
			value = (*it).mid(valStart+1); //works for prepended module and without (-1 +1 == 0).

			// if we don't have a class assigned or desired one isn't installed...
			if (!module) {
				// Morphs usually don't have [GH] prepended, but some old OLB
				// codes do.  We should check if we're digit after first char
				// to better guess this.
				// No need to check len, if at(1) is > len QChar::null is
				// returned which is ok to .isDigit()
				if (value.at(1).isDigit()) {
					switch (value.at(0).latin1()) {
						case 'G':
						module = CBTConfig::get
									 (CBTConfig::standardGreekMorphLexicon);
						skipFirstChar = true;
						break;
						case 'H':
						module = CBTConfig::get
									 (CBTConfig::standardHebrewMorphLexicon);
						skipFirstChar = true;
						break;
						default:
						skipFirstChar = false;
						//TODO: we can't tell here if it's a greek or hebrew moprh code, that's a problem we have to solve
						//       module = CBTConfig::get(CBTConfig::standardGreekMorphLexicon);
						break;
					}
				}
				//if it is still not set use the default
				if (!module) {
					module = CBTConfig::get
								 (CBTConfig::standardGreekMorphLexicon);
				}
			}

			QString text;
			Q_ASSERT(module);
			if (module) {
				util::scoped_ptr<CSwordKey> key( CSwordKey::createInstance(module) );

				//skip H or G (language sign) if we have to skip it
				const bool isOk = key->key( skipFirstChar ? value.mid(1) : value );
				Q_ASSERT(isOk);
				if (!isOk) { //try to use the other morph lexicon, because this one failed with the current morph code
					key->module(CBTConfig::get
									(CBTConfig::standardHebrewMorphLexicon));
					key->key( skipFirstChar ? value.mid(1) : value );
				}

				text = key->renderedText();
			}

			//if the module wasn't found just display an empty morph info
			ret.append( QString("<div class=\"morphinfo\"><h3>%1: %2</h3><p>%3</p></div>")
						.arg(i18n("Morphology"))
						.arg(value)
						.arg(text)
					  );
		}

		return ret;
	}

	const QString CInfoDisplay::getWordTranslation( const QString& data ) {
		CSwordModuleInfo* const module = CBTConfig::get
											 (CBTConfig::standardLexicon);
		if (!module) {
			return QString::null;
		}

		util::scoped_ptr<CSwordKey> key( CSwordKey::createInstance(module) );
		key->key( data );
		if (key->key().upper() != data.upper()) { //key not present in the lexicon
			return QString::null;
		}

		QString ret = QString("<div class=\"translationinfo\"><h3>%1: %2</h3><p>%3</p></div>")
					  .arg(i18n("Word lookup"))
					  .arg(data)
					  .arg(key->renderedText());

		return ret;
	}


	/*!
	    \fn CInfoDisplay::clearInfo()
	 */
	void CInfoDisplay::clearInfo() {
		CDisplayTemplateMgr* tmgr = CPointers::displayTemplateManager();
		CDisplayTemplateMgr::Settings settings;
		settings.pageCSS_ID = "infodisplay";

		m_htmlPart->setText( tmgr->fillTemplate(CBTConfig::get
													(CBTConfig::displayStyle), QString::null, settings) );
	}

} //end of namespace InfoDisplay
