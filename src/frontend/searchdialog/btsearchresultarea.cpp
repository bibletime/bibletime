/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/searchdialog/btsearchresultarea.h"

#include <QApplication>
#include <QDebug>
#include <QFrame>
#include <QMenu>
#include <QProgressDialog>
#include <QPushButton>
#include <QSize>
#include <QSplitter>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>
#include "backend/keys/cswordversekey.h"
#include "backend/rendering/cdisplayrendering.h"
#include "frontend/display/cdisplay.h"
#include "frontend/searchdialog/cmoduleresultview.h"
#include "frontend/searchdialog/csearchdialog.h"
#include "frontend/searchdialog/csearchresultview.h"
#include "util/tool.h"


namespace Search {

BtSearchResultArea::BtSearchResultArea(QWidget *parent)
        : QWidget(parent) {
    qDebug() << "BtSearchResultArea::BtSearchResultArea";
    initView();
    initConnections();
    qDebug() << "BtSearchResultArea::BtSearchResultArea end";
}

void BtSearchResultArea::initView() {
    QVBoxLayout *mainLayout;
    QWidget *resultListsWidget;
    QVBoxLayout *resultListsWidgetLayout;

    //Size is calculated from the font rather than set in pixels,
    // maybe this is better in different kinds of displays?
    int mWidth = util::tool::mWidth(this, 1);
    this->setMinimumSize(QSize(mWidth*40, mWidth*15));
    mainLayout = new QVBoxLayout(this);
    m_mainSplitter = new QSplitter(this);
    m_mainSplitter->setOrientation(Qt::Horizontal);

    resultListsWidget = new QWidget(m_mainSplitter);

    resultListsWidgetLayout = new QVBoxLayout(resultListsWidget);
    resultListsWidgetLayout->setContentsMargins(0, 0, 0, 0);

    //Splitter for two result lists
    m_resultListSplitter = new QSplitter(resultListsWidget);
    m_resultListSplitter->setOrientation(Qt::Vertical);
    m_moduleListBox = new CModuleResultView(m_resultListSplitter);
    m_resultListSplitter->addWidget(m_moduleListBox);
    m_resultListBox = new CSearchResultView(m_resultListSplitter);
    m_resultListSplitter->addWidget(m_resultListBox);
    resultListsWidgetLayout->addWidget(m_resultListSplitter);

    m_mainSplitter->addWidget(resultListsWidget);

    //Preview ("info") area
    m_displayFrame = new QFrame(m_mainSplitter);
    m_displayFrame->setFrameShape(QFrame::NoFrame);
    m_displayFrame->setFrameShadow(QFrame::Plain);
    m_mainSplitter->addWidget(m_displayFrame);

    mainLayout->addWidget(m_mainSplitter);

    QVBoxLayout* frameLayout = new QVBoxLayout(m_displayFrame);
    frameLayout->setContentsMargins(0, 0, 0, 0);
    m_previewDisplay = CDisplay::createReadInstance(0, m_displayFrame);
    m_previewDisplay->view()->setToolTip(tr("Text of the selected search result item"));
    frameLayout->addWidget(m_previewDisplay->view());

    QAction* selectAllAction = new QAction(QIcon(), tr("Select all"), this);
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    QObject::connect(selectAllAction, SIGNAL(triggered()), this, SLOT(selectAll()) );

    QAction* copyAction = new QAction(tr("Copy"), this);
    copyAction->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_C) );
    QObject::connect(copyAction, SIGNAL(triggered()), this, SLOT(copySelection()) );

    QMenu* menu = new QMenu();
    menu->addAction(selectAllAction);
    menu->addAction(copyAction);
    m_previewDisplay->installPopup(menu);

    loadDialogSettings();
}

void BtSearchResultArea::setSearchResult(
        const CSwordModuleSearch::Results &results)
{
    const QString searchedText = CSearchDialog::getSearchDialog()->searchText();
    reset(); //clear current modules

    m_results = results;

    // Populate listbox:
    m_moduleListBox->setupTree(results, searchedText);

    // Pre-select the first module in the list:
    m_moduleListBox->setCurrentItem(m_moduleListBox->topLevelItem(0), 0);

    Q_ASSERT(qobject_cast<CSearchDialog*>(parent()) != 0);
    static_cast<CSearchDialog*>(parent())->m_analyseButton->setEnabled(true);
}

