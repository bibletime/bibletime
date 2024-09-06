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

#include "../../backend/btglobal.h"


class BtQmlInterface;
class BtQuickWidget;
class CDisplayWindow;
class CSwordKey;
class QMenu;
class QScrollBar;

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
    void saveAnchorWithText() { save(AnchorWithText); }
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
    void save(TextPart const part);

    void print(TextPart const,
               DisplayOptions const & displayOptions,
               FilterOptions const & filterOptions);

    void printAll(DisplayOptions const & displayOptions,
                  FilterOptions const & filterOptions)
    { print(Document, displayOptions, filterOptions); }

    void printAnchorWithText(DisplayOptions const & displayOptions,
                             FilterOptions const & filterOptions)
    { print(AnchorWithText, displayOptions, filterOptions); }

#ifdef BUILD_TEXT_TO_SPEECH
    /** \brief Speaks the currently selected text (text-to-speech). */
    void speakSelectedText();
#endif

    /**
        \brief Installs the popup which should be opened when the right mouse
               button was pressed.
    */
    void installPopup(QMenu * const popup) { m_popup = popup; }

    /**
       \param[in] format The format to use for the text.
       \param[in] part The part of the text to return.
       \returns the right text part in the specified format.
    */
    QString text(TextPart const part = Document);

    void reloadModules();

    void setDisplayFocus();

    void setDisplayOptions(const DisplayOptions &displayOptions);

    void highlightText(const QString& text, bool caseSensitive);

    void findText(bool const backward);

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

    BtQuickWidget * quickWidget() const noexcept { return m_quickWidget; }

    BtQmlInterface * qmlInterface() const noexcept { return m_qmlInterface; }

    /** \returns whether the display has an active anchor. */
    bool hasActiveAnchor() const { return !m_activeAnchor.isEmpty(); }

private Q_SLOTS:

    void setBibleReference (const QString& reference);
    void slotSliderMoved(int value);
    void slotSliderPressed();
    void slotSliderReleased();

private: /* Methods: */

    void copyAsPlainText(TextPart const part);
    void contextMenuEvent(QContextMenuEvent * event) final override;

private: // fields:

    CDisplayWindow* m_parentWindow;
    QMenu * m_popup = nullptr;
    QString m_activeAnchor; //< Holds the current anchor

    QString m_nodeInfo;

    BtQuickWidget * const m_quickWidget;
    BtQmlInterface * const m_qmlInterface;
    QScrollBar * const m_scrollBar;
    int m_scrollBarPosition = 0;

};
