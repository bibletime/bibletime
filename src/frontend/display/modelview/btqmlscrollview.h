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

/**
  This class is part of the view implementation for the model/view read display.
  This widget contains the BtQuickWidget and a QScrollBar to its right. It implements
  the scrolling for the view.
  */

#include <QWidget>


class BtQuickWidget;
class BtTextFilter;
class BtQmlInterface;
class BtModelViewReadDisplay;
class CSwordModuleInfo;
class CSwordKey;
struct FilterOptions;
class QResizeEvent;
class QScrollBar;

class BtQmlScrollView : public QWidget {
    Q_OBJECT
public:

    BtQmlScrollView(QWidget * parent, BtModelViewReadDisplay * readDisplay);
    ~BtQmlScrollView() override;

    void settingsChanged();
    void pageDown();
    void pageUp();
    void scrollToSwordKey(CSwordKey * key);
    void setFilterOptions(FilterOptions filterOptions);

    BtQuickWidget* quickWidget() {
        return m_quickWidget;
    }

    BtQmlInterface * qmlInterface() const {
        return m_qmlInterface;
    }

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

private Q_SLOTS:
    void slotSliderMoved(int value);
    void slotSliderPressed();
    void slotSliderReleased();

Q_SIGNALS:
    void referenceDropped(const QString& reference);

private:    void initScrollBar();

    BtModelViewReadDisplay * m_readDisplay;
    BtQmlInterface* m_qmlInterface;
    BtQuickWidget* m_quickWidget;
    QScrollBar* m_scrollBar;
    int m_scrollBarPosition;
};
