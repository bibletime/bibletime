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

#include "btqmlscrollview.h"
#include "../../bibletime.h"

#include <QHBoxLayout>
#include <QList>
#include <QMenu>
#include <QMetaObject>
#include <QMimeData>
#include <QQmlError>
#include <QQuickItem>
#include <QQuickWidget>
#include <QResizeEvent>
#include <QScrollBar>
#include <QStringList>
#include <QStandardItemModel>
#include <QStandardItem>
#include "../../../util/btconnect.h"
#include "../../display/btmodelviewreaddisplay.h"
#include "../../display/btmodelviewreaddisplay.h"
#include "../../display/modelview/btqmlinterface.h"
#include "btquickwidget.h"


BtQmlScrollView::BtQmlScrollView(QWidget * parent,
                                 BtModelViewReadDisplay * readDisplay)
    : QWidget(parent),
      m_readDisplay(readDisplay),
      m_qmlInterface(nullptr),
      m_quickWidget(new BtQuickWidget(this)),
      m_scrollBar(new QScrollBar),
      m_scrollBarPosition(0) {
    auto layout = new QHBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(m_quickWidget);
    layout->addWidget(m_scrollBar);
    setLayout(layout);
    initScrollBar();

    m_quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_quickWidget->show();

    m_qmlInterface = m_quickWidget->rootObject()->findChild<BtQmlInterface*>();

    BT_CONNECT(m_quickWidget, &BtQuickWidget::referenceDropped,
               this, &BtQmlScrollView::referenceDropped);
}

BtQmlScrollView::~BtQmlScrollView() = default;

// Create the right mouse context menus
void BtQmlScrollView::contextMenuEvent(QContextMenuEvent* event) {

    // Save ListView index for later use
    int x = event->x();
    int y = event->y();
    m_quickWidget->saveContextMenuIndex(x, y);

    m_readDisplay->contextMenu(event);
}

void BtQmlScrollView::initScrollBar() {
    m_scrollBar->setRange(-100,100);
    m_scrollBar->setValue(0);
    BT_CONNECT(m_scrollBar, &QScrollBar::sliderMoved,
               this, &BtQmlScrollView::slotSliderMoved);
    BT_CONNECT(m_scrollBar, &QScrollBar::sliderPressed,
               this, &BtQmlScrollView::slotSliderPressed);
    BT_CONNECT(m_scrollBar, &QScrollBar::sliderReleased,
               this, &BtQmlScrollView::slotSliderReleased);
}

void BtQmlScrollView::slotSliderMoved(int value) {
    int speed = 25 * (1 + std::abs(value/30));
    int relative = value - m_scrollBarPosition;
    m_quickWidget->scroll(relative * speed);
    m_scrollBarPosition = value;
    BibleTime::instance()->autoScrollStop();
}

void BtQmlScrollView::slotSliderPressed()  {
    m_scrollBarPosition = m_scrollBar->value();
}

void BtQmlScrollView::slotSliderReleased() {
    m_scrollBar->setValue(0);
    m_quickWidget->updateReferenceText();
}

void BtQmlScrollView::setModules(const QStringList &modules) {
    m_qmlInterface->setModules(modules);
}

void BtQmlScrollView::scrollToSwordKey(CSwordKey * key) {
    m_qmlInterface->scrollToSwordKey(key);
}

void BtQmlScrollView::referenceChoosen() {
    m_qmlInterface->referenceChosen();
}

void BtQmlScrollView::settingsChanged() {
    m_qmlInterface->settingsChanged();
}

void BtQmlScrollView::pageDown() { m_quickWidget->pageDown(); }

void BtQmlScrollView::pageUp() { m_quickWidget->pageUp(); }

void BtQmlScrollView::findTextHighlight(const QString& text, bool caseSensitive) {
    m_qmlInterface->setHighlightWords(text, caseSensitive);
}

void BtQmlScrollView::setFilterOptions(FilterOptions filterOptions) {
    m_qmlInterface->setFilterOptions(filterOptions);
}