void BtSearchResultArea::reset() {
    m_moduleListBox->clear();
    m_resultListBox->clear();
    m_previewDisplay->setText("<html><head/><body></body></html>");
    qobject_cast<CSearchDialog*>(parent())->m_analyseButton->setEnabled(false);
}

void BtSearchResultArea::clearPreview() {
    m_previewDisplay->setText("<html><head/><body></body></html>");
}

void BtSearchResultArea::updatePreview(const QString& key) {
    using namespace Rendering;

    CSwordModuleInfo* module = m_moduleListBox->activeModule();
    if ( module ) {
        const QString searchedText = CSearchDialog::getSearchDialog()->searchText();

        QString text;
        CDisplayRendering render;

        QList<const CSwordModuleInfo*> modules;
        modules.append(module);

        CTextRendering::KeyTreeItem::Settings settings;

        //for bibles render 5 context verses
        if (module->type() == CSwordModuleInfo::Bible) {
            CSwordVerseKey vk(module);
            vk.Headings(1);
            vk.setKey(key);

            ((sword::VerseKey*)(module->module()->getKey()))->Headings(1); //HACK: enable headings for VerseKeys

            //first go back and then go forward the keys to be in context
            vk.previous(CSwordVerseKey::UseVerse);
            vk.previous(CSwordVerseKey::UseVerse);

            //include Headings in display, they are indexed and searched too
            if (vk.Verse() == 1) {
                if (vk.Chapter() == 1) {
                    vk.Chapter(0);
                }
                vk.Verse(0);
            }

            const QString startKey = vk.key();

            vk.setKey(key);

            vk.next(CSwordVerseKey::UseVerse);
            vk.next(CSwordVerseKey::UseVerse);
            const QString endKey = vk.key();

            settings.keyRenderingFace = CTextRendering::KeyTreeItem::Settings::CompleteShort;
            text = render.renderKeyRange(startKey, endKey, modules, key, settings);
        }
        //for commentaries only one verse, but with heading
        else if (module->type() == CSwordModuleInfo::Commentary) {
            CSwordVerseKey vk(module);
            vk.Headings(1);
            vk.setKey(key);

            ((sword::VerseKey*)(module->module()->getKey()))->Headings(1); //HACK: enable headings for VerseKeys

            //include Headings in display, they are indexed and searched too
            if (vk.Verse() == 1) {
                if (vk.Chapter() == 1) {
                    vk.Chapter(0);
                }
                vk.Verse(0);
            }
            const QString startKey = vk.key();

            vk.setKey(key);
            const QString endKey = vk.key();

            settings.keyRenderingFace = CTextRendering::KeyTreeItem::Settings::NoKey;
            text = render.renderKeyRange(startKey, endKey, modules, key, settings);
        }
        else {
            text = render.renderSingleKey(key, modules, settings);
        }

        m_previewDisplay->setText( highlightSearchedText(text, searchedText) );
        m_previewDisplay->moveToAnchor( CDisplayRendering::keyToHTMLAnchor(key) );
    }
}

