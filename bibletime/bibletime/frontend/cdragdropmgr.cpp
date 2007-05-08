/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cdragdropmgr.h"

#include "backend/cswordmoduleinfo.h"
#include "backend/cswordbackend.h"
#include "backend/cswordversekey.h"
#include "util/cpointers.h"

//Sword includes
#include "versekey.h"

//Qt includes
#include <qevent.h>
#include <qdom.h>

CDragDropMgr::BTDrag::BTDrag( const QString& xml, QWidget* dragSource, const char* name)
: QTextDrag(xml, dragSource, name) {}
;

//static function to see whether we can decode tje given mime type
bool CDragDropMgr::BTDrag::canDecode( const QMimeSource * mime ) {
	if ( mime->provides("BibleTime/DND") ) { //we can decode this type!
		return true;
	}
	return false; //not yet implemented
};

bool CDragDropMgr::BTDrag::provides( const char* type ) const {
	return (type == "BibleTime/DND"); //return only true if the type is BibleTime/DND
};

const char* CDragDropMgr::BTDrag::format( int i ) const {
	if ( i == 0) { //we support only one format!
	return "BibleTime/DND";
	};
	return 0;
};

bool CDragDropMgr::BTDrag::decode(const QMimeSource* e, QString& str) {
	if (canDecode(e)) {
		str = QString( e->encodedData( "BibleTime/DND" ) );
		return true;
	}
	return false;
};

bool CDragDropMgr::BTDrag::decode(const QMimeSource* e, QString& str, QCString& /*subtype*/) {
	return decode(e, str);
};

QByteArray CDragDropMgr::BTDrag::encodedData( const char* /*type*/ ) const {
	return QTextDrag::encodedData("text/plain"); //hack because QTextDrag only accepts text/plainand not our BibleTime/DND type
};

///////////////////////////// new class //////////////////////

CDragDropMgr::Item::Item( const QString& text )
: m_type(Text),
m_bookmarkModuleName(QString::null),
m_bookmarkKey(QString::null),
m_bookmarkDescription(QString::null),
m_text(text) {}

CDragDropMgr::Item::Item( const QString& moduleName, const QString& key, const QString& description  )
: m_type(Bookmark),
m_bookmarkModuleName(moduleName),
m_bookmarkKey(key),
m_bookmarkDescription(description),
m_text(QString::null) {
	//we have to make sure the key is saved in it's english representation, so we convert it
	if (CSwordModuleInfo* mod = CPointers::backend()->findModuleByName( moduleName )) {
		if (mod->type() == CSwordModuleInfo::Bible || mod->type() == CSwordModuleInfo::Commentary) {
			CSwordVerseKey vk(0);
			vk.key( key );
			vk.setLocale("en");

			m_bookmarkKey = vk.key();
			//      qWarning("english key of %s is %s", key.latin1(), m_bookmarkKey.latin1());
		}
	}
}

CDragDropMgr::Item::~Item() {}

const CDragDropMgr::Item::Type& CDragDropMgr::Item::type() const {
	//returns the type of drag & drop action this item represents
	return m_type;
}

/** Returns the text which is used by this DragDrop Item, only valid if type() == Text */
const QString& CDragDropMgr::Item::text() const {
	//  Q_ASSERT(!m_text.isEmpty());
	return m_text;
}

/** Returns the key, ony valid if type() == Bookmark */
const QString& CDragDropMgr::Item::bookmarkKey() const {
	//  Q_ASSERT(!m_bookmarkKey.isEmpty());
	return m_bookmarkKey;
}

/** Returns the bookmark module, ony valid if type() == Bookmark */
const QString& CDragDropMgr::Item::bookmarkModule() const {
	//  Q_ASSERT(!m_bookmarkModuleName.isEmpty());
	return m_bookmarkModuleName;
}

/** Returns the bookmark description, ony valid if type() == Bookmark */
const QString& CDragDropMgr::Item::bookmarkDescription() const {
	//  Q_ASSERT(!m_bookmarkDescription.isEmpty());
	return m_bookmarkDescription;
}

////////////////////////////////// NEW CLASS //////////////////////////

CDragDropMgr::CDragDropMgr() {}

CDragDropMgr::~CDragDropMgr() {}

const bool CDragDropMgr::canDecode( const QMimeSource* const mime ) {
	if (CDragDropMgr::BTDrag::canDecode(mime)) {
		return true;
	}
	else if( QTextDrag::canDecode(mime) ) {
		qWarning("QTextDrag can decode this mime!");
		return true;
	};
	return false;
};

