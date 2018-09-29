/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef CDISPLAY_H
#define CDISPLAY_H

#include <QMap>
#include "backend/btglobal.h"
#include "backend/managers/cswordbackend.h"


class CDisplayConnections;
class CDisplayWindow;
class QMenu;

/** The base class for all display widgets.
  * @author The BibleTime team
  */
class CDisplay {
public:

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

    virtual void scrollToKey(CSwordKey* key);

    virtual void setModules(const QStringList& modules);

    /**
        * Copies the given text with the specified format into the applications clipboard.
        */
    virtual bool copy( const CDisplay::TextType format, const CDisplay::TextPart part );
    /**
        * Saves the given text with the specified format into the applications clipboard.
        */
    virtual bool save( const CDisplay::TextType format, const CDisplay::TextPart part );

    virtual void highlightText(const QString& /*text*/, bool /*caseSensitive*/) {}

    virtual void findText(const QString& /*text*/, bool /*caseSensitive*/, bool /*backward*/) {}

    //the pure virtual methods of this base class

    /** Returns the text in the given format.
        *
        */
    virtual const QString text( const CDisplay::TextType format = CDisplay::HTMLText,
                                const CDisplay::TextPart part = CDisplay::Document ) = 0;
    /**
        * Sets the new text for this display widget.
        */
    virtual void setText( const QString& newText ) = 0;
    /**
        * Returns true if the display widget has a selection. Otherwise false.
        */
    virtual bool hasSelection() const = 0;
    /**
        * Returns the view of this display widget.
        */
    virtual QWidget* view() = 0;
    /**
        *  Selects the document text.
        */
    virtual void selectAll() = 0;
    /**
        * Returns the connections object used for signals and slots.
        */
    virtual CDisplayConnections* connectionsProxy() const;
    /**
        * Returns the parent window used for this display widget.
        */
    CDisplayWindow* parentWindow() const;

    virtual void print(const CDisplay::TextPart,
                       const DisplayOptions &displayOptions,
                       const FilterOptions &filterOptions) = 0;
    /**
        * Installs the popup which should be opened when the right mouse button was pressed.
        */
    void installPopup( QMenu* popup );
    /**
        * Returns the popup menu which was set by installPopupMenu()
        */
    QMenu* installedPopup();

    virtual void openFindTextDialog() {}

    virtual void setFilterOptions(FilterOptions /* filterOptions*/) {}

    inline virtual QString getCurrentNodeInfo() const {
        return QString::null;
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
    QMenu* m_popup;
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

    void printAll(const DisplayOptions &displayOptions,
                  const FilterOptions &filterOptions);

    void printAnchorWithText(const DisplayOptions &displayOptions,
                             const FilterOptions &filterOptions);

    void copySelection();
    void copyAll();
    void copyAnchorWithText();
    void copyAnchorTextOnly();
    void copyAnchorOnly();

    void clear();

    void openFindTextDialog();

signals:
    void referenceClicked(const QString& module, const QString& key);
    void referenceDropped(const QString& key);
    void textChanged();

private:
    CDisplay* m_display;

};

#endif
