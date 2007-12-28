/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "csearchdialog.h"
#include "csearchdialogareas.h"
//#include "csearchanalysis.h"
#include "backend/cswordmodulesearch.h"
#include "backend/keys/cswordkey.h"
#include "backend/keys/cswordversekey.h"
#include "frontend/cbtconfig.h"
#include "frontend/cmoduleindexdialog.h"

#include "util/cresmgr.h"
#include "util/ctoolclass.h"
#include "util/directoryutil.h"

//Qt includes
#include <QString>
#include <QWidget>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLineEdit>

//KDE includes
#include <kdialog.h>

#include <khistorycombobox.h>
#include <kpushbutton.h>

namespace Search {

static CSearchDialog* m_staticDialog = 0;

void CSearchDialog::openDialog(const ListCSwordModuleInfo modules, const QString& searchText, QWidget* parentDialog)
{
	if (!m_staticDialog) {
		m_staticDialog = new CSearchDialog(parentDialog);
	};
	m_staticDialog->reset();

	if (modules.count()) {
		m_staticDialog->setModules(modules);
	}
	else {
		m_staticDialog->showModulesSelector();
	}

	m_staticDialog->setSearchText(searchText);
	if (m_staticDialog->isHidden()) {
		m_staticDialog->show();
	}

	if (modules.count() && !searchText.isEmpty()) {
		m_staticDialog->startSearch();
	}
	// moved these to after the startSearch() because
	// the progress dialog caused them to loose focus.
	m_staticDialog->raise();
	m_staticDialog->activateWindow();
};

CSearchDialog* const CSearchDialog::getSearchDialog()
{
	Q_ASSERT(m_staticDialog);
	return m_staticDialog;
};

CSearchDialog::CSearchDialog(QWidget *parent)
//: KDialogBase(Plain, tr("Search dialog"), Close | User1, User1, parent, "CSearchDialog", false, true, tr("Search"))
	:KDialog(parent)
{
	setButtons(KDialog::Close|KDialog::User1);
	setButtonText(KDialog::User1, tr("&Search"));
	//setWFlags( windowFlags() | Qt::WStyle_MinMax );
	setWindowIcon( util::filesystem::DirectoryUtil::getIcon(CResMgr::searchdialog::icon) );

	m_searcher.connectFinished( this, SLOT(searchFinished()));

	initView();
	initConnections();
}

CSearchDialog::~CSearchDialog()
{
	//TODO: port to QSettings
// 	KConfigGroup cg = CBTConfig::getConfig()->group("CSearchDialog"); 
// 	saveDialogSize();
}

/** Starts the search with the set modules and the set search text. */
void CSearchDialog::startSearch()
{
	QString searchText(m_searchOptionsArea->searchText());

	if (searchText.isEmpty()) {
		return;
	}

	// check that we have the indices we need for searching
	if (!m_searcher.modulesHaveIndices( modules() ) )	{
		int result = QMessageBox::question(this, tr("Missing indices"),
			tr("One or more modules need indexing before they can be searched.\n"
			"This could take a long time. Proceed with indexing?"),
		QMessageBox::Yes | QMessageBox::Default,
		QMessageBox::No  | QMessageBox::Escape);
		// In SuSE 10.0 the result is the logical or of the button type, just like it is
		// inputed into the QMessageBox.
		if ( (result == (QMessageBox::Yes | QMessageBox::Default)) ||
		     (result == QMessageBox::Yes) || (result == QMessageBox::Default) ) {
			CModuleIndexDialog* dlg = CModuleIndexDialog::getInstance();
			dlg->indexUnindexedModules( modules() );
		}
		else {
			return;
		}
	}

	//m_searchResultArea->reset();

//	const int searchFlags = m_searchOptionsArea->searchFlags();

//	const CSwordModuleSearch::scopeType scopeType = m_searchOptionsArea->scopeType();
// 	if (scopeType == CSwordModuleSearch::Scope_LastSearch) {
// 		searchFlags |= CSwordModuleSearch::useLastResult;
// 	}
// 	else if ( (scopeType == CSwordModuleSearch::Scope_Bounds)
// 			  && strlen(m_searchOptionsArea->searchScope().getRangeText())) {
// 		//we need the scope flag and a valid scope!
// 		searchFlags |= CSwordModuleSearch::useScope;
// 		m_searcher.setSearchScope( m_searchOptionsArea->searchScope() );
// 	}

	if (m_searchOptionsArea->hasSearchScope()) {
		m_searcher.setSearchScope( m_searchOptionsArea->searchScope() );
	}
	else {
		m_searcher.resetSearchScope();
	}

	m_searcher.setModules( modules() );
	m_searcher.setSearchedText(searchText);
//	m_searcher.setSearchOptions(searchFlags);

	m_searcher.startSearch();
}

/** Starts the search with the given module list and given search text. */
void CSearchDialog::startSearch( const ListCSwordModuleInfo modules, const QString& searchText)
{
	m_searchResultArea->reset();
	m_searchOptionsArea->reset();
	setModules(modules);
	setSearchText(searchText);

	startSearch();
}

/** Returns the list of used modules. */
const ListCSwordModuleInfo CSearchDialog::modules()
{
	return m_searchOptionsArea->modules();
}

/** Sets the list of modules for the search. */
void CSearchDialog::setModules( const ListCSwordModuleInfo modules )
{
	m_searchOptionsArea->setModules(modules);
	resize( sizeHint() );
}

/** Returns the search text which is set currently. */
const QString CSearchDialog::searchText()
{
	return m_searchOptionsArea->searchText();
}

sword::ListKey CSearchDialog::searchScope()
{
	return m_searchOptionsArea->searchScope();
}

/** Returns the search text which is used for the search. */
void CSearchDialog::setSearchText( const QString searchText )
{
	m_searchOptionsArea->setSearchText(searchText);
}

/** Initializes this object. */
void CSearchDialog::initView()
{
	setMainWidget(new QWidget(this));
	setButtonToolTip(User1, CResMgr::searchdialog::searchButton::tooltip);
	QVBoxLayout *box = new QVBoxLayout( mainWidget());
	mainWidget()->setLayout(box);

	m_searchOptionsArea = new CSearchOptionsArea(mainWidget());
	box->addWidget( m_searchOptionsArea );

	m_searchResultArea = new CSearchResultArea(mainWidget());
	box->addWidget( m_searchResultArea );

	// The dialog doesn't resize properly if the minimum size of the
	// plain page is lower than the minimumsize of our two widgets.
	// You can resize the dialog, but it just starts covering up the
	// button bar and the two widgets instead of stopping at the
	// minimum size.  The following code sets the minimum with some
	// margin.  If you know of a better way to do this, do it!
	int w = m_searchOptionsArea->minimumWidth();
	int h = m_searchOptionsArea->minimumHeight() +
		m_searchResultArea->minimumHeight();
	mainWidget()->setMinimumSize(w+10, h+100);
	// Added code for loading last size of dialog
	//setInitialSize(configDialogSize("CSearchDialog"));
	//TODO: port to QSettings
// 	restoreDialogSize( CBTConfig::getConfig()->group("CSearchDialog") );
}

void CSearchDialog::searchFinished() {
	if ( m_searcher.foundItems() ) {
		m_searchResultArea->setSearchResult(modules());
	}
	else {
		m_searchResultArea->reset();
	}
	m_staticDialog->raise();
	m_staticDialog->activateWindow();
}

void CSearchDialog::showModulesSelector() {
	m_searchOptionsArea->chooseModules();
}

/** Initializes the signal slot connections */
void CSearchDialog::initConnections() {
	connect(this, SIGNAL(user1Clicked()), SLOT(startSearch()));
	connect(this, SIGNAL(closeClicked()), SLOT(slotClose()));
	// Pressing return in search term editor starts the search:
	QObject::connect(m_searchOptionsArea->m_searchTextCombo->lineEdit(), SIGNAL(returnPressed()), this->button(KDialog::User1), SLOT(setFocus()) );
}

/** Resets the parts to the default. */
void CSearchDialog::reset() {
	m_searchOptionsArea->reset();
	m_searchResultArea->reset();
}

/** Reimplementation. */
void CSearchDialog::slotClose() {
	//TODO: what to do with this when we move to QDialog?
	delayedDestruct();
	m_staticDialog = 0;
}

} //end of namespace Search
