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
#include "chistorycombobox.h"
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
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QLineEdit>
#include <QKeyEvent>
#include <QSettings>
#include <QComboBox>

#include <QDebug>

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
	QList<CSwordModuleInfo*> modules;
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
	QProgressDialog* progress = new QProgressDialog(QObject::tr("Parsing Strong's Numbers"), 0, 0, count);
	//0, "progressDialog", tr("Parsing Strong's Numbers"), tr("Parsing Strong's numbers for translations."), true);
	
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
	initView();
	initConnections();
	qDebug("CSearchResultArea::CSearchResultArea end");
}

CSearchResultArea::~CSearchResultArea() 
{
	saveDialogSettings();
}

void CSearchResultArea::initView()
{
	QVBoxLayout *mainLayout;
	QWidget *resultListsWidget;
	QVBoxLayout *resultListsWidgetLayout;
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
	//disabled temporarily until analyze window is fixed:
	//m_analyseButton = new QPushButton(tr("&Analyze search..."), resultListsWidget);
	m_analyseButton = new QPushButton(tr("&Analyze search..."), 0);
	//m_analyzeButton->setToolTip(tr("Show a graphical analyzis of the search result"));
	//hboxLayout->addWidget(m_analyseButton);
	spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	hboxLayout->addItem(spacerItem);
	resultListsWidgetLayout->addLayout(hboxLayout);

	mainSplitter->addWidget(resultListsWidget);

	//Preview ("info") area
	m_displayFrame = new QFrame(mainSplitter);
	m_displayFrame->resize(this->fontMetrics().width(QString().fill('M', 25)), 100);
	m_displayFrame->setMinimumSize(this->fontMetrics().width(QString().fill('M', 20)), 50);
	QSizePolicy sizePolicy2(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	// preview area will expand 2-to-1 compared to the modules area
	sizePolicy2.setHorizontalStretch(2);
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
	m_previewDisplay->view()->setToolTip(tr("Text of the selected search result item"));
	frameLayout->addWidget(m_previewDisplay->view());
	
	loadDialogSettings();
}

void CSearchResultArea::setSearchResult(QList<CSwordModuleInfo*> modules)
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

		QList<CSwordModuleInfo*> modules;
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
	foreach (QString word, words) { //search for every word in the list
		QRegExp findExp;
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
}

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

/**
* Load the settings from the resource file
*/
void CSearchResultArea::loadDialogSettings()
{
	QList<int> mainSplitterSizes = CBTConfig::get(CBTConfig::searchMainSplitterSizes);
	if (mainSplitterSizes.count() > 0) mainSplitter->setSizes(mainSplitterSizes);
	QList<int> resultSplitterSizes = CBTConfig::get(CBTConfig::searchResultSplitterSizes); 
	if (resultSplitterSizes.count() > 0) resultListSplitter->setSizes(resultSplitterSizes);
}

/**
* Save the settings to the resource file
*/
void CSearchResultArea::saveDialogSettings()
{
	CBTConfig::set(CBTConfig::searchMainSplitterSizes, mainSplitter->sizes());
	CBTConfig::set(CBTConfig::searchResultSplitterSizes, resultListSplitter->sizes());
}


//--------------CSearchOptionsArea------------------------

CSearchOptionsArea::CSearchOptionsArea(QWidget *parent )
	: QWidget(parent)
{
	initView();
	initConnections();
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
	m_searchTextCombo->setFocus();
}

void CSearchOptionsArea::initView()
{
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

	// ******** label for search text editor***********
    m_searchTextLabel = new QLabel(tr("Search for:"), searchGroupBox);
    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(m_searchTextLabel->sizePolicy().hasHeightForWidth());
    m_searchTextLabel->setSizePolicy(sizePolicy1);
    m_searchTextLabel->setWordWrap(false);
    gridLayout->addWidget(m_searchTextLabel, 0, 0);

	// **********Buttons******************
	m_syntaxButton = new QPushButton(tr("&Help..."), searchGroupBox);
	gridLayout->addWidget(m_syntaxButton, 0, 2);
	m_syntaxButton->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::searchdialog::help_icon));
	m_syntaxButton->setToolTip("Show short help for the search syntax");

	m_chooseModulesButton = new QPushButton(tr("Ch&oose..."), searchGroupBox);
	m_chooseModulesButton->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::searchdialog::chooseworks_icon));
	m_chooseModulesButton->setToolTip( tr("Choose works for the search"));
	gridLayout->addWidget(m_chooseModulesButton, 1, 2);

	m_chooseRangeButton = new QPushButton(tr("S&etup..."), searchGroupBox);
	m_chooseRangeButton->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::searchdialog::setupscope_icon));
	m_chooseRangeButton->setToolTip("Configure predefined scopes for search");
	gridLayout->addWidget(m_chooseRangeButton, 2, 2);

	// ************* Label for search range/scope selector *************
	m_searchScopeLabel = new QLabel(tr("Scope:"), searchGroupBox);
	sizePolicy1.setHeightForWidth(m_searchScopeLabel->sizePolicy().hasHeightForWidth());
	m_searchScopeLabel->setSizePolicy(sizePolicy1);
	m_searchScopeLabel->setWordWrap(false);
	gridLayout->addWidget(m_searchScopeLabel, 2, 0);

	// ***********Range/scope selector combo box***********
	m_rangeChooserCombo = new QComboBox(searchGroupBox);
	QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
	sizePolicy2.setHorizontalStretch(0);
	sizePolicy2.setVerticalStretch(0);
	sizePolicy2.setHeightForWidth(m_rangeChooserCombo->sizePolicy().hasHeightForWidth());
	m_rangeChooserCombo->setSizePolicy(sizePolicy2);
	m_rangeChooserCombo->setToolTip(tr("Choose the scope (books/chapters/verses to search in) from the list. Applicable for Bibles and commentaries."));
	gridLayout->addWidget(m_rangeChooserCombo, 2, 1);

	// ************* Search text combo box *******************
	m_searchTextCombo = new CHistoryComboBox(searchGroupBox);
	sizePolicy2.setHeightForWidth(m_searchTextCombo->sizePolicy().hasHeightForWidth());
	m_searchTextCombo->setSizePolicy(sizePolicy2);
	m_searchTextCombo->setFocusPolicy(Qt::WheelFocus);
	m_searchTextCombo->setProperty("sizeLimit", QVariant(25));
	m_searchTextCombo->setProperty("duplicatesEnabled", QVariant(false));
	m_searchTextCombo->setToolTip(tr("The text you want to search for"));
	m_searchTextCombo->setInsertPolicy(QComboBox::NoInsert);
	gridLayout->addWidget(m_searchTextCombo, 0, 1);
	
	m_modulesLabel = new QLabel(tr("Works:"), searchGroupBox);
	gridLayout->addWidget(m_modulesLabel, 1, 0);
	
	m_modulesCombo = new QComboBox(searchGroupBox);
	m_modulesCombo->setDuplicatesEnabled(false);
	gridLayout->addWidget(m_modulesCombo, 1, 1);

	hboxLayout->addWidget(searchGroupBox);

	refreshRanges();
	//set the initial focus
	m_searchTextCombo->setFocus();
	// event filter to prevent the Return/Enter presses in the combo box doing something
	// in the parent widget
	m_searchTextCombo->installEventFilter(this);
}