QStringList BtSearchResultArea::queryParser(const QString& queryString) {
    QString token;
    QStringList tokenList;
    int cnt, pos;

    token = "";
    cnt = 0;
    while (cnt < queryString.length()) {
        // add to token
        if ((queryString[cnt]).isLetterOrNumber() || (queryString[cnt] == '*')) {
            token = token + queryString[cnt];
            cnt++;
        }
        else if ((queryString[cnt]).isLetterOrNumber() || (queryString[cnt] == '?')) {
            token = token + queryString[cnt];
            cnt++;
        }
        // token break
        else if (queryString[cnt] == ' ') {
            token = token.simplified();
            if ((token != "*") && (token != ""))
                tokenList.append(token);
            token = "";
            cnt++;
        }
        // clucene appears to ignore quoted strings in the sence
        // that it treats all the words within quoted strings as
        // regular tokens and not as a single token.
        else if (queryString[cnt] == '"') {
            cnt++;
        }
        // wild card - treat as a special token break
        //else if (queryString[cnt] == '*') {
        //    token = token + queryString[cnt];
        //    token = token.simplified();
        //    if ((token != "*") && (token != ""))
        //        tokenList.append(token);
        //    // start next token with wildcard (kin*m -> kin* *m)
        //    token = "*";
        //    cnt++;
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
            cnt++;
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
            cnt++;
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
            cnt++;
        }
        // the || token is also a token break
        else if ((queryString[cnt] == '|') && (queryString[cnt+1] == '|')) {
            // store away current token
            token = token.simplified();
            if ((token != "*") && (token != ""))
                tokenList.append(token);
            // add the || token
            tokenList.append("||");
            token = "";
            cnt += 2;
        }
        // the && token is also a token break
        else if ((queryString[cnt] == '&') && (queryString[cnt+1] == '&')) {
            // store away current token
            token = token.simplified();
            if ((token != "*") && (token != ""))
                tokenList.append(token);
            // add the || token
            tokenList.append("&&");
            token = "";
            cnt += 2;
        }
        else cnt++;
    }
    token = token.simplified();
    if ((token != "*") && (token != ""))
        tokenList.append(token);

    cnt = 0;
    QStringList::iterator it;
    for ( it = tokenList.begin(); it != tokenList.end(); it++ ) {
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
            it--;
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
            it--;
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

QString BtSearchResultArea::highlightSearchedText(const QString& content, const QString& searchedText) {
    QString ret = content;

    const Qt::CaseSensitivity cs = Qt::CaseInsensitive;

    //   int index = 0;
    int index = ret.indexOf("<body", 0);
    int matchLen = 0;
    int length = searchedText.length();

    // Highlighting constants -
    // \todo We need to make the highlight color configurable.
    const QString rep1("<span style=\"background-color:#FFFF66;\">");
    const QString rep2("</span>");
    const unsigned int repLength = rep1.length() + rep1.length();
    const QString rep3("style=\"background-color:#FFFF66;\" ");
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
    qDebug() << "btsearchresultarea.cpp: " << __LINE__ << ": " <<  words << '\n';
    foreach (QString word, words) { //search for every word in the list
        QRegExp findExp;
        if (word.contains("*")) {
            length = word.length() - 1;
            word.replace('*', "\\S*"); //match within a word
            findExp = QRegExp(word);
            findExp.setMinimal(TRUE);
        }
        else if (word.contains("?")) {
            length = word.length() - 1;
            word.replace('?', "\\S?"); //match within a word
            findExp = QRegExp(word);
            findExp.setMinimal(TRUE);
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
    qDebug() << "btsearchresultarea.cpp: " << __LINE__ << ": " <<  words << '\n';
    //qWarning("\n\n\n%s", ret.latin1());
    return ret;
}

/** Initializes the signal slot conections of the child widgets, */
void BtSearchResultArea::initConnections() {
    connect(m_resultListBox, SIGNAL(keySelected(const QString&)), this, SLOT(updatePreview(const QString&)));
    connect(m_resultListBox, SIGNAL(keyDeselected()), this, SLOT(clearPreview()));
    connect(m_moduleListBox,
            SIGNAL(moduleSelected(const CSwordModuleInfo*, const sword::ListKey&)),
            m_resultListBox,
            SLOT(setupTree(const CSwordModuleInfo*, const sword::ListKey&)));
    connect(m_moduleListBox, SIGNAL(moduleChanged()), m_previewDisplay->connectionsProxy(), SLOT(clear()));

    // connect the strongs list
    connect(m_moduleListBox, SIGNAL(strongsSelected(CSwordModuleInfo*, const QStringList&)),
            m_resultListBox, SLOT(setupStrongsTree(CSwordModuleInfo*, const QStringList&)));
}

/**
* Load the settings from the resource file
*/
void BtSearchResultArea::loadDialogSettings() {
    QList<int> mainSplitterSizes = CBTConfig::get(CBTConfig::searchMainSplitterSizes);
    if (mainSplitterSizes.count() > 0) {
        m_mainSplitter->setSizes(mainSplitterSizes);
    }
    else {
        int w = this->size().width();
        int w2 = m_moduleListBox->sizeHint().width();
        mainSplitterSizes << w2 << w - w2;
        m_mainSplitter->setSizes(mainSplitterSizes);
    }
    QList<int> resultSplitterSizes = CBTConfig::get(CBTConfig::searchResultSplitterSizes);
    if (resultSplitterSizes.count() > 0) m_resultListSplitter->setSizes(resultSplitterSizes);
}

/**
* Save the settings to the resource file
*/
void BtSearchResultArea::saveDialogSettings() {
    CBTConfig::set(CBTConfig::searchMainSplitterSizes, m_mainSplitter->sizes());
    CBTConfig::set(CBTConfig::searchResultSplitterSizes, m_resultListSplitter->sizes());
}

/******************************************************************************
* StrongsResultList:
******************************************************************************/

StrongsResultList::StrongsResultList(const CSwordModuleInfo *module,
                                     const sword::ListKey &results,
                                     const QString &strongsNumber)
{
    using namespace Rendering;

    /// \warning This is a workaround for Sword constness
    sword::ListKey result = results;

    int count = result.Count();
    if (!count)
        return;

    CTextRendering::KeyTreeItem::Settings settings;
    QList<const CSwordModuleInfo*> modules;
    modules.append(module);
    clear();

    // for whatever reason the text "Parsing...translations." does not appear.
    // this is not critical but the text is necessary to get the dialog box
    // to be wide enough.
    QProgressDialog progress(QObject::tr("Parsing Strong's Numbers"), 0, 0, count);
    //0, "progressDialog", tr("Parsing Strong's Numbers"), tr("Parsing Strong's numbers for translations."), true);
    //progress->setAllowCancel(false);
    //progress->setMinimumDuration(0);
    progress.show();
    progress.raise();

    qApp->processEvents(QEventLoop::AllEvents, 1); //1 ms only

    for (int index = 0; index < count; index++) {
        progress.setValue(index);
        qApp->processEvents(QEventLoop::AllEvents, 1); //1 ms only

        QString key = QString::fromUtf8(result.GetElement(index)->getText());
        QString text = CDisplayRendering().renderSingleKey(key, modules, settings);
        for (int sIndex = 0;;) {
            continueloop:
            QString rText = getStrongsNumberText(text, sIndex, strongsNumber);
            if (rText.isEmpty()) break;

            for (iterator it = begin(); it != end(); ++it) {
                if ((*it).keyText() == rText) {
                    (*it).addKeyName(key);
                    goto continueloop; // break, then continue
                }
            }
            append(StrongsResult(rText, key));
        }
    }
}

QString StrongsResultList::getStrongsNumberText(const QString &verseContent,
                                                int &startIndex,
                                                const QString &lemmaText)
{
    // get the strongs text
    int idx1, idx2, index;
    QString sNumber, strongsText;
    //const bool cs = CSwordModuleSearch::caseSensitive;
    const Qt::CaseSensitivity cs = Qt::CaseInsensitive;

    if (startIndex == 0) {
        index = verseContent.indexOf("<body");
    }
    else {
        index = startIndex;
    }

    // find all the "lemma=" inside the the content
    while ((index = verseContent.indexOf("lemma=", index, cs)) != -1) {
        // get the strongs number after the lemma and compare it with the
        // strongs number we are looking for
        idx1 = verseContent.indexOf("\"", index) + 1;
        idx2 = verseContent.indexOf("\"", idx1 + 1);
        sNumber = verseContent.mid(idx1, idx2 - idx1);
        if (sNumber == lemmaText) {
            // strongs number is found now we need to get the text of this node
            // search right until the ">" is found.  Get the text from here to
            // the next "<".
            index = verseContent.indexOf(">", index, cs) + 1;
            idx2  = verseContent.indexOf("<", index, cs);
            strongsText = verseContent.mid(index, idx2 - index);
            index = idx2;
            startIndex = index;

            return(strongsText);
        }
        else {
            index += 6; // 6 is the length of "lemma="
        }
    }
    return QString::null;
}



} //namespace Search
