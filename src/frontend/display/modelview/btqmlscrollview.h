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

/**
  This class is part of the view implementation for the model/view read display.
  This widget contains the BtQuickWidget and a QScrollBar to its right. It implements
  the scrolling for the view.
  */


#ifndef BTQMLSCROLLVIEW_H
#define BTQMLSCROLLVIEW_H

#include <QWidget>

class BtQuickWidget;
class BtTextFilter;
class BtQmlInterface;
class BtConstModuleList;
class CReadDisplay;
class CSwordModuleInfo;
class CSwordKey;
class FilterOptions;
class QResizeEvent;
class QScrollBar;

class BtQmlScrollView : public QWidget {
    Q_OBJECT
public:

    BtQmlScrollView(QWidget * parent, CReadDisplay* readDisplay);
    ~BtQmlScrollView() override;

    /** hightlight text */
    void findTextHighlight(const QString& text, bool caseSensitive);

    BtQmlInterface* getQmlInterface() const;
    void setKey( CSwordKey* key );
    void setModules(const QStringList &modules);
    void settingsChanged();
    void pageDown();
    void pageUp();
    void referenceChoosen();
    void scrollToSwordKey(CSwordKey * key);
    void setFilterOptions(FilterOptions filterOptions);

    BtQuickWidget* quickWidget() {
        return m_quickWidget;
    }

    BtQmlInterface * qmlInterface() const {
        return m_qmlInterface;
    }

protected:
    virtual void	resizeEvent(QResizeEvent *event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
    void slotSliderMoved(int value);
    void slotSliderPressed();
    void slotSliderReleased();

signals:
    void referenceDropped(const QString& reference);

private:    void initScrollBar();

    CReadDisplay* m_readDisplay;
    BtQmlInterface* m_qmlInterface;
    BtQuickWidget* m_quickWidget;
    QScrollBar* m_scrollBar;
    int m_scrollBarPosition;
};

#endif
