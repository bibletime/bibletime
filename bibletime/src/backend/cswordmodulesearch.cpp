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
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/cswordbackend.h"
#include "btglobal.h"

// Sword includes:
#include <listkey.h>
#include <swkey.h>
#include <swmodule.h>


void CSwordModuleSearch::setModules(const QList<const CSwordModuleInfo*> &modules) {
    m_moduleList = modules;
}

void CSwordModuleSearch::startSearch() {
    m_foundItems = 0;

    CSwordBackend::instance()->setFilterOptions(CBTConfig::getFilterOptionDefaults());

    m_results.clear();
    Q_FOREACH(const CSwordModuleInfo *m, m_moduleList) {
        sword::ListKey results;
        int found = m->searchIndexed(m_searchedText, m_searchScope, results);
        if (found > 0) {
            m_results.insert(m, results);
            m_foundItems += found;
        }
    }

    emit finished();
}

/** Sets the text which should be search in the modules. */
void CSwordModuleSearch::setSearchedText( const QString& text ) {
    m_searchedText = text;
}

/** Sets the search scope. */
void CSwordModuleSearch::setSearchScope( const sword::ListKey& scope ) {
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

/** Sets the search scope back. */
void CSwordModuleSearch::resetSearchScope() {
    m_searchScope.ClearList();
}

/** Returns a copy of the used search scope. */
const sword::ListKey& CSwordModuleSearch::searchScope() const {
    return m_searchScope;
}

void CSwordModuleSearch::connectFinished( QObject *receiver, const char *member ) {
    //m_finishedSig.connect(receiver, member);
    QObject::connect(this, SIGNAL(finished()), receiver, member);
}

bool CSwordModuleSearch::modulesHaveIndices(
        const QList<const CSwordModuleInfo*> &modules)
{
    QList<const CSwordModuleInfo*>::const_iterator end_it = modules.end();
    for (QList<const CSwordModuleInfo*>::const_iterator it = modules.begin();
         it != end_it; ++it)
    {
        if (!(*it)->hasIndex()) {
            return false;
        }
    }
    return true;
}
