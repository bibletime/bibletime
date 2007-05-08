//
// C++ Implementation: ctextrendering
//
// Description:
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ctextrendering.h"

//BibleTime includes
#include "backend/cswordkey.h"
#include "backend/cswordversekey.h"
#include "backend/cswordmoduleinfo.h"
#include "backend/cdisplaytemplatemgr.h"
#include "backend/creferencemanager.h"

#include "util/scoped_resource.h"
#include "util/ctoolclass.h"

//Sword includes
#include <swkey.h>

//Qt includes
#include <qregexp.h>

//KDE includes
#include <klocale.h>
#include <kurl.h>

using namespace Rendering;

CTextRendering::KeyTreeItem::KeyTreeItem(const QString& key, CSwordModuleInfo const * mod, const Settings settings )
: m_settings( settings ),
m_moduleList(),
m_key( key ),
m_childList( 0 ),
m_stopKey( QString::null ),
m_alternativeContent( QString::null ) {
	m_moduleList.append( const_cast<CSwordModuleInfo*>(mod) ); //BAD CODE
}

CTextRendering::KeyTreeItem::KeyTreeItem(const QString& content, const Settings settings )
: m_settings( settings ),
m_moduleList(),
m_key( QString::null ),
m_childList( 0 ),
m_stopKey( QString::null ),
m_alternativeContent( content ) {}

CTextRendering::KeyTreeItem::KeyTreeItem(const QString& key, const ListCSwordModuleInfo& mods, const Settings settings )
: m_settings( settings ),
m_moduleList( mods ),
m_key( key ),
m_childList( 0 ),
m_stopKey( QString::null ),
m_alternativeContent( QString::null ) {}

CTextRendering::KeyTreeItem::KeyTreeItem()
: m_settings(),
m_moduleList(),
m_key(QString::null),
m_childList(0),
m_stopKey(QString::null),
m_alternativeContent(QString::null) {}

CTextRendering::KeyTreeItem::KeyTreeItem(const KeyTreeItem& i)
: m_settings( i.m_settings ),
m_moduleList( i.m_moduleList ),
m_key( i.m_key ),
m_childList( 0 ),
m_stopKey( i.m_stopKey ),
m_alternativeContent( i.m_alternativeContent ) {
	if (i.hasChildItems()) {
		m_childList = new KeyTree();
		*m_childList = *(i.childList()); //deep copy
	}

}

CTextRendering::KeyTreeItem::~KeyTreeItem() {
	delete m_childList;
	m_childList = 0;
}

CTextRendering::KeyTreeItem::KeyTreeItem(const QString& startKey, const QString& stopKey, CSwordModuleInfo* module, const Settings settings)
: m_settings( settings ),
m_moduleList(),
m_key( startKey ),
m_childList( 0 ),
m_stopKey( stopKey ),
m_alternativeContent( QString::null ) {
	Q_ASSERT(module);
	m_moduleList.append(module);

	//use the start and stop key to ceate our child items

	if (module->type() == CSwordModuleInfo::Bible) {
		CSwordVerseKey start(module);
		start = startKey;

		CSwordVerseKey stop(module);
		stop = stopKey;

		if (!m_key.isEmpty() && !m_stopKey.isEmpty()) { //we have a range of keys
			bool ok = true;

			while (ok && ((start < stop) || (start == stop)) ) { //range
				childList()->append(
					new KeyTreeItem(start.key(), module, KeyTreeItem::Settings(false, settings.keyRenderingFace))
				);
				

				ok = start.next(CSwordVerseKey::UseVerse);
			}
		}
		else if (m_key.isEmpty()) {
			childList()->append( new KeyTreeItem(startKey, module, KeyTreeItem::Settings(false, settings.keyRenderingFace)) );
		}
	}
	else if ((module->type() == CSwordModuleInfo::Lexicon) || (module->type() == CSwordModuleInfo::Commentary) ) {
		childList()->append( new KeyTreeItem(startKey, module, KeyTreeItem::Settings(false, KeyTreeItem::Settings::NoKey)) );
	}
	else if (module->type() == CSwordModuleInfo::GenericBook) {
		childList()->append( new KeyTreeItem(startKey, module, KeyTreeItem::Settings(false, KeyTreeItem::Settings::NoKey)) );
	}

	//make it into "<simple|range> (modulename)"

	if (startKey == stopKey) {
		m_alternativeContent = startKey;
	}
	else {
		sword::VerseKey vk(startKey.utf8(), stopKey.utf8());

		if (vk.LowerBound().Book() != vk.UpperBound().Book()) {
			m_alternativeContent = QString::fromUtf8(vk.getRangeText());
		}
		else if (vk.LowerBound().Chapter() != vk.UpperBound().Chapter()) {
			m_alternativeContent = QString("%1 - %2:%3")
								   .arg(QString::fromUtf8(vk.LowerBound().getText()))
								   .arg(vk.UpperBound().Chapter())
								   .arg(vk.UpperBound().Verse());
		}
		else { //only verses differ (same book, same chapter)
			m_alternativeContent = QString("%1 - %2")
								   .arg(QString::fromUtf8(vk.LowerBound().getText()))
								   .arg(vk.UpperBound().Verse());
		}
	}

	m_alternativeContent.append(" (").append(module->name()).append(")");
	m_alternativeContent.prepend("<div class=\"rangeheading\" dir=\"ltr\">").append("</div>"); //insert the right tags
}

