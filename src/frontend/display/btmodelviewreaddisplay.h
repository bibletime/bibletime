/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTMODELVIEWREADDISPLAY_H
#define BTMODELVIEWREADDISPLAY_H

#include "frontend/display/creaddisplay.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QPoint>
#include <QTimerEvent>
#include <QWidget>

class BtQmlScrollView;
class BtQmlInterface;

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
class BtModelViewReadDisplay : public QWidget, public CReadDisplay {
    Q_OBJECT

public:

    BtModelViewReadDisplay( CReadWindow* readWindow, QWidget* parent = nullptr );

    ~BtModelViewReadDisplay() override;

    // ---------------------------
    //reimplemented functions from CDisplay
    // Returns the right text part in the specified format.
    const QString text(const CDisplay::TextType format = CDisplay::HTMLText,
                       const CDisplay::TextPart part = CDisplay::Document)
    override;

    void setDisplayFocus() override;

    void setDisplayOptions(const DisplayOptions &displayOptions) override;

    void setText( const QString& newText ) override;

    bool hasSelection() const override;

    QWidget* view() override;

    void selectAll() override;

    // --------------------

    void contextMenu(QContextMenuEvent* event) override;

    QString getCurrentSource();

    void highlightText(const QString& text, bool caseSensitive) override;

    void findText(const QString& text, bool caseSensitive, bool backward) override;

    void moveToAnchor( const QString& anchor ) override;

    void openFindTextDialog() override;
    inline QString getCurrentNodeInfo() const override {
        return m_nodeInfo;
    }

    void pageDown();

    void pageUp();

    void scrollToKey(CSwordKey * key) override;

    void scroll(int pixels) override;

    void setFilterOptions(FilterOptions filterOptions) override;

    void setLemma(const QString& lemma);

    void setModules(const QStringList& modules) override;

    void settingsChanged();

    void updateReferenceText() override;

    BtQmlInterface * qmlInterface() const;


signals:
    void completed();

protected:

    void slotGoToAnchor(const QString& anchor);
    struct DNDData {
        bool mousePressed;
        bool isDragging;
        QString selection;
        QPoint startPos;
        enum DragType {
            Link,
            Text
        } dragType;
    }
    m_dndData;

    QString currentSource;

    QString m_nodeInfo;
    int m_magTimerId;

private slots:
    void slotDelayedMoveToAnchor();
    void slotUpdateReference(const QString& reference);
    void slotDragOccuring(const QString& moduleName, const QString& keyName);
    void slotReferenceDropped(const QString& reference);

private:

    BtQmlScrollView* m_widget;
    QString m_currentAnchorCache;

};

#endif
