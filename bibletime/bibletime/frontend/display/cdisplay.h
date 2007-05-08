/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CDISPLAY_H
#define CDISPLAY_H

//BibleTime includes
#include "util/cpointers.h"
#include "backend/cswordbackend.h"

//Qt includes
#include <qobject.h>
#include <qstring.h>
#include <qmap.h>


class CDisplayConnections;
class CReadWindow;
class CWriteWindow;

class CDisplayWindow;
class CReadDisplay;
class CWriteDisplay;


class QPopupMenu;

/** The base class for all display widgets.
  * @author The BibleTime team
  */
class CDisplay : public CPointers {
public:
	enum WriteDisplayType {
		HTMLDisplay = 0,
		PlainTextDisplay
	};

	static CReadDisplay* createReadInstance(CReadWindow* readWindow, QWidget* parent = 0);
	static CWriteDisplay* createWriteInstance( CWriteWindow* writeWindow, const WriteDisplayType& type = PlainTextDisplay, QWidget* parent = 0 );

	enum TextType {
		HTMLText, /* Used for HTML markup */
		PlainText /* Plain text without links etc. */
	};
	enum TextPart {
		Document, /* All text */
		SelectedText, /* Only the selected text */
		AnchorOnly,
		AnchorTextOnly,
		AnchorWithText
	};

	/**
	* Copies the given text with the specified format into the applications clipboard.
	*/
	virtual const bool copy( const CDisplay::TextType format, const CDisplay::TextPart part );
	/**
	* Saves the given text with the specified format into the applications clipboard.
	*/
	virtual const bool save( const CDisplay::TextType format, const CDisplay::TextPart part );

	//the pure virtual methods of this base class

	/** Returns the text in the given format.
	*
	*/
	virtual const QString text( const CDisplay::TextType format = CDisplay::HTMLText, const CDisplay::TextPart part = CDisplay::Document ) = 0;
	/**
	* Sets the new text for this display widget.
	*/
	virtual void setText( const QString& newText ) = 0;
	/**
	* Returns true if the display widget has a selection. Otherwise false.
	*/
	virtual const bool hasSelection() = 0;
	/**
	* Returns the view of this display widget.
	*/
	virtual QWidget* view() = 0;
	/**
	*  Selects the document text.
	*/
	virtual void selectAll() = 0;
	/**
	* Returns the connections obect used for signas and slots.
	*/
	virtual CDisplayConnections* const connectionsProxy() const;
	/**
	* Returns the parent window used for this display widget.
	*/
	CDisplayWindow* const parentWindow() const;
	virtual void print( const CDisplay::TextPart, CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions) = 0;
	/**
	* Installs the popup which should be opened when the right mouse button was pressed.
	*/
	void installPopup( QPopupMenu* popup );
	/**
	* Returns the popup menu which was set by installPopupMenu()
	*/
	QPopupMenu* const installedPopup();

	virtual void zoomIn() {}
	virtual void zoomOut() {}
	virtual void openFindTextDialog() {}

	enum NodeInfoType {
		Lemma
	};
	
	
	virtual QMap<NodeInfoType, QString> getCurrentNodeInfo() {
		return QMap<NodeInfoType, QString>();
	}
	
protected:
	/**
	* Used when a reference was dropped onto the widget.
	*/
	void emitReferenceDropped( const QString& reference );
	/**
	* Emits the signal which used when a reference was clicked.
	*/
	void emitReferenceClicked( const QString& reference );

protected:
	CDisplay(CDisplayWindow* parent);
	virtual ~CDisplay();

private:
	CDisplayWindow* m_parentWindow;
	CDisplayConnections* m_connections;
	QPopupMenu* m_popup;
};

class CDisplayConnections : public QObject {
	Q_OBJECT
public:
	CDisplayConnections( CDisplay* parent );

public slots:
	virtual void selectAll();
	void emitReferenceClicked( const QString& module, const QString& key);
	void emitReferenceDropped( const QString& key );
	void emitTextChanged();

	//stuff which works in every CDisplay
	void saveAsPlain();
	void saveAsHTML();
	void saveAnchorWithText();

	void printAll(CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions);
	void printAnchorWithText(CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions);

	void copySelection();
	void copyAll();
	void copyAnchorWithText();
	void copyAnchorTextOnly();
	void copyAnchorOnly();

	void clear();

	void zoomIn();
	void zoomOut();

	void openFindTextDialog();

signals:
	void referenceClicked(const QString& module, const QString& key);
	void referenceDropped(const QString& key);
	void textChanged();

private:
	CDisplay* m_display;

	struct {
		QString module;
		QString key;
	} m_referenceClickedCache;
};

#endif