void CSearchOptionsArea::initConnections()
{
	QObject::connect( m_searchTextCombo->lineEdit(), SIGNAL(returnPressed ()),
				this, SLOT( slotSearchTextEditReturnPressed() )
			);
	connect( m_syntaxButton, SIGNAL(clicked()), this, SLOT(syntaxHelp()));
	connect(m_chooseModulesButton, SIGNAL(clicked()), this, SLOT(chooseModules()));
	connect(m_chooseRangeButton, SIGNAL(clicked()), this, SLOT(setupRanges()));
	connect(m_modulesCombo, SIGNAL(activated(int)), this, SLOT(moduleListTextSelected(int) ) );

}

/** Sets the modules used by the search. */
void CSearchOptionsArea::setModules( QList<CSwordModuleInfo*> modules )
{
	qDebug("CSearchOptionsArea::setModules");
	qDebug() << modules;
	QString t;

	m_modules.clear(); //remove old modules
	QList<CSwordModuleInfo*>::iterator end_it = modules.end();

	for (QList<CSwordModuleInfo*>::iterator it(modules.begin()); it != end_it; ++it) {
		//ToDo:  Check for containsRef compat
		if (*it == 0) { //don't operate on null modules.
			continue;
		}
		qDebug() << "new module:" << (*it)->name();
		if ( !m_modules.contains(*it) ) {
			m_modules.append( *it );
			t.append( (*it)->name() );
			if (*it != modules.last()) {
				t += QString::fromLatin1(", "); // so that it will become a readable list (WLC, LXX, GerLut...)
			}
		}
	};
	//m_modulesLabel->setText(t);
	int existingIndex = m_modulesCombo->findText(t);
	qDebug() << "index of the module list string which already exists in combobox:" << existingIndex;
	if (existingIndex >= 0) {
		m_modulesCombo->removeItem(existingIndex);
	}
	if (m_modulesCombo->count() > 10) {
		m_modulesCombo->removeItem(m_modulesCombo->count()-1);
	}
	m_modulesCombo->insertItem(0, t);
	m_modulesCombo->setItemData(0, t, Qt::ToolTipRole);
	m_modulesCombo->setCurrentIndex(0);
	m_modulesCombo->setToolTip(t);
	//Save the list in config here, not when deleting, because the history may be used
	// elsewhere while the dialog is still open
	QStringList historyList;
	for (int i = 0; i < m_modulesCombo->count(); ++i) {
		historyList.append(m_modulesCombo->itemText(i));
	}
	CBTConfig::set(CBTConfig::searchModulesHistory, historyList);
	emit( sigSetSearchButtonStatus( (modules.count() != 0) ) );
}