QDragObject* const CDragDropMgr::dragObject( CDragDropMgr::ItemList& items, QWidget* dragSource ) {
	if ( items.count() ) {
		//process the items and set the data to the dragobject we return later
		QDomDocument doc("DOC");
		doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
		QDomElement content = doc.createElement("BibleTimeDND");
		content.setAttribute("syntaxVersion", "1.0");
		doc.appendChild(content);

		CDragDropMgr::ItemList::iterator it;
		for ( it = items.begin(); it != items.end(); ++it ) {
			Item item = (*it);
			if (item.type() == Item::Bookmark) { //a bookmark was dragged
				//append the XML stuff for a bookmark
				QDomElement bookmark = doc.createElement("BOOKMARK");
				bookmark.setAttribute("key", item.bookmarkKey());
				bookmark.setAttribute("description", item.bookmarkDescription());
				bookmark.setAttribute("moduleName", item.bookmarkModule());

				content.appendChild(bookmark);
			}
			else if (item.type() == Item::Text) { //plain text was dragged
				//append the XML stuff for plain text
				QDomElement plainText = doc.createElement("TEXT");
				plainText.setAttribute("text", item.text());

				content.appendChild(plainText);
			}
		}

		BTDrag* dragObject = new BTDrag( doc.toString(), dragSource );
		//    qWarning("DND data created: %s", (const char*)doc.toString().utf8());
		return dragObject;
	};
	return 0;
};

CDragDropMgr::ItemList CDragDropMgr::decode( const QMimeSource* const  src) {
	//if the drag was started by another widget which doesn't use CDragDropMgr (a drag created by QTextDrag)
	if (canDecode(src) && QTextDrag::canDecode(src)) { //if we can decode but it's a QTextDrag and not a BTDrag object
		QString text;
		QTextDrag::decode(src, text);
		//    qWarning(text.latin1());

		CDragDropMgr::ItemList dndItems;
		dndItems.append( Item(text) );
		return dndItems;
	}
	else if (!canDecode(src)) { //if we can't decode it
		return CDragDropMgr::ItemList();
	};

	QString xmlData;
	BTDrag::decode(src, xmlData);

	if (xmlData.isEmpty()) { //something went wrong!
		//    qWarning("CDragDropMgr::decode: empty xml data!");
		return CDragDropMgr::ItemList();
	}
	//  else {
	//    qWarning("Drag&Drop data is: %s", xmlData.latin1());
	//  }

	//we can handle the dropEvent and have xml data to work on!
	ItemList dndItems;

	QDomDocument doc;
	doc.setContent( xmlData );

	QDomElement document = doc.documentElement();
	if( document.tagName() != "BibleTimeDND" ) { //BibleTime was used in syntax version 1.0
		qWarning("DragDropMgr::decode: Missing BibleTimeDND doc");
		return CDragDropMgr::ItemList();
	}
	// see if there's a section with the name MAINWINDOW
	QDomElement elem = document.firstChild().toElement();
	while (!elem.isNull()) {
		if (elem.tagName() == "BOOKMARK") { //we found a bookmark!
			//        qWarning("found a bookmark!");
			const QString key = elem.hasAttribute("key") ? elem.attribute("key") : QString::null;
			const QString moduleName = elem.hasAttribute("moduleName") ? elem.attribute("moduleName") : QString::null;
			const QString description = elem.hasAttribute("description") ? elem.attribute("description") : QString::null;

			dndItems.append( CDragDropMgr::Item(moduleName, key, description) );
		}
		else if (elem.tagName() == "TEXT") { //we found a plain text passage!
			const QString text = elem.hasAttribute("text") ? elem.attribute("text") : QString::null;
			dndItems.append( CDragDropMgr::Item(text) );
		};
		elem = elem.nextSibling().toElement();
	};

	return dndItems;
};

/** Returns which type the given drop event has, if it's a mixed one (both bookmarks and plain text), which shouldn't happen, it return Item::Unknown. */
CDragDropMgr::Item::Type CDragDropMgr::dndType( const QMimeSource* e ) {
	ItemList dndItems = decode(e);
	if (dndItems.isEmpty()) {//wrong dropEvent or something strange
		return Item::Unknown;
	};

	//check whether all items have the ssame type, if they do return the type
	//as soon as two items have different types return Item::Unknown
	ItemList::Iterator it;
	Item::Type type = Item::Unknown;
	for( it = dndItems.begin(); it != dndItems.end(); ++it ) {
		if( type == Item::Unknown) { //if Unknown is set this is the first loop, don't return Unknown
			type = (*it).type();
		}
		else if (type != (*it).type() ) {//items have different type, return Item::Unknown
			return Item::Unknown;
		};
	};
	return type;
}
