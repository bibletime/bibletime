/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/cswordmodulesearch.h"

#include "backend/config/cbtconfig.h"
#include "backend/managers/cswordbackend.h"
#include "btglobal.h"


void CSwordModuleSearch::startSearch() {
    // Clear old search results:
    m_results.clear();
    m_foundItems = 0;

    /// \todo What is the purpose of the following statement?
    CSwordBackend::instance()->setFilterOptions(CBTConfig::getFilterOptionDefaults());

    // Search module-by-module:
    Q_FOREACH(const CSwordModuleInfo *m, m_searchModules) {
        sword::ListKey results;
        int found = m->searchIndexed(m_searchText, m_searchScope, results);
        if (found > 0) {
            m_results.insert(m, results);
            m_foundItems += found;
        }
    }

    emit finished();
}

void CSwordModuleSearch::setSearchScope(const sword::ListKey &scope) {
    /// \todo Properly examine and document the inner workings of this method.

    m_searchScope.copyFrom( scope );

    if (!strlen(scope.getRangeText())) { //we can't search with an empty search scope, would crash
        //reset the scope
        resetSearchScope();

        //disable searching with a scope!
        //  if (m_searchOptions | useScope) {
        //      qWarning("using the scope!");
        //set back the scope flag
        // }
    }
}

bool CSwordModuleSearch::modulesHaveIndices(
        const QList<const CSwordModuleInfo*> &modules)
{
    Q_FOREACH (const CSwordModuleInfo *m, modules) {
        if (!m->hasIndex()) {
            return false;
        }
    }
    return true;
}
