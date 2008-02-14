/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "csearchdialogareas.h"
#include "csearchdialogareas.moc"

#include "csearchdialog.h"
#include "crangechooserdialog.h"
#include "cmoduleresultview.h"
#include "csearchresultview.h"
#include "csearchmodulechooserdialog.h"
#include "frontend/searchdialog/analysis/csearchanalysisdialog.h"

#include "backend/keys/cswordversekey.h"
#include "backend/rendering/cdisplayrendering.h"

#include "frontend/display/cdisplay.h"
#include "frontend/display/creaddisplay.h"

#include "util/cresmgr.h"
#include "util/ctoolclass.h"
#include "util/directoryutil.h"

//Qt includes
#include <QApplication>
#include <QLayout>
#include <QPushButton>
#include <QLabel>
#include <QRegExp>
#include <QVBoxLayout>
#include <QProgressDialog>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QFrame>
#include <QPushButton>
#include <QSpacerItem>
#include <QSplitter>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QLineEdit>
#include <QKeyEvent>

#include <QDebug>

//KDE includes

#include <kcombobox.h>
#include <khistorycombobox.h>


namespace Search {

StrongsResult::StrongsResult()
{
}

StrongsResult::StrongsResult(const QString& text, const QString &keyName)
	   : text(text)
{
	//keyNameList.clear();
	keyNameList.append(keyName);
}

QString StrongsResult::keyText() const {
	return text;
}

int StrongsResult::keyCount() const {
	return keyNameList.count();
}

void StrongsResult::addKeyName(const QString& keyName) {
	if (keyNameList.indexOf(keyName) == -1)
		keyNameList.append(keyName);
}

QStringList* StrongsResult::getKeyList() {
	return & keyNameList;
}



/********************************************
************  StrongsResultClass *************
********************************************/
void StrongsResultClass::initStrongsResults(void)
{
	using namespace Rendering;
	
	CDisplayRendering render;
	ListCSwordModuleInfo modules;
	CTextRendering::KeyTreeItem::Settings settings;
	QString rText, lText, key;
	bool found;
	int sIndex;
	int count;
	int index;
	QString text;

	modules.append(srModule);
	sword::ListKey& result = srModule->searchResult();

	count = result.Count();
	if (!count)
		return;
	qApp->processEvents( QEventLoop::AllEvents, 1 ); //1 ms only
	srList.clear();
	// for whatever reason the text "Parsing...translations." does not appear.
	// this is not critical but the text is necessary to get the dialog box
	// to be wide enough.
	QProgressDialog* progress = new QProgressDialog(QObject::tr("Parsing Stong's Numbers"), 0, 0, count);
	//0, "progressDialog", tr("Parsing Stong's Numbers"), tr("Parsing Stong's numbers for translations."), true);
	
	//progress->setAllowCancel(false);
	//progress->setMinimumDuration(0);
	progress->show();
	progress->raise();
	for (index = 0; index < count; index++){
		progress->setValue( index );
 		qApp->processEvents(QEventLoop::AllEvents, 1 ); //1 ms only

		key = QString::fromUtf8(result.GetElement(index)->getText());
		text = render.renderSingleKey(key, modules, settings);
		sIndex = 0;
		while ((rText = getStrongsNumberText(text, &sIndex)) != "")
			{
			StrongsResultList::iterator it;
			found = FALSE;
			for ( it = srList.begin(); it != srList.end(); ++it )
				{
				lText = (*it).keyText();
				if (lText == rText)
					{
					found = TRUE;
					(*it).addKeyName(key);
					break;
					}
				}
			if (found == FALSE)
				srList.append( StrongsResult(rText, key) );
			}
		}
	  delete progress;
	  progress = 0;
	//qHeapSort(srList);
}

QString StrongsResultClass::getStrongsNumberText(const QString& verseContent, int *startIndex) {
	// get the strongs text
	int idx1, idx2, index;
	QString sNumber, strongsText;
	//const bool cs = CSwordModuleSearch::caseSensitive;
	const Qt::CaseSensitivity cs = Qt::CaseInsensitive;
	
	if (*startIndex == 0) {
		index = verseContent.indexOf("<body");
	}
	else {
		index = *startIndex;
	}
   
	// find all the "lemma=" inside the the content
	while((index = verseContent.indexOf("lemma=", index, cs)) != -1) {
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
			*startIndex = index;
 
			return(strongsText);
		}
		else {
			index += 6; // 6 is the length of "lemma="
		}
   }
   return QString::null;
}

