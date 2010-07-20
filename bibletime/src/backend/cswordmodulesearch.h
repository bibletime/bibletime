/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CSWORDMODULESEARCH_H
#define CSWORDMODULESEARCH_H

#include <QObject>

#include <QHash>
#include <QString>

// Sword includes:
#include <listkey.h>


class CSwordModuleInfo;

/**
 * CSwordModuleSearch manages the search on Sword modules. It manages the thread(s)
 * and manages the different modules.
  *
  * @author The BibleTime team
  * @version $Id: cswordmodulesearch.h,v 1.34 2006/08/08 19:32:48 joachim Exp $
  */

class CSwordModuleSearch: public QObject {
        Q_OBJECT

    public: /* Types: */
        typedef QHash<const CSwordModuleInfo*, sword::ListKey> Results;

    public:
        inline CSwordModuleSearch()
            : m_searchOptions(0), m_foundItems(0) {}

        /**
        * Sets the text which should be search in the modules.
        */
        void setSearchedText( const QString& );

        /**
          Starts the search for the search text.
        */
        void startSearch();

        /**
        * This function sets the modules which should be searched.
        */
        void setModules(const QList<const CSwordModuleInfo*> &modules);
        /**
        * Sets the search scope.
        */
        void setSearchScope( const sword::ListKey& scope );
        /**
        * Sets the seaech scope back.
        */
        void resetSearchScope();

        /**
          \returns the number of found items in the last search.
        */
        inline unsigned foundItems() const {
            return m_foundItems;
        }

        /**
          \returns the results of the search.
        */
        const Results &results() const {
            return m_results;
        }

        /**
        * Returns a copy of the used search scope.
        */
        const sword::ListKey& searchScope() const;

        void connectFinished( QObject * receiver, const char * member );

        /**
          \returns whether all of the specified modules have indices already built.
        */
        bool modulesHaveIndices(const QList<const CSwordModuleInfo*> &modules);

    protected:
        QString m_searchedText;
        sword::ListKey m_searchScope;
        QList<const CSwordModuleInfo*> m_moduleList;

        int m_searchOptions;
        Results m_results;
        unsigned m_foundItems;

    signals:
        void finished();

    private:
        static CSwordModuleSearch* searcher;
};

#endif
