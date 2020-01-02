/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef CSWORDMODULESEARCH_H
#define CSWORDMODULESEARCH_H

#include <QObject>

#include <QHash>
#include <QMetaType>
#include "../util/btassert.h"
#include "drivers/btmodulelist.h"

// Sword includes:
#include <listkey.h>


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
        using Results = QHash<const CSwordModuleInfo*, sword::ListKey>;

        enum SearchType { /* Values provided for serialization */
            AndType = 0,
            OrType = 1,
            FullType = 2
        };

    public: /* Methods: */
        inline CSwordModuleSearch()
            : m_foundItems(0u) {}

        /**
          Sets the text which should be search in the modules.
          \param[in] text the text to search.
        */
        inline void setSearchedText(const QString &text) {
            m_searchText = text;
        }

        /**
          Set the modules which should be searched.
          \param[in] modules the modules to search in.
        */
        inline void setModules(const BtConstModuleList &modules) {
            if (modules.empty())
                return;
            BT_ASSERT(unindexedModules(modules).empty());
            m_searchModules = modules;
        }

        /**
          Sets the search scope.
          \param[in] scope the scope used for the search.
        */
        void setSearchScope(const sword::ListKey &scope);

        /**
          Resets the search scope.
        */
        inline void resetSearchScope() {
            m_searchScope.clear();
        }

        /**
          \returns the search scope.
        */
        const sword::ListKey &searchScope() const {
            return m_searchScope;
        }

        /**
          Starts the search for the search text.
          \throws on error
        */
        void startSearch();

        /**
          \returns the number of found items in the last search.
        */
        inline size_t foundItems() const {
            return m_foundItems;
        }

        /**
          \returns the results of the search.
        */
        const Results &results() const {
            return m_results;
        }

        /**
          \returns the list of unindexed modules in the given list.
        */
        static const BtConstModuleList unindexedModules(const BtConstModuleList & modules);

        /**
        * This function highlights the searched text in the content using the search type given by search flags
        */
        static QString highlightSearchedText(const QString& content, const QString& searchedText);

        /**
          Prepares the search string given by user for a specific search type
        */
        static QString prepareSearchText(QString const & orig,
                                         SearchType const searchType);

    protected:
        /**
        * This function breakes the queryString into clucene tokens
        */
        static QStringList queryParser(const QString& queryString);

    private: /* Fields: */
        QString                        m_searchText;
        sword::ListKey                 m_searchScope;
        BtConstModuleList              m_searchModules;

        Results                        m_results;
        size_t                         m_foundItems;
};

QDataStream &operator<<(QDataStream &out, const CSwordModuleSearch::SearchType &searchType);
QDataStream &operator>>(QDataStream &in, CSwordModuleSearch::SearchType &searchType);
Q_DECLARE_METATYPE(CSwordModuleSearch::SearchType)

#endif
