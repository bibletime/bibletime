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

#pragma once

#include <memory>
#include <QMetaType>
#include <QString>
#include <vector>
#include "drivers/btmodulelist.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsuggest-destructor-override"
#endif
#include <listkey.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#pragma GCC diagnostic pop


class CSwordModuleInfo;
class QDataStream;
namespace sword { class SWKey; }

namespace CSwordModuleSearch {

using ModuleResultList = std::vector<std::shared_ptr<sword::SWKey const>>;

struct ModuleSearchResult {
    CSwordModuleInfo const * module;
    ModuleResultList results;
};

using Results = std::vector<ModuleSearchResult>;

enum SearchType { /* Values provided for serialization */
    AndType = 0,
    OrType = 1,
    FullType = 2
};

Results search(QString const & searchText,
               BtConstModuleList const & modules,
               sword::ListKey scope);

/**
* This function highlights the searched text in the content using the search type given by search flags
*/
QString highlightSearchedText(QString const & content,
                              QString const & searchedText,
                              bool plainSearchedText = false);

/**
  Prepares the search string given by user for a specific search type
*/
QString prepareSearchText(QString const & orig, SearchType const searchType);

} // namespace CSwordModuleSearch

QDataStream &operator<<(QDataStream &out, const CSwordModuleSearch::SearchType &searchType);
QDataStream &operator>>(QDataStream &in, CSwordModuleSearch::SearchType &searchType);
Q_DECLARE_METATYPE(CSwordModuleSearch::SearchType)
