/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/display/modelview/btqmlscrollview.h"

#include "frontend/display/modelview/btquickwidget.h"
#include "frontend/display/modelview/bttextfilter.h"
#include "frontend/display/btmodelviewreaddisplay.h"
#include "frontend/display/creaddisplay.h"
#include "frontend/BtMimeData.h"
#include "backend/drivers/btmodulelist.h"
#include "backend/models/btmoduletextmodel.h"
#include "frontend/display/modelview/btqmlinterface.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "util/btconnect.h"
#include "util/directory.h"
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


BtQmlScrollView::BtQmlScrollView(QWidget * parent, CReadDisplay* readDisplay)
    : QWidget(parent),
      m_readDisplay(readDisplay),
      m_qmlInterface(nullptr),
      m_quickWidget(new BtQuickWidget),
      m_scrollBar(new QScrollBar),
      m_scrollBarPosition(0) {

    setObjectName("BtQmlScrollView");
    m_quickWidget->setObjectName("BtQuickWidget");
    m_scrollBar->setObjectName("QScrollBar");
    auto layout = new QHBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(m_quickWidget);
    layout->addWidget(m_scrollBar);
    setLayout(layout);
    initScrollBar();

    m_quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_quickWidget->show();

    QQuickItem* root = m_quickWidget->rootObject();
    m_qmlInterface = root->findChild<BtQmlInterface*>();

    BT_CONNECT(m_quickWidget, SIGNAL(referenceDropped(const QString&)),
               this, SIGNAL(referenceDropped(const QString&)));
}

BtQmlScrollView::~BtQmlScrollView() {
}

// Create the right mouse context menus
void BtQmlScrollView::contextMenuEvent(QContextMenuEvent* event) {

    // Save ListView index for later use
    int x = event->x();
    int y = event->y();
    m_quickWidget->saveContextMenuIndex(x, y);

    auto readDisplay = dynamic_cast<BtModelViewReadDisplay*>(m_readDisplay);
    readDisplay->contextMenu(event);
}

void BtQmlScrollView::initScrollBar() {
    m_scrollBar->setRange(-100,100);
    m_scrollBar->setValue(0);
    BT_CONNECT(m_scrollBar, SIGNAL(sliderMoved(int)), this, SLOT(slotSliderMoved(int)));
    BT_CONNECT(m_scrollBar, SIGNAL(sliderPressed()), this, SLOT(slotSliderPressed()));
    BT_CONNECT(m_scrollBar, SIGNAL(sliderReleased()), this, SLOT(slotSliderReleased()));
}

void BtQmlScrollView::slotSliderMoved(int value) {
    int speed = 30;
    int relative = value - m_scrollBarPosition;
    m_quickWidget->scroll(relative * speed);
    m_scrollBarPosition = value;
}

void BtQmlScrollView::slotSliderPressed()  {
    m_scrollBarPosition = m_scrollBar->value();
}

void BtQmlScrollView::slotSliderReleased() {
    m_scrollBar->setValue(0);
    m_quickWidget->updateReferenceText();
}


BtQmlInterface* BtQmlScrollView::getQmlInterface() const {
    return m_qmlInterface;
}

void BtQmlScrollView::setModules(const QStringList &modules) {
    m_qmlInterface->setModules(modules);
}

/** Sets the new sword key. */
void BtQmlScrollView::setKey( CSwordKey* key ) {
    BT_ASSERT(key);
    m_qmlInterface->setKey(key);
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

void BtQmlScrollView::pageDown() {
    m_qmlInterface->pageDown();
}

void BtQmlScrollView::pageUp() {
    m_qmlInterface->pageUp();
}

void BtQmlScrollView::findTextHighlight(const QString& text, bool caseSensitive) {
    m_qmlInterface->setHighlightWords(text, caseSensitive);
}

void BtQmlScrollView::setFilterOptions(FilterOptions filterOptions) {
    m_qmlInterface->setFilterOptions(filterOptions);
}

void BtQmlScrollView::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
}

