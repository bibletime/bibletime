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
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStringList>
#include <QtCore>
#include "../util/btassert.h"
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

static auto const spaceRegexpString(QStringLiteral(R"PCRE(\s+)PCRE"));
static QRegularExpression const spaceRegexp(spaceRegexpString);

} // anonymous namespace

QString highlightSearchedText(QString const & content,
                              QString const & searchedText,
                              bool plainSearchedText)
{
    static Qt::CaseSensitivity const cs = Qt::CaseInsensitive;

    static auto const skipIndexToTagEnd =
        [](auto const & str, auto i) {
            static QRegularExpression const re(
                    QStringLiteral(R"PCRE(["'>])PCRE"));
            for (;;) {
                i = str.indexOf(re, i);
                if (i < 0)
                    return i;

                auto const match = str.at(i);
                if (match == QLatin1Char('>'))
                    return i + 1;

                // Skip to end of quoted attribute value:
                i = str.indexOf(match, ++i);
                if (i < 0)
                    return i;
                ++i;
            }
        };

    auto const bodyIndex =
        [&content]{
            static QRegularExpression const tagRe(
                    QStringLiteral(R"PCRE(<body(>|\\s))PCRE"));
            auto const i = content.indexOf(tagRe);
            return (i < 0) ? 0 : skipIndexToTagEnd(content, i + 5);
        }();
    if (bodyIndex < 0)
        return content;

    auto ret = content.mid(bodyIndex);

    if (!plainSearchedText) {
        // find the strongs search lemma and highlight it
        for (auto const & newSearchText
             : searchedText.split(spaceRegexp, Qt::SkipEmptyParts))
        {
            // strong search text index for finding "strong:"
            int sstIndex = newSearchText.indexOf(QStringLiteral("strong:"));
            if (sstIndex == -1)
                continue;

            // Get the strongs number from the search text.
            // First, find the first space after "strong:"
            sstIndex = sstIndex + 7;

            // set the start index to the start of <body>
            int strongIndex = 0;

            // get the strongs number -> the text following "strong:" to the end
            // of the string. find all the "lemma=" inside the the content
            while ((strongIndex =
                    ret.indexOf(QStringLiteral("lemma="), strongIndex, cs))
                   != -1)
            {
                // get the strongs number after the lemma and compare it with
                // the strongs number we are looking for
                int const idx1 = ret.indexOf('"', strongIndex) + 1;
                int const idx2 = ret.indexOf('"', idx1 + 1);

                // this is interesting because we could have a strongs number
                // like G3218|G300. To handle this we will use some extra cpu
                // cycles and do a partial match against the lemmaText
                if (ret.mid(idx1, idx2 - idx1)
                       .contains(newSearchText.mid(sstIndex, -1)))
                {
                    static auto const rep3 =
                        QStringLiteral(R"HTML(class="highlightwords" )HTML");
                    // strongs number is found now we need to highlight it
                    // I believe the easiest way is to insert rep3 just before
                    // "lemma="
                    ret = ret.insert(strongIndex, rep3); /// \bug ?
                    strongIndex += rep3.length();
                }
                strongIndex += 6; // 6 is the length of "lemma="
            }
        }
        //---------------------------------------------------------------------
        // now that the strong: stuff is out of the way continue with
        // other search options
        //---------------------------------------------------------------------
    }

    QRegularExpression highlightRegex; // Construct highLightRegex:
    if (plainSearchedText) {
        auto words = searchedText.split(spaceRegexp, Qt::SkipEmptyParts);
        for (auto & word : words)
            word = QRegularExpression::escape(word);
        highlightRegex = QRegularExpression(words.join(spaceRegexpString));
    } else {
        QString wordsRegexString;
        for (auto const & word : queryParser(searchedText)) {
            QString wordRegexString;
            auto const wordSize = word.size();
            wordRegexString.reserve(wordSize + 3);

            static QRegularExpression const wildCardRegex(
                QStringLiteral(R"PCRE([*?])PCRE"));
            auto fragmentEnd = word.indexOf(wildCardRegex);
            decltype(fragmentEnd) fragmentStart = 0;
            while (fragmentEnd >= 0) {
                if (auto const fragmentSize = fragmentEnd - fragmentStart)
                    wordRegexString.append(
                        QRegularExpression::escape(
                            word.mid(fragmentStart, fragmentSize)));
                wordRegexString.append(word.at(fragmentEnd) == QLatin1Char('*')
                                       ? QStringLiteral(R"PCRE(\S*?)PCRE")
                                       : QStringLiteral(R"PCRE(\S)PCRE"));
                fragmentStart = fragmentEnd + 1;
                fragmentEnd = word.indexOf(wildCardRegex, fragmentStart);
            }
            wordRegexString.append(
                QRegularExpression::escape(word.mid(fragmentStart)));

            if (!wordsRegexString.isEmpty())
                wordsRegexString.append(QLatin1Char('|'));
            wordsRegexString.append(wordRegexString);
        }
        highlightRegex =
            QRegularExpression(
                QStringLiteral(R"PCRE(\b(%1)\b)PCRE").arg(wordsRegexString),
                QRegularExpression::CaseInsensitiveOption);
    }

    QStringList r(content.left(bodyIndex));

    // Iterate over HTML text fragments:
    auto fragmentStart = 0;
    auto fragmentEnd = ret.indexOf(QLatin1Char('<'), fragmentStart);
    decltype(ret.size()) fragmentSize =
        (fragmentEnd < 0 ? ret.size() : fragmentEnd) - fragmentStart;
    for (QRegularExpressionMatch match;;) {
        if (fragmentSize > 0) {
            #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
            auto const fragment = ret.mid(fragmentStart, fragmentSize);
            #else
            QStringView const fragment(ret.constData() + fragmentStart,
                                       fragmentSize);
            #endif
            decltype(fragmentStart) searchStart = 0;
            for (;;) {
                auto i = fragment.indexOf(highlightRegex, searchStart, &match);
                if (i < 0) {
                    #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
                    r << fragment.mid(searchStart);
                    #else
                    r << fragment.mid(searchStart).toString();
                    #endif
                    break;
                }

                if (auto const noMatchSize = i - searchStart) {
                    #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
                    r << fragment.mid(searchStart, noMatchSize);
                    #else
                    r << fragment.mid(searchStart, noMatchSize).toString();
                    #endif
                }
                r << QStringLiteral(R"HTML(<span class="highlightwords">)HTML")
                  << match.captured()
                  << QStringLiteral(R"HTML(</span>)HTML");
                searchStart = i + match.capturedLength();
            }
        }

        if (fragmentEnd < 0)
            break;
        fragmentStart = skipIndexToTagEnd(ret, fragmentEnd + 1);
        r << ret.mid(fragmentEnd, fragmentStart - fragmentEnd);
        fragmentEnd = ret.indexOf(QLatin1Char('<'), fragmentStart);
        fragmentSize =
            (fragmentEnd < -1) ? ret.size() : (fragmentEnd - fragmentStart);
    }

    return r.join(QString());
}

QString prepareSearchText(QString const & orig, SearchType const searchType) {
    if (searchType == FullType)
        return orig;
    auto words = orig.split(spaceRegexp, Qt::SkipEmptyParts);
    static QRegularExpression const escapeRe(
        QLatin1String(R"PCRE(([\\+\-\!\(\)\:\^\]\[\"\{\}\~\*\?\|\&]))PCRE"));
    for (auto & word : words)
        word = QStringLiteral(R"("%1")").arg(word.replace(escapeRe, "\\\\1"));
    return words.join(searchType == OrType
                      ? QStringLiteral(" OR ")
                      : QStringLiteral(" AND "));
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
