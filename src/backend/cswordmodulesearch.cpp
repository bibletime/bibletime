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

#include "cswordmodulesearch.h"

#include <algorithm>
#include <QChar>
#include <QDataStream>
#include <QList>
#include <QRegExp>
#include <QRegularExpression>
#include <QStringList>
#include <QtCore>
#include "../util/btassert.h"
#include "../util/tool.h"
#include "config/btconfig.h"
#include "drivers/cswordmoduleinfo.h"
#include "managers/cswordbackend.h"

// Sword includes:
#include <listkey.h>


namespace CSwordModuleSearch {

Results search(QString const & searchText,
               BtConstModuleList const & modules,
               sword::ListKey scope)
{
    BT_ASSERT(std::all_of(modules.begin(),
                          modules.end(),
                          [](auto const * const m) { return m->hasIndex(); }));

    /// \todo What is the purpose of the following statement?
    CSwordBackend::instance().setFilterOptions(btConfig().getFilterOptions());

    // Search module-by-module:
    Results r;
    r.reserve(modules.size());
    for (auto const * const m : modules)
        r.emplace_back(
                    ModuleSearchResult{m, m->searchIndexed(searchText, scope)});
    return r;
}

namespace {

/** This function does a terrible job of trying to parse a CLucene query string
    into a list of words (potentially containing the * or ? glob characters) for
    word highlighting purposes. */
QStringList queryParser(QString const & queryString) {
    QStringList tokenList;
    {
        QString token;
        bool tokenHasLetterOrNumber = false;
        bool tokenHasStar = false;
        auto const pushToken =
                [&] {
                    if (tokenHasLetterOrNumber
                        // Ignore empty tokens and those equivalent to glob (*):
                        || (!tokenHasStar && !token.isEmpty()))
                        tokenList.append(token);
                    token.clear();
                    tokenHasLetterOrNumber = false;
                    tokenHasStar = false;
                };
        for (int cnt = 0; cnt < queryString.size(); cnt++) {
            auto const c = queryString[cnt];
            if (c.isLetterOrNumber()) {
                token.append(c);
                tokenHasLetterOrNumber = true;
            } else if (c == '*') {
                token.append(c);
                tokenHasStar = true;
            } else  if (c == '?') {
                token.append(c);
            } else if (c == '!' || c == '-' || c == '+') {
                pushToken();
                tokenList.append(c);
            } else if ((c == '|' || c == '&')
                       && cnt + 1 < queryString.size()
                       && queryString[cnt + 1] == c)
            {
                pushToken();
                tokenList.append(QString(2, c));
                ++cnt;
            } else { // Spaces and other unrecognized stuff act as separators:
                pushToken();
            }
        }
        pushToken();
    }

    for (auto it = tokenList.begin(); it != tokenList.end(); ++it) {
        int pos;
        //-----------------------------------------------------------
        // remove all the NOT(!) tokens - these do not need to be
        // highlighted in the highlighter
        //-----------------------------------------------------------
        if (((*it) == '!')
            || ((*it) == QStringLiteral("NOT"))
            || ((*it) == '-'))
        {
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
        else if (((*it) == QStringLiteral("||"))
                 || ((*it) == QStringLiteral("OR"))
                 || ((*it) == '+')
                 || ((*it) == QStringLiteral("AND"))
                 || ((*it) == QStringLiteral("&&")))
        {
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
        else if ( (pos = (*it).indexOf('^') ) >= 0 ) {
            (*it) = (*it).left(pos - 1);
        }
        // if the token contains a ~ then trim the remainder of the
        // token from the ~
        else if ( (pos = (*it).indexOf('~') ) >= 0 ) {
            (*it) = (*it).left(pos - 2) + '*';
        }
    }
    return(tokenList);
}

} // anonymous namespace

QString highlightSearchedText(QString const & content,
                              QString const & searchedText)
{
    QString ret = content;

    static Qt::CaseSensitivity const cs = Qt::CaseInsensitive;

    //   int index = 0;
    int index = ret.indexOf(QStringLiteral("<body"));

    // Work around Qt5 QML bug
    // QTBUG-36837 "background-color" css style in QML TextEdit does not work on most tags
    auto const rep1 = QStringLiteral("<span class=\"highlightwords\">");
    auto const rep3 = QStringLiteral("class=\"highlightwords\" ");
    auto const rep2 = QStringLiteral("</span>");
    const unsigned int repLength = rep1.length() + rep1.length();
    const unsigned int rep3Length = rep3.length();

    // find the strongs search lemma and highlight it
    // search the searched text for "strong:" until it is not found anymore
    // split the search string - some possibilities are "\\s|\\|", "\\s|\\+", or "\\s|\\|\\+"
    // \todo find all possible seperators
    static QRegularExpression const spaceRegexp(QStringLiteral("\\s"));
    for (auto const & newSearchText : searchedText.split(spaceRegexp)) {
        // strong search text index for finding "strong:"
        int sstIndex = newSearchText.indexOf(QStringLiteral("strong:"));
        if (sstIndex == -1)
            continue;

        // Get the strongs number from the search text.
        // First, find the first space after "strong:"
        sstIndex = sstIndex + 7;

        // set the start index to the start of <body>
        int strongIndex = index;

        // get the strongs number -> the text following "strong:" to the end of the string.
        // find all the "lemma=" inside the the content
        while ((strongIndex =
                    ret.indexOf(QStringLiteral("lemma="), strongIndex, cs))
               != -1)
        {
            // get the strongs number after the lemma and compare it with the
            // strongs number we are looking for
            int const idx1 = ret.indexOf('"', strongIndex) + 1;
            int const idx2 = ret.indexOf('"', idx1 + 1);

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
    int matchLen = 0;
    for (QStringList words(queryParser(searchedText));
         !words.empty();
         words.pop_front())
    {
        QString & word = words.first();
        auto length = word.length();
        if (word.contains('*')) {
            --length;
            word.replace('*', QStringLiteral("\\S*")); //match within a word
        }
        else if (word.contains('?')) {
            --length;
            word.replace('?', QStringLiteral("\\S")); //match within a word
        }
        QRegExp findExp(QStringLiteral("\\b%1\\b").arg(word));
        findExp.setMinimal(true);

        //       index = 0; //for every word start at the beginning
        index = ret.indexOf(QStringLiteral("<body"));
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

QString prepareSearchText(QString const & orig, SearchType const searchType) {
    if (searchType == FullType)
        return orig;
    static const QRegExp syntaxCharacters(QStringLiteral("[+\\-()!\"~]"));
    static const QRegExp andWords(QStringLiteral("\\band\\b"),
                                  Qt::CaseInsensitive);
    static const QRegExp orWords(QStringLiteral("\\bor\\b"),
                                 Qt::CaseInsensitive);
    QString text(orig.simplified());
    text.remove(syntaxCharacters);
    text.replace(andWords, QStringLiteral("\"and\""));
    text.replace(orWords, QStringLiteral("\"or\""));
    if (searchType == AndType)
        text.replace(' ', QStringLiteral(" AND "));
    return text;
}

} // namespace CSwordModuleSearch

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
