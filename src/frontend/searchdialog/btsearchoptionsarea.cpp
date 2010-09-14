/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btsearchoptionsarea.h"
#include "csearchmodulechooserdialog.h"
#include "crangechooserdialog.h"

#include "util/directoryutil.h"
#include "util/cresmgr.h"
#include "util/ctoolclass.h"
#include "util/cpointers.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/config/cbtconfig.h"
#include "frontend/htmldialogs/bttabhtmldialog.h"

#include <QHBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QDebug>
#include <QLineEdit>
#include <QMessageBox>
#include <QEvent>


namespace Search {

BtSearchOptionsArea::BtSearchOptionsArea(QWidget *parent )
	: QWidget(parent)
{
	initView();
	initConnections();
	readSettings();
}

BtSearchOptionsArea::~BtSearchOptionsArea() {
	saveSettings();
}

QString BtSearchOptionsArea::searchText() const {
	return m_searchTextCombo->currentText();
}

BtSearchOptionsArea::SearchType BtSearchOptionsArea::searchType()
{
	if (m_typeAndButton->isChecked()) {
		return BtSearchOptionsArea::AndType;
	}
	if (m_typeOrButton->isChecked()) {
		return BtSearchOptionsArea::OrType;
	}
	return BtSearchOptionsArea::FullType;
}

QPushButton* BtSearchOptionsArea::searchButton() const {
	return m_searchButton;
}

void BtSearchOptionsArea::setSearchText(const QString& text) {
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

void BtSearchOptionsArea::initView()
{
	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	this->setSizePolicy(sizePolicy);
	hboxLayout = new QHBoxLayout(this);

	searchGroupBox = new QGroupBox(this);

	gridLayout = new QGridLayout(searchGroupBox);

	gridLayout->setHorizontalSpacing(3);

	// ******** label for search text editor***********
    m_searchTextLabel = new QLabel(tr("Search for:"), searchGroupBox);
	m_searchTextLabel->setWordWrap(false);
	gridLayout->addWidget(m_searchTextLabel, 0, 0);

	// **********Buttons******************

	m_searchButton = new QPushButton(this);
	m_searchButton->setText(tr("&Search"));
	m_searchButton->setIcon( util::filesystem::DirectoryUtil::getIcon(CResMgr::searchdialog::icon));
	m_searchButton->setToolTip(tr("Start to search the text in the chosen works"));
	gridLayout->addWidget(m_searchButton, 0, 2);

	m_chooseModulesButton = new QPushButton(tr("Ch&oose..."), searchGroupBox);
	m_chooseModulesButton->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::searchdialog::chooseworks_icon));
	m_chooseModulesButton->setToolTip( tr("Choose works for the search"));
	gridLayout->addWidget(m_chooseModulesButton, 2, 2);

