/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include <QDebug>

#include "backend/cswordmodulesearch.h"

#include "backend/config/btconfig.h"
#include "backend/managers/cswordbackend.h"
#include "btglobal.h"
#include "util/tool.h"


void CSwordModuleSearch::startSearch() {
    // Clear old search results:
    m_results.clear();
    m_foundItems = 0;

    /// \todo What is the purpose of the following statement?
    CSwordBackend::instance()->setFilterOptions(btConfig().getFilterOptions());

    // Search module-by-module:
    Q_FOREACH(const CSwordModuleInfo *m, m_searchModules) {
        sword::ListKey results;
        int found = m->searchIndexed(m_searchText, m_searchScope, results);
        if (found > 0) {
            m_results.insert(m, results);
            m_foundItems += found;
        }
    }
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

QList<const CSwordModuleInfo*> CSwordModuleSearch::unindexedModules(
        const QList<const CSwordModuleInfo*> &modules)
{
    QList<const CSwordModuleInfo*> unindexed;
    Q_FOREACH (const CSwordModuleInfo *m, modules) {
        if (!m->hasIndex()) {
            unindexed.append(m);
        }
    }
    return unindexed;
}

QString CSwordModuleSearch::highlightSearchedText(const QString& content, const QString& searchedText) {
    QString ret = content;

    const Qt::CaseSensitivity cs = Qt::CaseInsensitive;

    //   int index = 0;
    int index = ret.indexOf("<body", 0);
    int matchLen = 0;
    int length = searchedText.length();

    // Highlighting constants -
    // \todo We need to make the highlight color configurable.

    // Work around Qt5 QML bug
    // QTBUG-36837 "background-color" css style in QML TextEdit does not work on most tags
#ifdef BT_MOBILE
    const QString rep1("<span class=\"highlightwords\">");
    const QString rep3("class=\"highlightwords\" ");
#else
    const QString rep1("<span style=\"background-color:#FFFF66;\">");
    const QString rep3("style=\"background-color:#FFFF66;\" ");
#endif
    const QString rep2("</span>");
    const unsigned int repLength = rep1.length() + rep1.length();
    const unsigned int rep3Length = rep3.length();


    QString newSearchText;

    newSearchText = searchedText;

    // find the strongs search lemma and highlight it
    // search the searched text for "strong:" until it is not found anymore
    QStringList list;

    // split the search string - some possibilities are "\\s|\\|", "\\s|\\+", or "\\s|\\|\\+"
    // \todo find all possible seperators
    QString regExp = "\\s";
    list = searchedText.split(QRegExp(regExp));
    foreach (QString newSearchText, list) {
        int sstIndex; // strong search text index for finding "strong:"
        int idx1, idx2;
        QString sNumber, lemmaText;

        sstIndex = newSearchText.indexOf("strong:");
        if (sstIndex == -1)
            continue;

        // set the start index to the start of <body>
        int strongIndex = index;

        // Get the strongs number from the search text.
        // First, find the first space after "strong:"
        sstIndex = sstIndex + 7;
        // get the strongs number -> the text following "strong:" to the end of the string.
        sNumber = newSearchText.mid(sstIndex, -1);
        // find all the "lemma=" inside the the content
        while ((strongIndex = ret.indexOf("lemma=", strongIndex, cs)) != -1) {
            // get the strongs number after the lemma and compare it with the
            // strongs number we are looking for
            idx1 = ret.indexOf("\"", strongIndex) + 1;
            idx2 = ret.indexOf("\"", idx1 + 1);
            lemmaText = ret.mid(idx1, idx2 - idx1);

            // this is interesting because we could have a strongs number like: G3218|G300
            // To handle this we will use some extra cpu cycles and do a partial match against
            // the lemmaText
            if (lemmaText.contains(sNumber)) {
                // strongs number is found now we need to highlight it
                // I believe the easiest way is to insert rep3 just before "lemma="
                ret = ret.insert(strongIndex, rep3);
                strongIndex += rep3Length;
            }
            strongIndex += 6; // 6 is the length of "lemma="
        }
    }
    //---------------------------------------------------------------------
    // now that the strong: stuff is out of the way continue with
    // other search options
    //---------------------------------------------------------------------

    // try to figure out how to use the lucene query parser

    //using namespace lucene::queryParser;
    //using namespace lucene::search;
    //using namespace lucene::analysis;
    //using namespace lucene::util;

    //wchar_t *buf;
    //char buf8[1000];
    //standard::WhitespaceAnalyzer analyzer;
    //lucene_utf8towcs(m_wcharBuffer, searchedText.utf8(), MAX_CONV_SIZE);
    //QSharedPointer<Query> q( QueryParser::parse(m_wcharBuffer, _T("content"), &analyzer) );
    //StringReader reader(m_wcharBuffer);
    //TokenStream* tokenStream = analyzer.tokenStream( _T("field"), &reader);
    //Token token;
    //while(tokenStream->next(&token) != 0) {
    //    lucene_wcstoutf8(buf8, token.termText(), 1000);
    //    printf("%s\n", buf8);
    //}

    //===========================================================
    // since I could not figure out the lucene query parser, I
    // made a simple parser.
    //===========================================================
    QStringList words = queryParser(newSearchText);
    //qDebug() << "btsearchresultarea.cpp: " << __LINE__ << ": " <<  words << '\n';
    foreach (QString word, words) { //search for every word in the list
        QRegExp findExp;
        if (word.contains("*")) {
            length = word.length() - 1;
            word.replace('*', "\\S*"); //match within a word
            findExp = QRegExp(word);
            findExp.setMinimal(true);
        }
        else if (word.contains("?")) {
            length = word.length() - 1;
            word.replace('?', "\\S?"); //match within a word
            findExp = QRegExp(word);
            findExp.setMinimal(true);
        }
        else {
            length = word.length();
            findExp = QRegExp("\\b" + word + "\\b");
        }

        //       index = 0; //for every word start at the beginning
        index = ret.indexOf("<body", 0);
        findExp.setCaseSensitivity(cs);
        //while ( (index = ret.find(findExp, index)) != -1 ) { //while we found the word
        while ( (index = findExp.indexIn(ret, index)) != -1 ) { //while we found the word
            matchLen = findExp.matchedLength();
            if (!util::tool::inHTMLTag(index, ret)) {
                length = matchLen;
                ret = ret.insert( index + length, rep2 );
                ret = ret.insert( index, rep1 );
                index += repLength;
            }
            index += length;
        }
    }
    //qDebug() << "btsearchresultarea.cpp: " << __LINE__ << ": " <<  words << '\n';
    //qWarning("\n\n\n%s", ret.latin1());
    return ret;
}

QStringList CSwordModuleSearch::queryParser(const QString& queryString) {
    QString token("");
    QStringList tokenList;
    for (int cnt = 0; cnt < queryString.length(); cnt++) {
    loop1_body:
        // add to token
        if ((queryString[cnt]).isLetterOrNumber() || (queryString[cnt] == '*')) {
            token = token + queryString[cnt];
        }
        else if ((queryString[cnt]).isLetterOrNumber() || (queryString[cnt] == '?')) {
            token = token + queryString[cnt];
        }
        // token break
        else if (queryString[cnt] == ' ') {
            token = token.simplified();
            if ((token != "*") && (token != ""))
                tokenList.append(token);
            token = "";
        }
        // clucene appears to ignore quoted strings in the sence
        // that it treats all the words within quoted strings as
        // regular tokens and not as a single token.
        else if (queryString[cnt] == '"') {}
        // wild card - treat as a special token break
        //else if (queryString[cnt] == '*') {
        //    token = token + queryString[cnt];
        //    token = token.simplified();
        //    if ((token != "*") && (token != ""))
        //        tokenList.append(token);
        //    // start next token with wildcard (kin*m -> kin* *m)
        //    token = "*";
        //}
        // the ! token is also a token break
        else if (queryString[cnt] == '!') {
            // store away current token
            token = token.simplified();
            if ((token != "*") && (token != ""))
                tokenList.append(token);
            // add the ! token
            tokenList.append("!");
            token = "";
        }
        // the - token is also a token break
        else if (queryString[cnt] == '-') {
            // store away current token
            token = token.simplified();
            if ((token != "*") && (token != ""))
                tokenList.append(token);
            // add the ! token
            tokenList.append("-");
            token = "";
        }
        // the + token is also a token break
        else if (queryString[cnt] == '+') {
            // store away current token
            token = token.simplified();
            if ((token != "*") && (token != ""))
                tokenList.append(token);
            // add the + token
            tokenList.append("+");
            token = "";
        }
        // the || token is also a token break
        else {
            if ((queryString[cnt] == '|') && (queryString[cnt+1] == '|')) {
                // store away current token
                token = token.simplified();
                if ((token != "*") && (token != ""))
                    tokenList.append(token);
                // add the || token
                tokenList.append("||");
            }
            // the && token is also a token break
            else if ((queryString[cnt] == '&') && (queryString[cnt+1] == '&')) {
                // store away current token
                token = token.simplified();
                if ((token != "*") && (token != ""))
                    tokenList.append(token);
                // add the || token
                tokenList.append("&&");
            } else {
                continue;
            }
            token = "";
            cnt += 2;
            goto loop1_body;
        }
    }
    token = token.simplified();
    if ((token != "*") && (token != ""))
        tokenList.append(token);

    typedef QStringList::iterator TLI;
    for (TLI it = tokenList.begin(); it != tokenList.end(); ++it) {
        int pos;
        //-----------------------------------------------------------
        // remove all the NOT(!) tokens - these do not need to be
        // highlighted in the highlighter
        //-----------------------------------------------------------
        if (((*it) == "!") || ((*it) == "NOT") || ((*it) == "-")) {
            it = tokenList.erase(it);
            if (it == tokenList.end())
                break;
            it = tokenList.erase(it);
            if (it == tokenList.end())
                break;
            --it;
        }
        //-----------------------------------------------------------
        // remove all the operator tokens - these do not need to be
        // highlighted in the highlighter
        //-----------------------------------------------------------
        else if ( ((*it) == "||")  || ((*it) == "OR") || ((*it) == "+") ||
                  ((*it) == "AND") || ((*it) == "&&") ) {
            it = tokenList.erase(it);
            if (it == tokenList.end())
                break;
            --it;
        }
        // if the token contains a ^ then trim the remainder of the
        // token from the ^
        //What??? error: invalid conversion from ‘const void*’ to ‘int’
        // and how come "contains" returns bool but is used as int?
        //else if ( (pos = (*it).contains("^")) >= 0 ) {
        else if ( (pos = (*it).indexOf("^") ) >= 0 ) {
            (*it) = (*it).left(pos - 1);
        }
        // if the token contains a ~ then trim the remainder of the
        // token from the ~
        else if ( (pos = (*it).indexOf("~") ) >= 0 ) {
            (*it) = (*it).left(pos - 2) + "*";
        }
    }
    return(tokenList);
}
