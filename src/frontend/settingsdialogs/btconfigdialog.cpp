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

#include "btconfigdialog.h"

#include <QDialogButtonBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QListView>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QRect>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QVBoxLayout>
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../messagedialog.h"


namespace {

void resizeListWidget(QListWidget & listWidget) {
    BT_ASSERT(listWidget.count() > 0);
    listWidget.setFixedWidth(
                listWidget.visualItemRect(listWidget.item(0)).width()
                + listWidget.frameWidth() * 2);
}

} // anonymous namespace

BtConfigDialog::Page::Page(QIcon const & icon, QWidget * parent)
    : QWidget(parent)
    , m_listWidgetItem(new QListWidgetItem)
{
    try {
        m_listWidgetItem->setIcon(icon);
        m_listWidgetItem->setTextAlignment(Qt::AlignHCenter);
        m_listWidgetItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    } catch (...) {
        delete m_listWidgetItem;
        throw;
    }
}

BtConfigDialog::Page::~Page() noexcept {
    if (m_ownsListWidgetItem)
        delete m_listWidgetItem;
}

void BtConfigDialog::Page::setHeaderText(QString const & headerText)
{ m_listWidgetItem->setText(headerText); }


BtConfigDialog::BtConfigDialog(QWidget * const parent,
                               Qt::WindowFlags const flags)
    : QDialog(parent, flags)
    , m_contentsList(new QListWidget(this))
    , m_pageWidget(new QStackedWidget(this))
{
    QHBoxLayout * const mainLayout = new QHBoxLayout(this);

    m_contentsList->setUniformItemSizes(true);
    m_contentsList->setViewMode(QListView::IconMode);
    m_contentsList->setMovement(QListView::Static);
    BT_CONNECT(m_contentsList, &QListWidget::currentRowChanged,
               m_pageWidget,   &QStackedWidget::setCurrentIndex);
    BT_CONNECT(m_contentsList, &QListWidget::itemChanged,
               [this]{ resizeListWidget(*m_contentsList); });
    mainLayout->addWidget(m_contentsList);

    auto * const pageLayout = new QVBoxLayout;
    try {
        mainLayout->addLayout(pageLayout);
    } catch (...) {
        delete pageLayout;
        throw;
    }

    m_pageWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                                QSizePolicy::MinimumExpanding);
    pageLayout->addWidget(m_pageWidget);

    QFrame * const buttonBoxRuler = new QFrame(this);
    buttonBoxRuler->setGeometry(QRect(1, 1, 1, 3));
    buttonBoxRuler->setFrameShape(QFrame::HLine);
    buttonBoxRuler->setFrameShadow(QFrame::Sunken);
    buttonBoxRuler->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    pageLayout->addWidget(buttonBoxRuler);

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
    pageLayout->addWidget(bbox);
}

void BtConfigDialog::addPage(Page * const pageWidget) {
    m_pageWidget->addWidget(pageWidget);

    m_contentsList->addItem(pageWidget->m_listWidgetItem);
    pageWidget->m_ownsListWidgetItem = false;

    resizeListWidget(*m_contentsList);
    if (m_pageWidget->count() == 1)
        m_contentsList->setCurrentRow(0);
}

void BtConfigDialog::save() {
    for (int i = 0; i < m_pageWidget->count(); ++i) {
        BT_ASSERT(dynamic_cast<Page *>(m_pageWidget->widget(i)));
        static_cast<Page *>(m_pageWidget->widget(i))->save();
    }
    Q_EMIT signalSettingsChanged();
}
