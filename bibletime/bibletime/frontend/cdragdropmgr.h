/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CDRAGDROPMGR_H
#define CDRAGDROPMGR_H

//Qt includes
#include <qstring.h>
#include <qcstring.h>
#include <qdragobject.h>
#include <qvaluelist.h>

//forward declarations
class QDropEvent;
class QWidget;

/** This class is the drag'n' drop manager for BibleTime.
 * The dnd is managed by XML code, which defines the type of the drag/drop, the items and the action (moe, copy etc.).
 * It's possible to create the XML code by passing a list of keys etc.
 *
 * Functions:
 *  - dnd_type(), returns either CDragDropMgr::Action_Drag or CDragDropMgr::Action_Drop
 *  - dnd_action(), returns either CDragDropMgr::Move or CDragDropMgr::Copy
 *  - dnd_acceptDrop(), returns true or false. True if the drop could be handled by the CDragDropMgr class, otherwise false
 *
 *
 * The functions which create the XML stuff can't be reimplemented, too. Otherwise we could not guarantee for good XML code
 * which works everywhere.
 *
 * @author The BibleTime team
 */
class CDragDropMgr  {
public:
	//The class which represents one single drag&drop entry (e.g. a bookmark or a portion of text)
	class Item {
public:
		/**
		* The possible types of Drag&Drop actions.
		*/
		enum Type {
			Bookmark = 0, /* A bookmark: Has a key, a module and a description*/
			Text, /* Simple text, e.g. can be dropped on a module to start a search in this module using the dropped text */
			Unknown /* For situatiosn like CDragDropMgr::dndType */
		};
		/**
		* This function returns the type of drag this item has
		*/
		const CDragDropMgr::Item::Type& type() const;
		/**
		* Returns the key, ony valid if type() == Bookmark
		*/
		const QString& bookmarkKey() const;
		/**
		* Returns the module name, ony valid if type() == Bookmark
		*/
		const QString& bookmarkModule() const;
		/**
		* Returns the bookmark description, ony valid if type() == Bookmark
		*/
		const QString& bookmarkDescription() const;
		/**
		* Returns the text which is used by this DragDrop Item, only valid if type() == Text
		*/
		const QString& text() const;

		//  protected:
		friend class CDragDropMgr;
		/*
		* We use protected constructor and destructor because creation of objects
		* of this class sould only be possible for CDragDropMgr
		*/

		/** Constructor for a text item
		* This constructor automatically sets the type member to Text
		* This is also the default constructor
		*/
		Item(const QString& text = QString::null );
		/** Constructor for a Bookmark item
		* This constructor automatically sets the type member to Bookmark
		*/
		Item(const QString& moduleName, const QString& key, const QString& description);
		virtual ~Item();

private:
		Type m_type; //the member to save the type of the action
		QString m_bookmarkModuleName; //the modules which is used by this item, only valid for type() == Bookmark
		QString m_bookmarkKey; //the key of a bookmark, only valid if type() == Bookmark
		QString m_bookmarkDescription; //the description of a bookmark, only valid if type() == Bookmark
		QString m_text; //the text of this item, only valid if type() == Text
	}
	; //end of class CDragDropMgr::Item

	//the item list we're using
	typedef QValueList<Item> ItemList;

	/** Return whether the drop should be accepted
	* This functions tests whether the drop should be accepted or not. It returns true if the drop object
	* is supported by the CDragDropMgr and if it cotains valid data. Oterwise this function returns false.
	*/
	static const bool canDecode( const QMimeSource* const mime );
	/**
	* This function returns the drag object with the data which represents the items given as parameter
	* If the list is invalid or empty we return NULL.
	*/
	static QDragObject* const dragObject( CDragDropMgr::ItemList& items, QWidget* dragSource );

	/**
	* Decodes the XML stuff we passed to the dragObject at creation time.
	* Returns a list of CDragDropMgr::Item objects.
	* If it's a wrong dropEvent we return an empty ist
	*/
	static CDragDropMgr::ItemList decode( const QMimeSource* const src  );
	/**
	* Returns which type the given drop event has, if it's a mixed one (both bookmarks and plain text),
	* which shouldn't happen, it return Item::Unknown.
	* It also returns Unknown if the drop event is not supported.
	*/
	static CDragDropMgr::Item::Type dndType( const QMimeSource* e );

protected:
	//The class which represents our XML drag object stuff
class BTDrag : public QTextDrag {
public:
		BTDrag( const QString& xml, QWidget* dragSource = 0, const char* name = 0);
		//reimplemented static publoc function to provide functionality for BibleTime XML drags
		static bool canDecode( const QMimeSource * e );
		virtual bool provides( const char* type ) const;
		virtual const char* format( int i = 0 ) const;

		virtual QByteArray encodedData( const char* type ) const;

protected:
		friend class CDragDropMgr;
		//made protected because the BibleTime classes may not manage the data of BTDrag
		//    virtual void setText(const QString& text);

		//made protected because the BibleTime classes should not manage the DRag&Drop stuff themself
		static bool decode(const QMimeSource* e, QString& str);
		static bool decode(const QMimeSource* e, QString& str, QCString& subtype);
	};

	//protected constructor and destructor because we do not allow inheritance, functionality is provided by static functions
	CDragDropMgr();
	virtual ~CDragDropMgr();
};

#endif
