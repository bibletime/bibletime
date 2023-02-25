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
#include <QStringList>
#include <Qt>
#include <QWidget>
#include <utility>
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../util/btconnect.h"
#include "../../util/btmodules.h"
#include "bttextwindowheaderwidget.h"
#include "cdisplaywindow.h"


BtTextWindowHeader::BtTextWindowHeader(CSwordModuleInfo::ModuleType modtype,
                                       BtModuleList modules,
                                       CDisplayWindow * const window)
    : QWidget(window)
    , m_modules(std::move(modules))
    , m_moduleType(modtype)
{
    auto * const layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setLayoutDirection(Qt::LeftToRight);

    for (int i = 0; i < m_modules.count(); i++)
        addWidget();
    updateWidgets();
}

void BtTextWindowHeader::setModules(BtModuleList newModules) {
    m_modules = std::move(newModules);
    adjustWidgetCount();
    updateWidgets();
}

void BtTextWindowHeader::adjustWidgetCount() {
    int widgetCountDifference = m_widgetList.count() - (m_modules.count());
    if (m_moduleType == CSwordModuleInfo::GenericBook)
        widgetCountDifference = (1 - m_widgetList.count()) * -1;

    // if there are more buttons than modules, delete buttons:
    for (; widgetCountDifference > 0; --widgetCountDifference) {
        // it should be safe to delete the button later
        auto * const w = m_widgetList.takeFirst();
        w->setParent(nullptr);
        w->deleteLater();
    }

    // if there are more modules than buttons, add buttons:
    for (; widgetCountDifference < 0; ++widgetCountDifference)
        addWidget();
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

void BtTextWindowHeader::updateWidgets() {
    int const leftLikeModules = leftLikeParallelModules(m_modules);
    for (int i = 0; i < m_widgetList.count(); i++)
        m_widgetList.at(i)->updateWidget(m_modules,
                                         m_modules.at(i),
                                         i,
                                         leftLikeModules);
}