const QString& CTextRendering::KeyTreeItem::getAlternativeContent() const {
	return m_alternativeContent;
}

ListCSwordModuleInfo CTextRendering::KeyTree::collectModules() const {
	//collect all modules which are available and used by child items
	ListCSwordModuleInfo modules;

	for (KeyTreeItem* c = first(); c; c = next()) {
	Q_ASSERT(c);

		ListCSwordModuleInfo childMods = c->modules();

		/*ToDo: Use the const iterators as soon as we use Qt > 3.1
		 const ListCSwordModuleInfo::const_iterator c_end = childMods.end();
		  for (ListCSwordModuleInfo::const_iterator c_it = childMods.constBegin(); c_it != c_end; ++c_it) {
		   if (!modules.contains(*c_it)) {
		    modules.append(*c_it);
		   }
		  }*/

		//   for (CSwordModuleInfo* m = childMods.first(); m; m = childMods.next()) {
		ListCSwordModuleInfo::iterator end_it = childMods.end();

		for (ListCSwordModuleInfo::iterator it(childMods.begin()); it != end_it; ++it) {
			if (!modules.contains(*it)) {
				modules.append(*it);
			}
		}

	}

	return modules;
}

const QString CTextRendering::renderKeyTree( KeyTree& tree ) {
	initRendering();

	ListCSwordModuleInfo modules = tree.collectModules();
	QString t;

	//optimization for entries with the same key
	util::scoped_ptr<CSwordKey> key(
		(modules.count() == 1) ? CSwordKey::createInstance(modules.first()) : 0
	);

	for (KeyTreeItem* c = tree.first(); c; c = tree.next()) {
		if (modules.count() == 1) { //this optimizes the rendering, only one key created for all items
			key->key( c->key() );
			t.append( renderEntry( *c, key) );
		}
		else {
			t.append( renderEntry( *c ) );
		}
	}

	return finishText(t, tree);
}

const QString CTextRendering::renderKeyRange( const QString& start, const QString& stop, const ListCSwordModuleInfo& modules, const QString& highlightKey, const KeyTreeItem::Settings& keySettings ) {

	CSwordModuleInfo* module = modules.first();
	//qWarning( "renderKeyRange start %s stop %s \n", start.latin1(), stop.latin1() );
	
	util::scoped_ptr<CSwordKey> lowerBound( CSwordKey::createInstance(module) );
	lowerBound->key(start);

	util::scoped_ptr<CSwordKey> upperBound( CSwordKey::createInstance(module) );
	upperBound->key(stop);

	sword::SWKey* sw_start = dynamic_cast<sword::SWKey*>(lowerBound.get());
	sword::SWKey* sw_stop = dynamic_cast<sword::SWKey*>(upperBound.get());

	Q_ASSERT((*sw_start == *sw_stop) || (*sw_start < *sw_stop));

	if (*sw_start == *sw_stop) { //same key, render single key
		return renderSingleKey(lowerBound->key(), modules);
	}
	else if (*sw_start < *sw_stop) { // Render range
		KeyTree tree;
		KeyTreeItem::Settings settings = keySettings;

		CSwordVerseKey* vk_start = dynamic_cast<CSwordVerseKey*>(lowerBound.get());
		Q_ASSERT(vk_start);

		CSwordVerseKey* vk_stop = dynamic_cast<CSwordVerseKey*>(upperBound.get());
		Q_ASSERT(vk_stop);

		bool ok = true;
		while (ok && ((*vk_start < *vk_stop) || (*vk_start == *vk_stop))) {
			//make sure the key given by highlightKey gets marked as current key
			settings.highlight = (!highlightKey.isEmpty() ? (vk_start->key() == highlightKey) : false);

			/*TODO: We need to take care of linked verses if we render one or (esp) more modules
			If the verses 2,3,4,5 are linked to 1, it should be displayed as one entry with the caption 1-5 */
			
			if (vk_start->Chapter() == 0){ //range was 0:0-1:x, render 0:0 first and jump to 1:0
				vk_start->Verse(0);
				tree.append( new KeyTreeItem(vk_start->key(), modules, settings) );
				vk_start->Chapter(1);
				vk_start->Verse(0);
			}
			tree.append( new KeyTreeItem(vk_start->key(), modules, settings) );
			ok = vk_start->next(CSwordVerseKey::UseVerse);
		}

		return renderKeyTree(tree);
	}

	return QString::null;
}

const QString CTextRendering::renderSingleKey( const QString& key, const ListCSwordModuleInfo& moduleList, const KeyTreeItem::Settings& settings ) {
	KeyTree tree;
	tree.append( new KeyTreeItem(key, moduleList, settings) );

	return renderKeyTree(tree);
}


