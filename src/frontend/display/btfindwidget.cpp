/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/display/btfindwidget.h"
#include "QHBoxLayout"
#include "QLineEdit"
#include "QToolButton"
#include "util/directory.h"
#include "util/cresmgr.h"
#include "QLabel"
#include "QSpacerItem"

BtFindWidget::BtFindWidget(QWidget* parent)
        : QWidget(parent) {
    namespace DU = util::directory;
    QHBoxLayout *hboxLayout = new QHBoxLayout(this);
    hboxLayout->setMargin(0);
    hboxLayout->setSpacing(6);

    QToolButton* closeButton = createToolButton(CResMgr::findWidget::close_icon);
    hboxLayout->addWidget(closeButton);
    bool ok = connect(closeButton, SIGNAL(released()), this, SLOT(closeWidget()));
    Q_ASSERT(ok);

    QLineEdit* editFind = new QLineEdit(this);
    hboxLayout->addWidget(editFind);

    QToolButton* previousButton = createToolButton(CResMgr::findWidget::previous_icon);
    hboxLayout->addWidget(previousButton);

    QLabel* previousText = new QLabel(tr("Previous"), this);
    hboxLayout->addWidget(previousText);

    QSpacerItem* spacerItem = new QSpacerItem(16, 16, QSizePolicy::Minimum, QSizePolicy::Minimum);
    hboxLayout->addItem(spacerItem);

    QToolButton* nextButton = createToolButton(CResMgr::findWidget::next_icon);
    hboxLayout->addWidget(nextButton);

    QLabel* nextText = new QLabel(tr("Next"), this);
    hboxLayout->addWidget(nextText);
}


BtFindWidget::~BtFindWidget() {
}

QToolButton* BtFindWidget::createToolButton(const QString& iconName)
{
    namespace DU = util::directory;
    QToolButton* button = new QToolButton(this);
    button->setIcon(DU::getIcon(iconName));
    button->setIconSize(QSize(16,16));
    return button;
}

void BtFindWidget::closeWidget() {
    setVisible(false);
}
