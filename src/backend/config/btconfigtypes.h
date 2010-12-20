#ifndef BTCONFIGTYPES_H
#define BTCONFIGTYPES_H

#include <QMetaType>
#include "backend/btmoduletreeitem.h" // for BTModuleTreeItem::Grouping
#include "frontend/searchdialog/btsearchoptionsarea.h" // for Search::BtSearchOptionsArea::SearchType

namespace btconfigtypes
{
enum alignmentMode
{
    autoTileVertical,
    autoTileHorizontal,
    autoTile,
    autoTabbed,
    autoCascade
};
Q_DECLARE_METATYPE(alignmentMode);

Q_DECLARE_METATYPE(BTModuleTreeItem::Grouping);
Q_DECLARE_METATYPE(Search::BtSearchOptionsArea::SearchType);

}

#endif // BTCONFIGTYPES_H
