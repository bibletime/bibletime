/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
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


BtConfigDialog::BtConfigDialog(QWidget* parent, Qt::WindowFlags flags)
        : QDialog(parent, flags)
        , m_buttonBoxRuler(0)
        , m_buttonBox(0)
        , m_maxItemWidth(0)
        , m_previousPageIndex(-2)
{
    m_contentsList = new QListWidget(this);
    m_contentsList->setViewMode(QListView::IconMode);
    m_contentsList->setMovement(QListView::Static);

    m_pageWidget = new QStackedWidget(this);
    m_pageWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    m_pageLayout = new QVBoxLayout;
    m_pageLayout->addWidget(m_pageWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(m_contentsList);
    mainLayout->addLayout(m_pageLayout);

    connect(m_contentsList, SIGNAL(currentRowChanged(int)),
            this,           SLOT(slotChangePage(int)));
}

void BtConfigDialog::addPage(Page* pageWidget) {

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

    setCurrentPage(m_contentsList->row(item));
}

void BtConfigDialog::setButtonBox(QDialogButtonBox *box) {
    Q_ASSERT(box != 0);
    Q_ASSERT(m_buttonBox == 0);
    Q_ASSERT(m_buttonBoxRuler == 0);

    m_buttonBox = box;

    // First add a horizontal ruler:
    m_buttonBoxRuler = new QFrame(this);
    m_buttonBoxRuler->setGeometry(QRect(1, 1, 1, 3));
    m_buttonBoxRuler->setFrameShape(QFrame::HLine);
    m_buttonBoxRuler->setFrameShadow(QFrame::Sunken);
    m_buttonBoxRuler->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_pageLayout->addWidget(m_buttonBoxRuler);

    // Add button box:
    m_pageLayout->addWidget(box);
}

void BtConfigDialog::setCurrentPage(int newIndex) {
    if (m_previousPageIndex != newIndex) {
        m_previousPageIndex = newIndex;
        m_contentsList->setCurrentRow(newIndex);
        m_pageWidget->setCurrentIndex(newIndex);
    }
}

void BtConfigDialog::slotChangePage(int newIndex) {
    /*
      This check is in place here because this slot is indirectly called by the
      setCurrentPage method.
    */
    if (m_previousPageIndex != newIndex) {
        m_previousPageIndex = newIndex;
        m_pageWidget->setCurrentIndex(newIndex);
    }
}