// Catch activated signal of module selector combobox
void CSearchOptionsArea::moduleListTextSelected(int index)
{
	qDebug("CSearchOptionsArea::moduleListTextSelected");
	//create the module list
	QString text = m_modulesCombo->itemText(index);
	qDebug() << text;
	QStringList moduleNamesList = text.split(", ");
	QList<CSwordModuleInfo*> moduleList;
	foreach(QString name, moduleNamesList) {
		moduleList.append(CPointers::backend()->findModuleByName(name));
	}
	//set the list and the combobox list and text
	setModules(moduleList);
}

void CSearchOptionsArea::chooseModules() {
	QString title(tr("Works to Search in"));
	QString label(tr("Select the works which should be searched."));
	CSearchModuleChooserDialog* dlg = new CSearchModuleChooserDialog(this, title, label, modules());
	connect(dlg, SIGNAL(modulesChanged(QList<CSwordModuleInfo*>, QTreeWidget*)), this, SLOT(setModules(QList<CSwordModuleInfo*>)));
	dlg->exec();
}

const QList<CSwordModuleInfo*> CSearchOptionsArea::modules() {
	return m_modules;
}

void CSearchOptionsArea::reset() {
	m_rangeChooserCombo->setCurrentIndex(0);
	m_searchTextCombo->clearEditText();
}

void CSearchOptionsArea::saveSettings() {
	CBTConfig::set(CBTConfig::searchTexts, m_searchTextCombo->historyItems());
}

void CSearchOptionsArea::readSettings() {
	QStringList texts = CBTConfig::get(CBTConfig::searchTexts);
	for (int i=0; i<texts.size(); i++)
	{
		if (texts.at(i).size() > 0)
			m_searchTextCombo->addItem(texts.at(i));
	}
	m_modulesCombo->insertItems(0, CBTConfig::get(CBTConfig::searchModulesHistory));
	for (int i = 0; i < m_modulesCombo->count(); ++i) {
		m_modulesCombo->setItemData(i, m_modulesCombo->itemText(i), Qt::ToolTipRole);
	}
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
	//the first option is fixed, the others can be edited using the "Setup ranges" button.
	m_rangeChooserCombo->clear();
	m_rangeChooserCombo->insertItem(0, QString("[") + tr("No search scope") + QString("]"));
	//TODO: what about this?
	//m_rangeChooserCombo->insertItem(tr("Last search result"));

	//insert the user-defined ranges
	m_rangeChooserCombo->insertItems(1, CBTConfig::get(CBTConfig::searchScopes).keys());

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

void CSearchOptionsArea::addToHistory(const QString& text)
{
	m_searchTextCombo->addToHistory(text);
}

void CSearchOptionsArea::slotSearchTextEditReturnPressed()
{
	qDebug("CSearchOptionsArea::slotSearchTextEditReturnPressed");
	m_searchTextCombo->addToHistory( m_searchTextCombo->currentText() );
	emit sigStartSearch();
}

bool CSearchOptionsArea::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
		qDebug() << "CSearchOptionsArea::eventFilter" << obj << event;
		if (obj == m_searchTextCombo->view() || obj == m_searchTextCombo || obj == m_searchTextCombo->lineEdit()) {
			//qDebug() << "CSearchOptionsArea::eventFilter" << obj << event;
			obj->event(event);
			// don't handle this event in parent
			event->accept();
			return true;
		}
	}
	return QWidget::eventFilter(obj, event);
}

} //end of namespace Search
