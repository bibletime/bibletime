/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/searchdialog/csearchdialog.h"

#include <QDebug>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QSizePolicy>
#include <QString>
#include <QRegExp>
#include <QVBoxLayout>
#include <QWidget>

#include "backend/config/cbtconfig.h"
#include "backend/cswordmodulesearch.h"
#include "backend/keys/cswordkey.h"
#include "backend/keys/cswordversekey.h"
#include "frontend/cmoduleindexdialog.h"
#include "frontend/searchdialog/btsearchoptionsarea.h"
#include "frontend/searchdialog/btsearchresultarea.h"
#include "util/cresmgr.h"
#include "util/directory.h"
#include "util/dialogutil.h"


namespace Search {

static CSearchDialog* m_staticDialog = 0;

void CSearchDialog::openDialog(const QList<CSwordModuleInfo*> modules, const QString& searchText, QWidget* parentDialog) {
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
}

void CSearchDialog::closeDialog() {
    if (m_staticDialog != 0)
        m_staticDialog->closeButtonClicked();
}

CSearchDialog* CSearchDialog::getSearchDialog() {
    Q_ASSERT(m_staticDialog);
    return m_staticDialog;
}

CSearchDialog::CSearchDialog(QWidget *parent)
        : QDialog(parent), /*m_searchButton(0),*/ m_closeButton(0),
        m_searchResultArea(0), m_searchOptionsArea(0) {
    namespace DU = util::directory;

    setWindowIcon(DU::getIcon(CResMgr::searchdialog::icon));
    setWindowTitle(tr("Search"));
    setAttribute(Qt::WA_DeleteOnClose);
    m_searcher.connectFinished( this, SLOT(searchFinished()));
    initView();
    initConnections();
}

CSearchDialog::~CSearchDialog() {
    saveDialogSettings();
    m_staticDialog = 0;
}

/** Starts the search with the set modules and the set search text. */
void CSearchDialog::startSearch() {
    QString originalSearchText(m_searchOptionsArea->searchText());
    QString searchText("");

    // first check the search string for errors
    {
        QString TestString(originalSearchText);
        QRegExp ReservedWords("heading:|footnote:|morph:|strong:");
        if (TestString.replace(ReservedWords, "").simplified().isEmpty()) {
            return;
        }
    }

    searchText = prepareSearchText(originalSearchText);

    // Insert search text into history list of combobox
    m_searchOptionsArea->addToHistory(originalSearchText);

    // check that we have the indices we need for searching
    if (!m_searcher.modulesHaveIndices( modules() ) )	{
        int result = util::showQuestion(this, tr("Missing indices"),
                                        tr("One or more works need indexing before they can be searched.\n"
                                           "This could take a long time. Proceed with indexing?"),
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
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

    if (m_searchOptionsArea->hasSearchScope()) {
        m_searcher.setSearchScope( m_searchOptionsArea->searchScope() );
    }
    else {
        m_searcher.resetSearchScope();
    }

    m_searcher.setModules( modules() );
    m_searcher.setSearchedText(searchText);


    //Just to be sure that it can't be clicked again, if the search happens to be a bit slow.
    m_searchOptionsArea->searchButton()->setEnabled(false);
    m_searchOptionsArea->m_searchTextCombo->setEnabled(false);

    m_searcher.startSearch();

    m_searchOptionsArea->searchButton()->setEnabled(true);
    m_searchOptionsArea->m_searchTextCombo->setEnabled(true);
    m_searchOptionsArea->m_searchTextCombo->setFocus();
}

QString CSearchDialog::prepareSearchText(const QString& orig) {
    qDebug() << "Original search text:" << orig;
    static const QRegExp syntaxCharacters("[+\\-()!\"~]");
    static const QRegExp andWords("\\band\\b", Qt::CaseInsensitive);
    static const QRegExp orWords("\\bor\\b", Qt::CaseInsensitive);
    QString text("");
    if (m_searchOptionsArea->searchType() == BtSearchOptionsArea::AndType) {
        qDebug() << "AND type";
        text = orig.simplified();
        text.remove(syntaxCharacters);
        qDebug() << "After syntax characters removed:" << text;
        text.replace(andWords, "\"and\"");
        text.replace(orWords, "\"or\"");
        qDebug() << "After andor repclaced:" << text;
        text.replace(" ", " AND ");
    }
    if (m_searchOptionsArea->searchType() == BtSearchOptionsArea::OrType) {
        text = orig.simplified();
        text.remove(syntaxCharacters);
        text.replace(andWords, "\"and\"");
        text.replace(orWords, "\"or\"");
    }
    if (m_searchOptionsArea->searchType() == BtSearchOptionsArea::FullType) {
        text = orig;
    }
    qDebug() << "The final search string:" << text;
    return text;
}

/** Starts the search with the given module list and given search text. */
void CSearchDialog::startSearch( const QList<CSwordModuleInfo*> modules, const QString& searchText) {
    m_searchResultArea->reset();
    m_searchOptionsArea->reset();
    setModules(modules);
    setSearchText(searchText);

    startSearch();
}

/** Returns the list of used modules. */
QList<CSwordModuleInfo*> CSearchDialog::modules() const {
    return m_searchOptionsArea->modules();
}

/** Sets the list of modules for the search. */
void CSearchDialog::setModules( const QList<CSwordModuleInfo*> modules ) {
    m_searchOptionsArea->setModules(modules);
}

/** Returns the search text which is set currently. */
QString CSearchDialog::searchText() const {
    return m_searchOptionsArea->searchText();
}

sword::ListKey CSearchDialog::searchScope() {
    return m_searchOptionsArea->searchScope();
}

/** Sets the search text which is used for the search. */
void CSearchDialog::setSearchText( const QString &searchText ) {
    m_searchOptionsArea->setSearchText(searchText);
}

/** Initializes this object. */
void CSearchDialog::initView() {
    namespace DU = util::directory;

    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    setLayout(verticalLayout);

    m_searchOptionsArea = new BtSearchOptionsArea(this);
    verticalLayout->addWidget(m_searchOptionsArea);

    m_searchResultArea = new BtSearchResultArea(this);
    verticalLayout->addWidget(m_searchResultArea);

    QHBoxLayout* horizontalLayout = new QHBoxLayout();

    m_analyseButton = new QPushButton(tr("&Analyze results..."), 0);
    m_analyseButton->setToolTip(tr("Show a graphical analysis of the search result"));
    QSpacerItem* spacerItem = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addWidget(m_analyseButton);
    horizontalLayout->addItem(spacerItem);

    m_closeButton = new QPushButton(this);
    m_closeButton->setText(tr("&Close"));
    m_closeButton->setIcon(DU::getIcon(CResMgr::searchdialog::close_icon));
    horizontalLayout->addWidget(m_closeButton);

    verticalLayout->addLayout(horizontalLayout);

    loadDialogSettings();
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
    // Search button is clicked
    bool ok = connect(m_searchOptionsArea->searchButton(), SIGNAL(clicked()), this, SLOT(startSearch()));
    Q_ASSERT(ok);
    // Return/Enter is pressed in the search text field
    ok = connect(m_searchOptionsArea, SIGNAL(sigStartSearch()), this, SLOT(startSearch()) );
    Q_ASSERT(ok);
    ok = connect(m_closeButton, SIGNAL(clicked()), this, SLOT(closeButtonClicked()));
    Q_ASSERT(ok);

    connect(m_analyseButton, SIGNAL(clicked()), m_searchResultArea, SLOT(showAnalysis()));

}

/** Resets the parts to the default. */
void CSearchDialog::reset() {
    m_searchOptionsArea->reset();
    m_searchResultArea->reset();
}

void CSearchDialog::closeButtonClicked() {
    // With Qt::WA_DeleteOnClose set, the dialog will be deleted now
    m_staticDialog->close();
}

void CSearchDialog::loadDialogSettings() {
    resize(CBTConfig::get(CBTConfig::searchDialogWidth), CBTConfig::get(CBTConfig::searchDialogHeight));
    move(CBTConfig::get(CBTConfig::searchDialogX), CBTConfig::get(CBTConfig::searchDialogY));
}

void CSearchDialog::saveDialogSettings() {
    CBTConfig::set(CBTConfig::searchDialogWidth, size().width());
    CBTConfig::set(CBTConfig::searchDialogHeight, size().height());
    CBTConfig::set(CBTConfig::searchDialogX, x());
    CBTConfig::set(CBTConfig::searchDialogY, y());
}


} //end of namespace Search
