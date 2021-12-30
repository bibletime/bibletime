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

#include "cswordmodulesearch.h"

#include "../util/tool.h"
#include "btglobal.h"
#include "config/btconfig.h"
#include "managers/cswordbackend.h"


void CSwordModuleSearch::startSearch() {
    // Clear old search results:
    m_results.clear();
    m_foundItems = 0u;

    /// \todo What is the purpose of the following statement?
    CSwordBackend::instance()->setFilterOptions(btConfig().getFilterOptions());

    // Search module-by-module:
    Q_FOREACH(CSwordModuleInfo const * const m, m_searchModules) {
        sword::ListKey results;
        size_t const found =
                m->searchIndexed(m_searchText, m_searchScope, results);
        if (found > 0u) {
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

const BtConstModuleList CSwordModuleSearch::unindexedModules(
        const BtConstModuleList &modules)
{
    BtConstModuleList unindexed;
    Q_FOREACH(CSwordModuleInfo const * const m, modules)
        if (!m->hasIndex())
            unindexed.append(m);
    return unindexed;
}

QString CSwordModuleSearch::highlightSearchedText(const QString& content, const QString& searchedText) {
    QString ret = content;

    static Qt::CaseSensitivity const cs = Qt::CaseInsensitive;

    //   int index = 0;
    int index = ret.indexOf("<body", 0);

    // Work around Qt5 QML bug
    // QTBUG-36837 "background-color" css style in QML TextEdit does not work on most tags
    const QString rep1("<span class=\"highlightwords\">");
    const QString rep3("class=\"highlightwords\" ");
    const QString rep2("</span>");
    const unsigned int repLength = rep1.length() + rep1.length();
    const unsigned int rep3Length = rep3.length();

    // find the strongs search lemma and highlight it
    // search the searched text for "strong:" until it is not found anymore
    // split the search string - some possibilities are "\\s|\\|", "\\s|\\+", or "\\s|\\|\\+"
    // \todo find all possible seperators
    Q_FOREACH (QString const & newSearchText,
               searchedText.split(QRegExp("\\s")))
    {
        // strong search text index for finding "strong:"
        int sstIndex = newSearchText.indexOf("strong:");
        if (sstIndex == -1)
            continue;

        // Get the strongs number from the search text.
        // First, find the first space after "strong:"
        sstIndex = sstIndex + 7;

        // set the start index to the start of <body>
        int strongIndex = index;

        // get the strongs number -> the text following "strong:" to the end of the string.
        // find all the "lemma=" inside the the content
        while ((strongIndex = ret.indexOf("lemma=", strongIndex, cs)) != -1) {
            // get the strongs number after the lemma and compare it with the
            // strongs number we are looking for
            int const idx1 = ret.indexOf("\"", strongIndex) + 1;
            int const idx2 = ret.indexOf("\"", idx1 + 1);

            // this is interesting because we could have a strongs number like: G3218|G300
            // To handle this we will use some extra cpu cycles and do a partial match against
            // the lemmaText
            if (ret.mid(idx1, idx2 - idx1).contains(newSearchText.mid(sstIndex,
                                                                      -1)))
            {
                // strongs number is found now we need to highlight it
                // I believe the easiest way is to insert rep3 just before "lemma="
                ret = ret.insert(strongIndex, rep3); /// \bug ?
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
    int length = searchedText.length();
    int matchLen = 0;
    for (QStringList words(queryParser(searchedText));
         !words.empty();
         words.pop_front())
    {
        QString & word = words.first();
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
    return ret;
}

QStringList CSwordModuleSearch::queryParser(const QString& queryString) {
    QString token("");
    QStringList tokenList;
    for (int cnt = 0; cnt < queryString.length(); cnt++) {
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
            if ((queryString[cnt] == '|')
                && (cnt + 1 < queryString.length())
                && (queryString[cnt + 1] == '|'))
            {
                // store away current token
                token = token.simplified();
                if ((token != "*") && (token != ""))
                    tokenList.append(token);
                // add the || token
                tokenList.append("||");
            }
            // the && token is also a token break
            else if ((queryString[cnt] == '&')
                     && (cnt + 1 < queryString.length())
                     && (queryString[cnt + 1] == '&'))
            {
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
            ++cnt;
        }
    }
    token = token.simplified();
    if ((token != "*") && (token != ""))
        tokenList.append(token);

    using TLI = QStringList::iterator;
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
        //What??? error: invalid conversion from 'const void*' to 'int'
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

QString CSwordModuleSearch::prepareSearchText(
        QString const & orig,
        SearchType const searchType)
{
    if (searchType == FullType)
        return orig;
    static const QRegExp syntaxCharacters("[+\\-()!\"~]");
    static const QRegExp andWords("\\band\\b", Qt::CaseInsensitive);
    static const QRegExp orWords("\\bor\\b", Qt::CaseInsensitive);
    QString text(orig.simplified());
    text.remove(syntaxCharacters);
    text.replace(andWords, "\"and\"");
    text.replace(orWords, "\"or\"");
    if (searchType == AndType)
        text.replace(" ", " AND ");
    return text;
}

QDataStream &operator<<(QDataStream &out, const CSwordModuleSearch::SearchType &searchType) {
    out << static_cast<qint8>(searchType);
    return out;
}

QDataStream &operator>>(QDataStream &in, CSwordModuleSearch::SearchType &searchType) {
    qint8 i;
    in >> i;
    searchType = static_cast<CSwordModuleSearch::SearchType>(i);
    return in;
}
