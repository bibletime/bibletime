/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include <QWidget>

#include <QString>
#include "../../backend/btglobal.h"
#include "modelview/btqmlscrollview.h"


class BtQmlScrollView;
class BtQmlInterface;
class CDisplayWindow;
class QMenu;

/**
  * @page modelviewmain Details about the model/view read display
  * <p>
  * In the simplest form, the model/view read display uses BtModuleTextModel
  * as the model and a <b>QML ListView</b> in Display.qml. BtModuleTextModel is a
  * subclass of QAbstractItemList. The BtModuleTextModel creates entries for
  * references that are being displayed in the ListView or those that are just
  * above and below the displayed references. As the ListView is scrolled, the
  * entries are automatically created and deleted. The entries represent verses,
  * chapters, lexicon items, etc. depending upon the module type.
  * </p>
  * <p>
  *     - For more details about the model see @ref modelviewmodel.<br/>
  *     - For details about the view, see @ref modelviewdisplay.<br/>
  * </p>
  */

/**
  * @page modelviewdisplay Details about the QML view
  * <p>
  * The class BtModelViewReadDisplay contains the functionality to display a model
  * of a module. It uses a set of widgets to do this. The hierarchy of contained
  * widgets is:<br/>
  *     - BtQmlScrollView - Contains the scrolling functionality.<br/>
  *     - BtQuickWidget - Contains the functionality for displaying QML.<br/>
  *
  * Below the BtQuickWidget is the QML file, <b>DisplayView.qml</b>. It contains
  * the ListView that displays the module entries. The QML instantiates
  * another class, BtQmlInterface, that provides properties and functions
  * that QML uses.
  * </p>
  */

/** The view implementation for the Model/View read display.
  */
class BtModelViewReadDisplay : public QWidget {
    Q_OBJECT

public: // types:

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

public: // methods:

    BtModelViewReadDisplay(CDisplayWindow * displayWindow,
                           QWidget * parent = nullptr);

    ~BtModelViewReadDisplay() override;

    void copyAnchorOnly() { copyAsPlainText(AnchorOnly); }
    void copyAnchorTextOnly() { copyAsPlainText(AnchorTextOnly); }
    void copyAnchorWithText() { copyAsPlainText(AnchorWithText); }
    void saveAnchorWithText() { save(PlainText, AnchorWithText); }
    void copyAll() { copyAsPlainText(Document); }

    /** \brief Copies the currently selected text. */
    void copySelectedText();

    /**
        \brief Copies the given text specified by asking user for first and last
               references.
    */
    void copyByReferences();

    /**
        \brief Saves the given text with the specified format into the
               applications clipboard.
    */
    bool save(TextType const format, TextPart const part);

    void print(TextPart const,
               DisplayOptions const & displayOptions,
               FilterOptions const & filterOptions);

    void printAll(DisplayOptions const & displayOptions,
                  FilterOptions const & filterOptions)
    { print(Document, displayOptions, filterOptions); }

    void printAnchorWithText(DisplayOptions const & displayOptions,
                             FilterOptions const & filterOptions)
    { print(AnchorWithText, displayOptions, filterOptions); }

    /**
        \brief Installs the popup which should be opened when the right mouse
               button was pressed.
    */
    void installPopup(QMenu * const popup) { m_popup = popup; }

    /** \returns the popup menu which was set by installPopupMenu() */
    QMenu * installedPopup() { return m_popup; }

    /**
       \param[in] format The format to use for the text.
       \param[in] part The part of the text to return.
       \returns the right text part in the specified format.
    */
    QString text(TextType const format = HTMLText,
                 TextPart const part = Document);

    void reloadModules();

    void setDisplayFocus();

    void setDisplayOptions(const DisplayOptions &displayOptions);

    /** \returns the view of this display widget. */
    QWidget * view() { return m_widget; }

    // --------------------

    void contextMenu(QContextMenuEvent * event);

    QString getCurrentSource() { return m_currentSource; }

    void highlightText(const QString& text, bool caseSensitive);

    void findText(const QString& text, bool caseSensitive, bool backward);

    void openFindTextDialog();
    QString getCurrentNodeInfo() const { return m_nodeInfo; }

    void pageDown();

    void pageUp();

    void scrollToKey(CSwordKey * key);

    void scroll(int pixels);

    void setFilterOptions(FilterOptions filterOptions);

    void setLemma(const QString& lemma);

    void setModules(QStringList const & modules);

    void settingsChanged();

    void updateReferenceText();

    BtQuickWidget * quickWidget() { return m_widget->quickWidget(); }

    BtQuickWidget const * quickWidget() const
    { return m_widget->quickWidget(); }

    BtQmlInterface * qmlInterface() const;

    /** \returns whether the display has an active anchor. */
    bool hasActiveAnchor() const { return !m_activeAnchor.isEmpty(); }

private: /* Methods: */

    void copyAsPlainText(TextPart const part);

private: // fields:

    CDisplayWindow* m_parentWindow;
    QMenu* m_popup;
    QString m_activeAnchor; //< Holds the current anchor

    QString m_currentSource;

    QString m_nodeInfo;
    int m_magTimerId;

    BtQmlScrollView* m_widget;
    QString m_currentAnchorCache;

};
