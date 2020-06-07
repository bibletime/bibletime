/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btconfigdialog.h"

#include <QDialogButtonBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../messagedialog.h"


BtConfigDialog::BtConfigDialog(QWidget * const parent,
                               Qt::WindowFlags const flags)
    : QDialog(parent, flags)
    , m_contentsList(new QListWidget(this))
    , m_pageWidget(new QStackedWidget(this))
{
    QHBoxLayout * const mainLayout = new QHBoxLayout(this);

    m_contentsList->setViewMode(QListView::IconMode);
    m_contentsList->setMovement(QListView::Static);
    BT_CONNECT(m_contentsList, &QListWidget::currentRowChanged,
               m_pageWidget,   &QStackedWidget::setCurrentIndex);
    mainLayout->addWidget(m_contentsList);

    m_pageLayout = new QVBoxLayout;
    mainLayout->addLayout(m_pageLayout);

    m_pageWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                                QSizePolicy::MinimumExpanding);
    m_pageLayout->addWidget(m_pageWidget);

    QFrame * const buttonBoxRuler = new QFrame(this);
    buttonBoxRuler->setGeometry(QRect(1, 1, 1, 3));
    buttonBoxRuler->setFrameShape(QFrame::HLine);
    buttonBoxRuler->setFrameShadow(QFrame::Sunken);
    buttonBoxRuler->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_pageLayout->addWidget(buttonBoxRuler);

    // Add button box:
    auto * const bbox = new QDialogButtonBox(this);
    BT_CONNECT(bbox->addButton(QDialogButtonBox::Ok),
               &QPushButton::clicked,
               [this] { save(); close(); });
    BT_CONNECT(bbox->addButton(QDialogButtonBox::Apply),
               &QPushButton::clicked,
               [this] { save(); });
    BT_CONNECT(bbox->addButton(QDialogButtonBox::Cancel),
               &QPushButton::clicked,
               [this]{ close(); });
    message::prepareDialogBox(bbox);
    m_pageLayout->addWidget(bbox);
}

void BtConfigDialog::addPage(Page * const pageWidget) {
    m_pageWidget->addWidget(pageWidget);

    QListWidgetItem * const item = new QListWidgetItem(m_contentsList);
    item->setTextAlignment(Qt::AlignHCenter);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    pageWidget->setListWidgetItem(item);

    int const thisItemWidth = m_contentsList->visualItemRect(item).width();
    if (m_maxItemWidth < thisItemWidth) {
        // Reset the list width:
        m_maxItemWidth = thisItemWidth;
        m_contentsList->setFixedWidth(
                m_maxItemWidth + (m_contentsList->frameWidth() * 2));
    }

    // All items should have the same width:
    for (int i = 0; i < m_contentsList->count(); ++i)
        m_contentsList->item(i)->setSizeHint(
                QSize(m_maxItemWidth,
                      m_contentsList->visualItemRect(
                            m_contentsList->item(i)).height()));

    if (m_pageWidget->count() == 1)
        setCurrentPage(0);
}

void BtConfigDialog::setCurrentPage(int const newIndex)
{ m_contentsList->setCurrentRow(newIndex); }

void BtConfigDialog::save() {
    for (int i = 0; i < m_pageWidget->count(); ++i) {
        BT_ASSERT(dynamic_cast<Page *>(m_pageWidget->widget(i)));
        static_cast<Page *>(m_pageWidget->widget(i))->save();
    }
    Q_EMIT signalSettingsChanged();
}