	m_chooseRangeButton = new QPushButton(tr("S&etup..."), searchGroupBox);
	m_chooseRangeButton->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::searchdialog::setupscope_icon));
	m_chooseRangeButton->setToolTip(tr("Configure predefined scopes for search"));
	gridLayout->addWidget(m_chooseRangeButton, 3, 2);

	// ************* Search type (AND/OR) selector ***************************************
	QHBoxLayout* typeSelectorLayout = new QHBoxLayout();
	int tsLeft, tsTop, tsRight, tsBottom;
	// Added space looks nicer and enhances readability
	typeSelectorLayout->getContentsMargins(&tsLeft, &tsTop, &tsRight, &tsBottom);
	typeSelectorLayout->setContentsMargins(tsLeft, 0, tsRight, tsBottom + CToolClass::mWidth(this,1) );
	typeSelectorLayout->setSpacing(typeSelectorLayout->spacing()+CToolClass::mWidth(this,1));
	QHBoxLayout* fullButtonLayout = new QHBoxLayout();
	fullButtonLayout->setSpacing(CToolClass::mWidth(this,1)/2);
	m_typeAndButton = new QRadioButton(tr("All words"));
	m_typeAndButton->setChecked(true);
	m_typeOrButton = new QRadioButton(tr("Some words"));
	m_typeFreeButton = new QRadioButton(tr("Free"));

	m_typeAndButton->setToolTip(tr("All of the words (AND is added between the words)"));
	m_typeOrButton->setToolTip(tr("Some of the words (OR is added between the words)"));
	m_typeFreeButton->setToolTip(tr("Full lucene syntax"));

	m_helpLabel = new QLabel(tr(" (<a href='syntax_help'>full syntax</a>)"));
	m_helpLabel->setToolTip(tr("Click the link to get help for search syntax"));

	typeSelectorLayout->addWidget(m_typeAndButton);
	typeSelectorLayout->addWidget(m_typeOrButton);
	fullButtonLayout->addWidget(m_typeFreeButton);
	fullButtonLayout->addWidget(m_helpLabel);
	typeSelectorLayout->addLayout(fullButtonLayout);
	gridLayout->addLayout(typeSelectorLayout, 1,1, 1,-1, Qt::AlignLeft|Qt::AlignTop);

	// ************* Label for search range/scope selector *************
	m_searchScopeLabel = new QLabel(tr("Scope:"), searchGroupBox);
	m_searchScopeLabel->setWordWrap(false);
	gridLayout->addWidget(m_searchScopeLabel, 3, 0);

	// ***********Range/scope selector combo box***********
	m_rangeChooserCombo = new QComboBox(searchGroupBox);
	QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
	sizePolicy2.setHorizontalStretch(0);
	sizePolicy2.setVerticalStretch(0);
	sizePolicy2.setHeightForWidth(m_rangeChooserCombo->sizePolicy().hasHeightForWidth());
	m_rangeChooserCombo->setSizePolicy(sizePolicy2);
	m_rangeChooserCombo->setToolTip(tr("Choose the scope (books/chapters/verses to search in).<br />Applicable for Bibles and commentaries."));
	gridLayout->addWidget(m_rangeChooserCombo, 3, 1);

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
	gridLayout->addWidget(m_modulesLabel, 2, 0);

	m_modulesCombo = new QComboBox(searchGroupBox);
	m_modulesCombo->setDuplicatesEnabled(false);
	gridLayout->addWidget(m_modulesCombo, 2, 1);

	hboxLayout->addWidget(searchGroupBox);

	// Set the minimum size before the widgets are populated with data.
	// Otherwise we will get problems with sizing.
	setMinimumSize(minimumSizeHint());

	refreshRanges();
	//set the initial focus
	m_searchTextCombo->setFocus();
	// event filter to prevent the Return/Enter presses in the combo box doing something
	// in the parent widget
	m_searchTextCombo->installEventFilter(this);
}

void BtSearchOptionsArea::initConnections()
{
	QObject::connect( m_searchTextCombo->lineEdit(), SIGNAL(returnPressed ()),
				this, SLOT( slotSearchTextEditReturnPressed() )
			);
	connect(m_chooseModulesButton, SIGNAL(clicked()), this, SLOT(chooseModules()));
	connect(m_chooseRangeButton, SIGNAL(clicked()), this, SLOT(setupRanges()));
	connect(m_modulesCombo, SIGNAL(activated(int)), this, SLOT(moduleListTextSelected(int) ) );
	connect(m_helpLabel, SIGNAL(linkActivated(QString)), this, SLOT(syntaxHelp()));
	connect(m_searchTextCombo, SIGNAL(editTextChanged(const QString&)), this, SLOT(slotValidateText(const QString&)));
}

