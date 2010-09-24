/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfmanager/btconfigdialog.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListView>
#include <QListWidgetItem>
#include <QStackedWidget>
#include <QVBoxLayout>
#include "util/tool.h"


BtConfigDialog::BtConfigDialog(QWidget* parent)
        : QDialog(parent),
        m_maxItemWidth(0),
        m_previousPageIndex(-2) {
    setWindowFlags(Qt::Window);
    m_contentsList = new QListWidget(this);
    m_contentsList->setViewMode(QListView::IconMode);
    m_contentsList->setMovement(QListView::Static);

    m_pageWidget = new QStackedWidget(this);
    m_pageWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(m_contentsList);
    m_pageLayout = new QVBoxLayout;
    mainLayout->addLayout(m_pageLayout);

    m_pageLayout->addWidget(m_pageWidget);

    connect(m_contentsList,
            SIGNAL(currentRowChanged(int)),
            this, SLOT(slotChangePage(int))
           );

}

BtConfigDialog::~BtConfigDialog() {}

void BtConfigDialog::addPage(BtConfigPage* pageWidget) {
    // this is a friend
    pageWidget->m_parentDialog = this;

    m_pageWidget->addWidget(pageWidget);


    QListWidgetItem* item = new QListWidgetItem(m_contentsList);
    item->setIcon(pageWidget->icon());
    item->setText(pageWidget->header());
    item->setTextAlignment(Qt::AlignHCenter);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    //set the list width - it may bee too wide (if there were no pages) or too narrow
    if (m_maxItemWidth < m_contentsList->visualItemRect(item).width()) {
        m_maxItemWidth = m_contentsList->visualItemRect(item).width();
        m_contentsList->setFixedWidth( m_maxItemWidth + (m_contentsList->frameWidth()*2) );
    }
    // all items should has the same width
    for (int i = 0; i < m_contentsList->count(); ++i) {
        m_contentsList->item(i)->setSizeHint(QSize(m_maxItemWidth, m_contentsList->visualItemRect(m_contentsList->item(i)).height()) );
    }

    slotChangePage(m_contentsList->row(item));
}

void BtConfigDialog::addButtonBox(QDialogButtonBox* box) {
    // First add a horizontal ruler:
    QFrame *line = new QFrame();
    line->setGeometry(QRect(1, 1, 1, 3));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_pageLayout->addWidget(line);

    // Add button box:
    m_pageLayout->addWidget(box);
}

void BtConfigDialog::slotChangePage(int newIndex) {
    if (m_previousPageIndex != newIndex) {
        m_previousPageIndex = newIndex;
        m_contentsList->setCurrentRow(newIndex);
        m_pageWidget->setCurrentIndex(newIndex);
    }
}



BtConfigPage::BtConfigPage(QWidget *parent)
    : QWidget(parent), m_labelInitialized(false)
{
    QVBoxLayout *containerLayout = new QVBoxLayout(this);
    m_headerLabel = new QLabel(this);
    containerLayout->addWidget(m_headerLabel);
    setLayout(containerLayout);
}

BtConfigPage::~BtConfigPage() {
    // Intentionally empty
}

void BtConfigPage::showEvent(QShowEvent *event) {
    if (!m_labelInitialized) {
        initializeLabel();
    }

    QWidget::showEvent(event);
}

void BtConfigPage::initializeLabel() {
    util::tool::initExplanationLabel(m_headerLabel, header(), label());
    m_labelInitialized = true;
}
