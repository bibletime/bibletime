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
typedef QMap<QString, QString> StringMap;
}
Q_DECLARE_METATYPE(btconfigtypes::alignmentMode);
Q_DECLARE_METATYPE(BTModuleTreeItem::Grouping);
Q_DECLARE_METATYPE(Search::BtSearchOptionsArea::SearchType);
Q_DECLARE_METATYPE(btconfigtypes::StringMap);

#endif // BTCONFIGTYPES_H