/** Sets the modules used by the search. */
void BtSearchOptionsArea::setModules( QList<CSwordModuleInfo*> modules )
{
	qDebug("BtSearchOptionsArea::setModules");
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
void BtSearchOptionsArea::moduleListTextSelected(int index)
{
	qDebug("BtSearchOptionsArea::moduleListTextSelected");
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

void BtSearchOptionsArea::chooseModules() {
	QString title(tr("Works to Search in"));
	QString label(tr("Select the works which should be searched."));
	CSearchModuleChooserDialog* dlg = new CSearchModuleChooserDialog(this, title, label, modules());
	connect(dlg, SIGNAL(modulesChanged(QList<CSwordModuleInfo*>, QTreeWidget*)), this, SLOT(setModules(QList<CSwordModuleInfo*>)));
	dlg->exec();
}

QList<CSwordModuleInfo*> BtSearchOptionsArea::modules() const {
	return m_modules;
}

void BtSearchOptionsArea::reset() {
	m_rangeChooserCombo->setCurrentIndex(0);
	m_searchTextCombo->clearEditText();
}

void BtSearchOptionsArea::saveSettings() {
	CBTConfig::set(CBTConfig::searchTexts, m_searchTextCombo->historyItems());
	SearchType t = FullType;
	if (m_typeAndButton->isChecked()) {
		t = AndType;
	}
	if (m_typeOrButton->isChecked()) {
		t = OrType;
	}
	CBTConfig::set(CBTConfig::searchType, t);
}

void BtSearchOptionsArea::readSettings() {
	QStringList texts = CBTConfig::get(CBTConfig::searchTexts);
	//for some reason the slot was called when setting the upmost item
	disconnect(m_searchTextCombo, SIGNAL(editTextChanged(const QString&)), this, SLOT(slotValidateText(const QString&)));
	for (int i=0; i<texts.size(); i++)
	{
		if (texts.at(i).size() > 0)
			m_searchTextCombo->addItem(texts.at(i));
	}
	connect(m_searchTextCombo, SIGNAL(editTextChanged(const QString&)), this, SLOT(slotValidateText(const QString&)));

	m_modulesCombo->insertItems(0, CBTConfig::get(CBTConfig::searchModulesHistory));
	for (int i = 0; i < m_modulesCombo->count(); ++i) {
		m_modulesCombo->setItemData(i, m_modulesCombo->itemText(i), Qt::ToolTipRole);
	}

	int stype = CBTConfig::get(CBTConfig::searchType);
	switch (stype) {
		case AndType: m_typeAndButton->setChecked(true);
		break;
		case OrType: m_typeOrButton->setChecked(true);
		break;
		default: m_typeFreeButton->setChecked(true);
	}
}

void BtSearchOptionsArea::aboutToShow() {
	m_searchTextCombo->setFocus();
}

void BtSearchOptionsArea::setupRanges() {
	CRangeChooserDialog* chooser = new CRangeChooserDialog(this);
	chooser->exec();

	refreshRanges();
}

void BtSearchOptionsArea::syntaxHelp() {

	QString style = QString(
	"<style type='text/css'>"
	"h1 a {font-size: medium}"
	"table {margin-left: 20px}"
	"td {"
	"	border-width: 3px 3px 3px 3px;"
	"	border-style: solid solid solid solid;"
	"	border-color: white white white white;"
	"	background-color: #faf0e6;"
	"}"
	"p#links {margin-left: 20px}"
	"</style>");
	
	//: Don't translate words inside <> tags!
	//: Translate 'All words' etc. indentically to the Search dialog options.
	QString intro = tr(
	"<p>"
	"This help is mainly for 'Full syntax' option. 'All words' and 'Some words' options " "have more limited syntax; <a href='#wildcards'>wildcards</a> and <a " "href='#fields'>text fields</a> are supported for them. Some other syntax features " "may give strange or wrong results with All words/Some words."
	"</p>");
	
	QString links = tr(
	"<p id='links'>"
	"<A href='#allsome'>Which words to find</A><br />"
	"<A href='#grouping'>Grouping and order</A><br />"
	"<A href='#wildcards'>Wildcards (partial words)</A><br />"
	"<A href='#fields'>Text fields (different parts of text)</A><br/>"
	"<A href='#lucene'>Other syntax features</A><br/>"
	"</p>");
	
	//: Syntax words (AND, OR...) must not be translated.
	QString whichwords = tr(
	"<h1><A name='allsome'>Which words to find</A></h1>"
	"<p>Search terms are separated by spaces. <strong>AND</strong> (all words), " "<strong>OR</strong> (some words) and <strong>NOT</strong> (not the following word) " "can be added between the words. If none is added explicitly OR is used " "automatically. '<strong>+</strong>word' means the word must be in the results, " "'<strong>-</strong>word' means it must not be in the results.</p>");
	
	//: In examples words to be searched for may be translated, but syntax words (AND, OR...) must not be translated.
	QString whichwordstable = tr(
	"<p><table><tr>"
	"<td>jesus AND god</td><td>Finds verses with both 'Jesus' and 'God'</td>"
	"</tr><tr>"
	"<td>jesus OR god</td><td>Finds verses with 'Jesus' or 'God' or both</td>"
	"</tr><tr>"
	"<td>jesus NOT god</td><td>Finds verses with 'Jesus' but with no 'God'</td>"
	"</tr><tr>"
	"<td>+jesus -god</td><td>Finds verses with 'Jesus' but with no 'God'</td>"
	"</tr></table></p>");
	
	QString grouping = tr(
	"<h1><A name='grouping'>Grouping and order</A></h1>"
	"<p>Words can be grouped with <strong>parenthesis</strong>. "
	"Strict word order can be defined with <strong>quotes</strong>.</p>");
	
	QString groupingtable = tr(
	"<p><table><tr>"
	"<td>(a AND b) OR c</td><td>Finds verses with both 'a' AND 'b', and verses with 'c'</td>"
	"</tr><tr>"
	"<td>\"says lord\"</td><td>Finds e.g. '...Isaiah says, \"Lord...' but not '...says the LORD'</td>"
	"</tr><tr>"
	"<td>\"says the lord\"</td><td>Finds all verses with 'says the LORD'</td>"
	"</tr></table></p>");
	
	QString wildcards = tr(
	"<h1><A name='wildcards'>Wildcards (partial words)</A></h1>"
	"<p>'<strong>*</strong>' matches any sequence of 0 or more characters, while '<strong>?</strong>' matches any single character. A wildcard can not be used in the beginning of a word.</p>");
	
	QString wildcardstable = tr(
	"<p><table><tr>"
	"<td>a*</td><td>All words beginning with 'a'</td>"
	"</tr><tr>"
	"<td>a*a</td><td>'Assyria', 'aroma', 'abba' etc.</td>"
	"</tr><tr>"
	"<td>a?</td><td>'at' and 'an'</td>"
	"</tr><tr>"
	"<td>a??a</td><td>'abba', 'area', 'Asia' etc.</td>"
	"</tr></table></p>");
	
	QString fields = tr(
	"<h1><A name='fields'>Text fields (different parts of text)</A></h1>"
	"<p>Available text fields:<br /><table>"
	"<tr><td>heading:</td><td>Searches headings</td></tr>"
	"<tr><td>footnote:</td><td>Searches footnotes</td></tr>"
	"<tr><td>strong:</td><td>Searches Strong's numbers</td></tr>"
	"<tr><td>morph:</td><td>Searches morphology codes</td></tr></table></p>");
	
	QString fieldstable = tr(
	"<p>Examples:<br /><table>"
	"<tr><td>heading:Jesus</td><td>Finds headings with 'Jesus'</td></tr>"
	"<tr><td>footnote:Jesus AND footnote:said</td><td>Finds footnotes with 'Jesus' and 'said'</td></tr>"
	"<tr><td>strong:G846</td><td>Finds verses with Strong's Greek number 846</td></tr>"
	"<tr><td>morph:\"N-NSF\"</td><td>Finds verses with morphology code 'N-NSF'</td>"
	"</tr></table></p>");
	
	QString lucene = tr(
	"<h1><A name='lucene'>Other syntax features</A></h1>"
	"<p>BibleTime uses the CLucene search engine. You can read more on the <a href='http://lucene.apache.org/java/1_4_3/queryparsersyntax.html'>lucene syntax web page</a> (in external browser).</p>");

	QString syntax = style + intro + links + whichwords + whichwordstable +
		grouping + groupingtable + wildcards + wildcardstable +
		fields + fieldstable + lucene;

	BtTabHtmlDialog* dlg = new BtTabHtmlDialog(tr("Search Syntax Help"), 0, this);
	dlg->setHtml(syntax);
	dlg->show();
}

void BtSearchOptionsArea::refreshRanges() {
	//the first option is fixed, the others can be edited using the "Setup ranges" button.
	m_rangeChooserCombo->clear();
	m_rangeChooserCombo->insertItem(0, QString("[") + tr("No search scope") + QString("]"));
	//TODO: what about this?
	//m_rangeChooserCombo->insertItem(tr("Last search result"));

	//insert the user-defined ranges
	m_rangeChooserCombo->insertItems(1, CBTConfig::get(CBTConfig::searchScopes).keys());

}

sword::ListKey BtSearchOptionsArea::searchScope() {
	if (m_rangeChooserCombo->currentIndex() > 0) { //is not "no scope"
		CBTConfig::StringMap map = CBTConfig::get(CBTConfig::searchScopes);
		QString scope = map[ m_rangeChooserCombo->currentText() ];
		if (!scope.isEmpty()) {
			return sword::VerseKey().ParseVerseList( (const char*)scope.toUtf8(), "Genesis 1:1", true);
		}
	}
	return sword::ListKey();
}

bool BtSearchOptionsArea::hasSearchScope() {
	return (searchScope().Count() > 0);
}

void BtSearchOptionsArea::addToHistory(const QString& text)
{
	m_searchTextCombo->addToHistory(text);
}

void BtSearchOptionsArea::slotSearchTextEditReturnPressed()
{
	qDebug("BtSearchOptionsArea::slotSearchTextEditReturnPressed");
	m_searchTextCombo->addToHistory( m_searchTextCombo->currentText() );
	emit sigStartSearch();
}

bool BtSearchOptionsArea::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
		//qDebug() << "BtSearchOptionsArea::eventFilter" << obj << event;
		if (obj == m_searchTextCombo->view() || obj == m_searchTextCombo || obj == m_searchTextCombo->lineEdit()) {
			//qDebug() << "BtSearchOptionsArea::eventFilter" << obj << event;
			obj->event(event);
			// don't handle this event in parent
			event->accept();
			return true;
		}
	}
	return QWidget::eventFilter(obj, event);
}