/********************************************
**********  CSearchDialogResultArea *********
********************************************/

CSearchResultArea::CSearchResultArea(QWidget *parent)
	: QWidget(parent)
{
	qDebug("CSearchResultArea::CSearchResultArea");
	setParent(parent);
	initView();
	initConnections();
	qDebug("CSearchResultArea::CSearchResultArea end");
}

CSearchResultArea::~CSearchResultArea() {}

void CSearchResultArea::initView()
{
	QVBoxLayout *mainLayout;
	QSplitter *mainSplitter;
	QWidget *resultListsWidget;
	QVBoxLayout *resultListsWidgetLayout;
	QSplitter *resultListSplitter;
	QHBoxLayout *hboxLayout;
	QSpacerItem *spacerItem;

	//Size is calculated from the font rather than set in pixels,
	// maybe this is better in different kinds of displays?
	int width = this->fontMetrics().width(QString().fill('M', 50));
	QSize size = QSize(width, width );
	this->setMinimumSize(QSize(width, width/2));
	this->setBaseSize(QSize(width, width/2));
	mainLayout = new QVBoxLayout(this);
	mainSplitter = new QSplitter(this);
	mainSplitter->setOrientation(Qt::Horizontal);

	//we have to create a widget (layout is not enough) to set the stretch factor
	resultListsWidget = new QWidget(mainSplitter);
	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	sizePolicy.setHorizontalStretch(1);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(resultListsWidget->sizePolicy().hasHeightForWidth());
	resultListsWidget->setSizePolicy(sizePolicy);
	resultListsWidgetLayout = new QVBoxLayout(resultListsWidget);
	resultListsWidgetLayout->setContentsMargins(0, 0, 0, 0);

	//Splitter for two result lists
	resultListSplitter = new QSplitter(resultListsWidget);
	resultListSplitter->setOrientation(Qt::Vertical);
	m_moduleListBox = new CModuleResultView(resultListSplitter);
	resultListSplitter->addWidget(m_moduleListBox);
	m_resultListBox = new CSearchResultView(resultListSplitter);
	resultListSplitter->addWidget(m_resultListBox);
	resultListsWidgetLayout->addWidget(resultListSplitter);

	//analyze button
	hboxLayout = new QHBoxLayout();
	m_analyseButton = new QPushButton(tr("&Analyze search..."), resultListsWidget);
	hboxLayout->addWidget(m_analyseButton);
	spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	hboxLayout->addItem(spacerItem);
	resultListsWidgetLayout->addLayout(hboxLayout);

	mainSplitter->addWidget(resultListsWidget);

	//Preview ("info") area
	m_displayFrame = new QFrame(mainSplitter);
	m_displayFrame->resize(this->fontMetrics().width(QString().fill('M', 25)), 100);
	m_displayFrame->setMinimumSize(this->fontMetrics().width(QString().fill('M', 20)), 50);
	QSizePolicy sizePolicy2(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	sizePolicy2.setHorizontalStretch(1);
	sizePolicy2.setVerticalStretch(0);
	sizePolicy2.setHeightForWidth(m_displayFrame->sizePolicy().hasHeightForWidth());
	m_displayFrame->setSizePolicy(sizePolicy2);
	m_displayFrame->setFrameShape(QFrame::NoFrame);
	m_displayFrame->setFrameShadow(QFrame::Plain);
	mainSplitter->addWidget(m_displayFrame);

	mainLayout->addWidget(mainSplitter);

	QVBoxLayout* frameLayout = new QVBoxLayout(m_displayFrame);
	frameLayout->setContentsMargins(0,0,0,0);
	m_previewDisplay = CDisplay::createReadInstance(0, m_displayFrame);
	frameLayout->addWidget(m_previewDisplay->view());
}

void CSearchResultArea::setSearchResult(ListCSwordModuleInfo modules)
{
	const QString searchedText = CSearchDialog::getSearchDialog()->searchText();
	reset(); //clear current modules

	m_modules = modules;
	//pre-select the first module in the list
	//this will pre-select and display the first hit of that module
	m_moduleListBox->setupTree(modules, searchedText);
	m_moduleListBox->setCurrentItem(m_moduleListBox->topLevelItem(0), 0);

	m_analyseButton->setEnabled(true);
}

void CSearchResultArea::reset()
{
	m_moduleListBox->clear();
	m_resultListBox->clear();
	m_previewDisplay->setText("<html><head/><body></body></html>");
	m_analyseButton->setEnabled(false);
	//   m_modules.setAutoDelete(false); //make sure we don't delete modules accidentally
	m_modules.clear();
}

void CSearchResultArea::clearPreview(){
	m_previewDisplay->setText("<html><head/><body></body></html>");
}

void CSearchResultArea::updatePreview(const QString& key)
{
	using namespace Rendering;

	CSwordModuleInfo* module = m_moduleListBox->activeModule();
	if ( module ) {
		const QString searchedText = CSearchDialog::getSearchDialog()->searchText();
		//const int searchFlags = CSearchDialog::getSearchDialog()->searchFlags();

		QString text;
		CDisplayRendering render;

		ListCSwordModuleInfo modules;
		modules.append(module);

		CTextRendering::KeyTreeItem::Settings settings;

		//for bibles render 5 context verses
		if (module->type() == CSwordModuleInfo::Bible) {
			CSwordVerseKey vk(module);
			vk.Headings(1);
			vk.key(key);

			((sword::VerseKey*)(module->module()->getKey()))->Headings(1); //HACK: enable headings for VerseKeys

			//first go back and then go forward the keys to be in context
			vk.previous(CSwordVerseKey::UseVerse);
			vk.previous(CSwordVerseKey::UseVerse);
			
			//include Headings in display, they are indexed and searched too
			if (vk.Verse() == 1){
				if (vk.Chapter() == 1){
					vk.Chapter(0);
				}
				vk.Verse(0);
			}
			
			const QString startKey = vk.key();

			vk.key(key);

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
			vk.key(key);
			
			((sword::VerseKey*)(module->module()->getKey()))->Headings(1); //HACK: enable headings for VerseKeys

			//include Headings in display, they are indexed and searched too
			if (vk.Verse() == 1){
				if (vk.Chapter() == 1){
					vk.Chapter(0);
				}
				vk.Verse(0);
			}
			const QString startKey = vk.key();

			vk.key(key);
			const QString endKey = vk.key();

			settings.keyRenderingFace = CTextRendering::KeyTreeItem::Settings::NoKey;
			text = render.renderKeyRange(startKey, endKey, modules, key, settings);
		}
		else {
			text = render.renderSingleKey(key, modules, settings);
		}

		m_previewDisplay->setText( highlightSearchedText(text, searchedText/*, searchFlags*/) );
		m_previewDisplay->moveToAnchor( CDisplayRendering::keyToHTMLAnchor(key) );
	}
}

QStringList CSearchResultArea::QueryParser(const QString& queryString) {
	QString token;
	QStringList tokenList;
	int cnt, pos;

	token = "";
	cnt = 0;
	while(cnt < queryString.length()) {
		// add to token
		if ((queryString[cnt]).isLetterOrNumber() || (queryString[cnt] == '*')) {
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
		//	token = token + queryString[cnt];
		//	token = token.simplified();
		//	if ((token != "*") && (token != ""))
		//		tokenList.append(token);
		//	// start next token with wildcard (kin*m -> kin* *m)
		//	token = "*";
		//	cnt++;
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
		     ((*it) == "AND") || ((*it) == "&&") )
		{
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

const QString CSearchResultArea::highlightSearchedText(const QString& content, const QString& searchedText/*, const int searchFlags*/) {
	QString ret = content;

	//const bool cs = (searchFlags & CSwordModuleSearch::caseSensitive);
	const Qt::CaseSensitivity cs = Qt::CaseInsensitive;
	
	//   int index = 0;
	int index = ret.indexOf("<body", 0);
	int matchLen = 0;
	int length = searchedText.length();

	// Highlighting constants - 
	// TODO: We need to make the highlight color configurable.
	const QString rep1("<span style=\"background-color:#FFFF66;\">");
	const QString rep2("</span>");
	const unsigned int repLength = rep1.length() + rep1.length();
	const QString rep3("style=\"background-color:#FFFF66;\" ");
	const unsigned int rep3Length = rep3.length();

	
	bool inQuote;
	QString newSearchText;

	newSearchText = searchedText;
	
	// find the strongs search lemma and highlight it
	// search the searched text for "strong:" until it is not found anymore
	QStringList list;
	
	// split the search string - some possibilities are "\\s|\\|", "\\s|\\+", or "\\s|\\|\\+"
	// TODO: find all possible seperators
	QString regExp = "\\s";
	list = searchedText.split(QRegExp(regExp));
	foreach (QString newSearchText, list) {		
		int sstIndex; // strong search text index for finding "strong:"
		int idx1, idx2, sTokenIndex, sTokenIndex2;
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
		while((strongIndex = ret.indexOf("lemma=", strongIndex, cs)) != -1) {
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
	//boost::scoped_ptr<Query> q( QueryParser::parse(m_wcharBuffer, _T("content"), &analyzer) );
	//StringReader reader(m_wcharBuffer);
	//TokenStream* tokenStream = analyzer.tokenStream( _T("field"), &reader);
	//Token token;
	//while(tokenStream->next(&token) != 0) {
	//	lucene_wcstoutf8(buf8, token.termText(), 1000);
	//	printf("%s\n", buf8);
	//}

	//===========================================================
	// since I could not figure out the lucene query parser, I
	// made a simple parser.
	//===========================================================
	QStringList words = QueryParser(newSearchText);
	for ( int wi = 0; (unsigned int)wi < words.count(); ++wi ) { //search for every word in the list
		QRegExp findExp;
		QString word = words[ wi ];
		if (word.contains("*")) {
			length = word.length() - 1;
			word.replace('*', "\\S*"); //match within a word
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
			if (!CToolClass::inHTMLTag(index, ret)) {
				length = matchLen;
				ret = ret.insert( index+length, rep2 );
				ret = ret.insert( index, rep1 );
				index += repLength;
			}
			index += length;
		}
	}
	//qWarning("\n\n\n%s", ret.latin1());
	return ret;
};

/** Initializes the signal slot conections of the child widgets, */
void CSearchResultArea::initConnections()
{
	connect(m_resultListBox, SIGNAL(keySelected(const QString&)), this, SLOT(updatePreview(const QString&)));
	connect(m_resultListBox, SIGNAL(keyDeselected()), this, SLOT(clearPreview()));
	connect(m_moduleListBox, SIGNAL(moduleSelected(CSwordModuleInfo*)), m_resultListBox, SLOT(setupTree(CSwordModuleInfo*)));
	connect(m_moduleListBox, SIGNAL(moduleChanged()), m_previewDisplay->connectionsProxy(), SLOT(clear()));
	connect(m_analyseButton, SIGNAL(clicked()), SLOT(showAnalysis()));
	// connect the strongs list
	connect(m_moduleListBox, SIGNAL(strongsSelected(CSwordModuleInfo*, QStringList*)), 
		m_resultListBox, SLOT(setupStrongsTree(CSwordModuleInfo*, QStringList*)));
}

/** Shows a dialog with the search analysis of the current search. */
void CSearchResultArea::showAnalysis() {
	CSearchAnalysisDialog dlg(m_modules, this);
	dlg.exec();
}

//--------------CSearchOptionsArea------------------------

CSearchOptionsArea::CSearchOptionsArea(QWidget *parent )
	: QWidget(parent)
{
	setParent(parent);
	initView();
	readSettings();
}

CSearchOptionsArea::~CSearchOptionsArea() {
	saveSettings();
}

const QString CSearchOptionsArea::searchText() {
	return m_searchTextCombo->currentText();
}

void CSearchOptionsArea::setSearchText(const QString& text) {
	bool found = false;
	int i = 0;
	for (i = 0; !found && i < m_searchTextCombo->count(); ++i) {
		if (m_searchTextCombo->itemText(i) == text) {
			found = true;
		}
	}
   // This is needed because in the for loop i is incremented before the comparison (++i)
   // As a result the index i is actually one greater than expected.
   i--;
	if (!found) {
		i = 0;
		m_searchTextCombo->insertItem(0, text );
	}

	m_searchTextCombo->setCurrentIndex(i);
	m_searchTextCombo->reset();
	m_searchTextCombo->setFocus();
}

void CSearchOptionsArea::initView() {
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    this->setSizePolicy(sizePolicy);
    this->setMinimumSize(QSize(260, 130));
    this->setBaseSize(QSize(420, 150));
    hboxLayout = new QHBoxLayout(this);
    hboxLayout->setSpacing(3);
    hboxLayout->setMargin(0);

    searchGroupBox = new QGroupBox(this);

    gridLayout = new QGridLayout(searchGroupBox);
    gridLayout->setSpacing(3);
    gridLayout->setMargin(0);

    gridLayout->setHorizontalSpacing(3);
    gridLayout->setVerticalSpacing(3);
    gridLayout->setContentsMargins(6, 6, 6, 6);
    m_searchTextLabel = new QLabel(tr("Search for:"), searchGroupBox);

    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(m_searchTextLabel->sizePolicy().hasHeightForWidth());
    m_searchTextLabel->setSizePolicy(sizePolicy1);
    m_searchTextLabel->setWordWrap(false);

    gridLayout->addWidget(m_searchTextLabel, 0, 0, 1, 1);

    m_syntaxButton = new QPushButton(tr("&Help"), searchGroupBox);
    gridLayout->addWidget(m_syntaxButton, 0, 2, 1, 1);

    m_chooseModulesButton = new QPushButton(tr("Ch&oose"), searchGroupBox);
    gridLayout->addWidget(m_chooseModulesButton, 1, 2, 1, 1);

    m_chooseRangeButton = new QPushButton(tr("S&etup"), searchGroupBox);
    gridLayout->addWidget(m_chooseRangeButton, 2, 2, 1, 1);

    m_searchScopeLabel = new QLabel(tr("Search scope:"), searchGroupBox);
    sizePolicy1.setHeightForWidth(m_searchScopeLabel->sizePolicy().hasHeightForWidth());
    m_searchScopeLabel->setSizePolicy(sizePolicy1);
    m_searchScopeLabel->setWordWrap(false);
    gridLayout->addWidget(m_searchScopeLabel, 2, 0, 1, 1);

    m_rangeChooserCombo = new KComboBox(searchGroupBox);
    QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(m_rangeChooserCombo->sizePolicy().hasHeightForWidth());
    m_rangeChooserCombo->setSizePolicy(sizePolicy2);
    gridLayout->addWidget(m_rangeChooserCombo, 2, 1, 1, 1);

    m_searchTextCombo = new KHistoryComboBox(searchGroupBox);
    sizePolicy2.setHeightForWidth(m_searchTextCombo->sizePolicy().hasHeightForWidth());
    m_searchTextCombo->setSizePolicy(sizePolicy2);
    m_searchTextCombo->setFocusPolicy(Qt::WheelFocus);
    m_searchTextCombo->setProperty("sizeLimit", QVariant(25));
    m_searchTextCombo->setProperty("duplicatesEnabled", QVariant(false));

    gridLayout->addWidget(m_searchTextCombo, 0, 1, 1, 1);
	
	//Modules label, the text will be set later
	m_modulesLabel = new QLabel( searchGroupBox);
	sizePolicy1.setHeightForWidth(m_modulesLabel->sizePolicy().hasHeightForWidth());
	m_modulesLabel->setSizePolicy(sizePolicy1);
	m_modulesLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    m_modulesLabel->setWordWrap(true);

	gridLayout->addWidget(m_modulesLabel, 1, 0, 1, 2);


	hboxLayout->addWidget(searchGroupBox);

	Q_ASSERT(m_searchTextCombo);
	QObject::connect( m_searchTextCombo, SIGNAL(activated( const QString& )),
				this, SLOT( slotHistoryItemActivated(const QString&) )
			);
	QObject::connect( m_searchTextCombo->lineEdit(), SIGNAL(returnPressed ( const QString& )),
				this, SLOT( slotSearchTextEditReturnPressed(const QString&) )
			);

	m_searchTextCombo->setToolTip(CResMgr::searchdialog::options::searchedText::tooltip);

	Q_ASSERT(m_syntaxButton);
	m_syntaxButton->setIcon(util::filesystem::DirectoryUtil::getIcon("contexthelp"));
	connect( m_syntaxButton, SIGNAL(clicked()), this, SLOT(syntaxHelp()));

	Q_ASSERT(m_chooseModulesButton);
	m_chooseModulesButton->setIcon(util::filesystem::DirectoryUtil::getIcon("wizard"));
	connect(m_chooseModulesButton, SIGNAL(clicked()), this, SLOT(chooseModules()));
	
	m_chooseModulesButton->setToolTip( CResMgr::searchdialog::options::moduleChooserButton::tooltip);
	Q_ASSERT(m_rangeChooserCombo);
	m_rangeChooserCombo->setToolTip( CResMgr::searchdialog::options::chooseScope::tooltip);
	refreshRanges();

	Q_ASSERT(m_chooseRangeButton);
	m_chooseRangeButton->setIcon(util::filesystem::DirectoryUtil::getIcon("configure"));
	connect(m_chooseRangeButton, SIGNAL(clicked()), this, SLOT(setupRanges()));

	//set the initial focus
	Q_ASSERT(m_searchTextCombo);
	m_searchTextCombo->setFocus();	

	// event filter to prevent the Return/Enter presses in the combo box doing something
	// in the parent widget
	//m_searchTextCombo->view()->installEventFilter(this);
	m_searchTextCombo->installEventFilter(this);
}

/** Sets the modules used by the search. */
void CSearchOptionsArea::setModules( ListCSwordModuleInfo modules ) {
	QString t = tr("Searching in: ");

	m_modules.clear(); //remove old modules
	ListCSwordModuleInfo::iterator end_it = modules.end();

	for (ListCSwordModuleInfo::iterator it(modules.begin()); it != end_it; ++it) {
		//ToDo:  Check for containsRef compat
		if (*it == 0) { //don't operate on null modules.
			continue;
		}
		
		if ( !m_modules.contains(*it) ) {
			m_modules.append( *it );
			t.append( (*it)->name() );
			if (*it != modules.last()) {
				t += QString::fromLatin1(", "); // so that it will become a readable list (WLC, LXX, GerLut...)
			}
		}
	};
	m_modulesLabel->setText(t);
	emit( sigSetSearchButtonStatus( (modules.count() != 0) ) );
}

void CSearchOptionsArea::chooseModules() {
	QString title(tr("Modules to Search in"));
	QString label(tr("Select the modules in which the search should be run."));
	CSearchModuleChooserDialog* dlg = new CSearchModuleChooserDialog(this, title, label, modules());
	connect(dlg, SIGNAL(modulesChanged(ListCSwordModuleInfo, QTreeWidget*)), this, SLOT(setModules(ListCSwordModuleInfo)));
	dlg->exec();
}

const ListCSwordModuleInfo CSearchOptionsArea::modules() {
	return m_modules;
}

void CSearchOptionsArea::reset() {
	m_rangeChooserCombo->setCurrentItem(0); //no scope
	m_searchTextCombo->clearEditText();
}

void CSearchOptionsArea::saveSettings() {
	CBTConfig::set(CBTConfig::searchCompletionTexts, m_searchTextCombo->completionObject()->items());
	CBTConfig::set(CBTConfig::searchTexts, m_searchTextCombo->historyItems());
}

void CSearchOptionsArea::readSettings() {
	m_searchTextCombo->completionObject()->setItems( CBTConfig::get(CBTConfig::searchCompletionTexts) );
	m_searchTextCombo->setHistoryItems( CBTConfig::get(CBTConfig::searchTexts) );
}

void CSearchOptionsArea::aboutToShow() {
	m_searchTextCombo->setFocus();
}

void CSearchOptionsArea::setupRanges() {
	CRangeChooserDialog* chooser = new CRangeChooserDialog(this);
	chooser->exec();

	refreshRanges();
}

void CSearchOptionsArea::syntaxHelp() {
	QString syntax = tr(
			"<p>Enter search terms separated by spaces.  By default the search "
			"function will return results that match any of the search terms (OR). "
			"To search for all the terms separate the terms by AND.</p>") + tr(
			"<p>You can use wildcards: '*' matches any sequence of characters, "
			"while '?' matches any single character. The use of brackets allows you to "
			"group your search terms, e.g. '(Jesus OR spirit) AND God'.</p>") + tr(
			"<p>To search text other than the main text, enter the text type followed "
			"by \":\", and then the search term.  For example, to search for the Strong's "
			"number H8077, use 'strong:H8077'.</p>") + tr(
			"<p>Available text types:<br><table>"
			"<tr><td>heading:</td><td>searches headings</td></tr>"
			"<tr><td>footnote:</td><td>searches footnotes</td></tr>"
			"<tr><td>strong:</td><td>searches Strong's Numbers</td></tr>"
			"<tr><td>morph:</td><td>searches morphology codes</td></tr></table></p>") + tr(
			"<p>BibleTime uses the Lucene search engine to perform your searches. "
			"It has many advanced features, and you can read more about it here: "
			"<a href=\"http://lucene.apache.org/java/docs/queryparsersyntax.html\">"
			"http://lucene.apache.org/java/docs/queryparsersyntax.html</a></p>");

	QMessageBox::about( this, tr("Basic Search Syntax Introduction"), syntax);
}

void CSearchOptionsArea::refreshRanges() {
	//the first two options are fixed, the others can be edited using the "Setup ranges" button.
	m_rangeChooserCombo->clear();
	m_rangeChooserCombo->insertItem(0, tr("No search scope"));
	//m_rangeChooserCombo->insertItem(tr("Last search result"));

	//insert the user-defined ranges
	CBTConfig::StringMap map = CBTConfig::get(CBTConfig::searchScopes);
	CBTConfig::StringMap::Iterator it;
	for (it = map.begin(); it != map.end(); ++it) {
		m_rangeChooserCombo->insertItem(0, it.key());
	};
}

sword::ListKey CSearchOptionsArea::searchScope() {
	if (m_rangeChooserCombo->currentIndex() > 0) { //is not "no scope"
		CBTConfig::StringMap map = CBTConfig::get(CBTConfig::searchScopes);
		QString scope = map[ m_rangeChooserCombo->currentText() ];
		if (!scope.isEmpty()) {
			return sword::VerseKey().ParseVerseList( (const char*)scope.toUtf8(), "Genesis 1:1", true);
		}
	}
	return sword::ListKey();
}

bool CSearchOptionsArea::hasSearchScope() {
	return (searchScope().Count() > 0);
}

void CSearchOptionsArea::slotHistoryItemActivated(const QString& item)
{
	qDebug("CSearchOptionsArea::slotHistoryItemActivated");
	m_searchTextCombo->addToHistory( item );
}

void CSearchOptionsArea::slotSearchTextEditReturnPressed(const QString& item)
{
	qDebug("CSearchOptionsArea::slotSearchTextEditReturnPressed");
	m_searchTextCombo->addToHistory( item );
	emit sigStartSearch();
}

bool CSearchOptionsArea::eventFilter(QObject* obj, QEvent* event)
{
	if (obj == m_searchTextCombo->view() || obj == m_searchTextCombo || obj == m_searchTextCombo->lineEdit()) {
		//qDebug() << "CSearchOptionsArea::eventFilter" << obj << event;
		obj->event(event);
		// don't handle this event in parent
		event->accept();
		return true;
	}
	return QWidget::eventFilter(obj, event);
}

} //end of namespace Search
