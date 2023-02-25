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

#include "bttextwindowheader.h"

#include <QHBoxLayout>
#include <QLayout>
#include <QSizePolicy>
#include <Qt>
#include <utility>
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../../util/btmodules.h"
#include "bttextwindowheaderwidget.h"


BtTextWindowHeader::BtTextWindowHeader(CSwordModuleInfo::ModuleType modtype,
                                       BtModuleList modules,
                                       QWidget * const parent)
    : QWidget(parent)
    , m_moduleType(modtype)
{
    BT_ASSERT(modtype != CSwordModuleInfo::GenericBook);

    auto * const layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setLayoutDirection(Qt::LeftToRight);

    for (int i = 0; i < modules.count(); i++)
        addWidget();
    updateWidgets(modules);
}

void BtTextWindowHeader::setModules(BtModuleList modules) {
    adjustWidgetCount(modules.size());
    updateWidgets(modules);
}

void BtTextWindowHeader::adjustWidgetCount(int const numWidgets) {
    if (m_widgetList.size() > numWidgets) {
        auto toRemove = m_widgetList.size() - numWidgets;
        do {
            // it should be safe to delete the button later
            auto * const w = m_widgetList.takeFirst();
            w->setParent(nullptr);
            w->deleteLater();
        } while (--toRemove);
    } else if (numWidgets > m_widgetList.size()) {
        auto toAdd = numWidgets - m_widgetList.size();
        do {
            addWidget();
        } while (--toAdd);
    }
}

BtTextWindowHeaderWidget * BtTextWindowHeader::addWidget() {
    auto * const w = new BtTextWindowHeaderWidget(m_moduleType, this);
    layout()->addWidget(w);
    m_widgetList.append(w);
    BT_CONNECT(w, &BtTextWindowHeaderWidget::sigModuleAdd,
               this, &BtTextWindowHeader::moduleAdded);
    BT_CONNECT(w, &BtTextWindowHeaderWidget::sigModuleReplace,
               this, &BtTextWindowHeader::moduleReplaced);
    BT_CONNECT(w, &BtTextWindowHeaderWidget::sigModuleRemove,
               this, &BtTextWindowHeader::moduleRemoved);
    return w;
}

void BtTextWindowHeader::updateWidgets(BtModuleList const & modules) {
    int const leftLikeModules = leftLikeParallelModules(modules);
    for (int i = 0; i < m_widgetList.count(); i++)
        m_widgetList.at(i)->updateWidget(modules,
                                         modules.at(i),
                                         i,
                                         leftLikeModules);
}