void BtSearchOptionsArea::slotValidateText(const QString& /*newText*/)
{
// 	static const QRegExp re("\\b(AND|OR)\\b");
// 	qDebug() << "new text:" << newText;
// 	qDebug() << "contains:" << (newText.contains(re));
// 	if (newText.isEmpty() || !newText.contains(re) ) {
// 		qDebug()<< "no AND/OR";
// 		if (!m_typeAndButton->isEnabled()) {
// 			m_typeOrButton->setEnabled(true);
// 			m_typeAndButton->setEnabled(true);
// 			m_typeAndButton->setToolTip(tr("All of the words (AND is added between the words)"));
// 			m_typeOrButton->setToolTip(tr("Some of the words"));
// 		}
// 	}
// 	else {
// 		qDebug("AND/OR!");
// 		if (m_typeAndButton->isEnabled()) {
// 			m_typeOrButton->setChecked(true);
// 			m_typeOrButton->setEnabled(false);
// 			m_typeAndButton->setEnabled(false);
// 			m_typeAndButton->setToolTip(tr("Full syntax is used because text includes AND or OR"));
// 			m_typeOrButton->setToolTip(tr("Full syntax is used because text includes AND or OR"));
// 		}
// 	}
}

//bool BtSearchOptionsArea::isAndSearchType()
//{
//
//}


}
