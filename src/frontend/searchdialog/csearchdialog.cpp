/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/searchdialog/csearchdialog.h"

#include <QDebug>
#include <QDialogButtonBox>
#include <QLabel>
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
#include "frontend/btmoduleindexdialog.h"
#include "frontend/searchdialog/btsearchoptionsarea.h"
#include "frontend/searchdialog/btsearchresultarea.h"
#include "util/cresmgr.h"
#include "util/directory.h"
#include "util/dialogutil.h"


namespace Search {

static CSearchDialog* m_staticDialog = 0;

void CSearchDialog::openDialog(const QList<const CSwordModuleInfo*> modules,
                               const QString &searchText, QWidget *parentDialog)
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

    initView();
    initConnections();
}

CSearchDialog::~CSearchDialog() {
    saveDialogSettings();
    m_staticDialog = 0;
}

void CSearchDialog::startSearch() {
    typedef QList<const CSwordModuleInfo*> ML;
    QString originalSearchText(m_searchOptionsArea->searchText());

    // first check the search string for errors
    {
        QString TestString(originalSearchText);
        QRegExp ReservedWords("heading:|footnote:|morph:|strong:");
        if (TestString.replace(ReservedWords, "").simplified().isEmpty()) {
            return;
        }
    }
    QString searchText = prepareSearchText(originalSearchText);

    // Insert search text into history list of combobox
    m_searchOptionsArea->addToHistory(originalSearchText);

    // Check that we have the indices we need for searching
    ML unindexedModules = CSwordModuleSearch::unindexedModules(modules());
    if (unindexedModules.size() > 0) {
        // Build the list of module names:
        QStringList moduleNameList;
        Q_FOREACH (const CSwordModuleInfo *m, unindexedModules) {
            moduleNameList.append(m->name());
        }
        QString moduleNames("<br><center>");
        moduleNames.append(moduleNameList.join(", "));
        moduleNames.append("</center><br>");

        // Ask the user about unindexed modules:
        int result = util::showQuestion(
                this, tr("Missing indices"),
                tr("The following modules need to be indexed before they can be"
                   " searched in:") + moduleNames + tr("Indexing could take a l"
                   "ong time. Click \"Yes\" to index the modules and start the "
                   "search, or \"No\" to cancel the search."),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

        // User didn't press "Yes":
        if ((result & (QMessageBox::Yes | QMessageBox::Default)) == 0x0) {
            return;
        }

        // Show indexing dialog, and index the modules:
        if (!BtModuleIndexDialog::indexAllModules(unindexedModules)) {
            // Failed or user cancelled.
            return;
        }
    }

    // Set the search options:
    m_searcher.setSearchedText(searchText);
    m_searcher.setModules(modules());
    if (m_searchOptionsArea->hasSearchScope()) {
        m_searcher.setSearchScope(m_searchOptionsArea->searchScope());
    } else {
        m_searcher.resetSearchScope();
    }

    // Disable the dialog:
    setEnabled(false);
    setCursor(Qt::WaitCursor);

    // Execute search:
    m_searcher.startSearch();

    // Display the search results:
    if (m_searcher.foundItems() > 0) {
        m_searchResultArea->setSearchResult(m_searcher.results());
    } else {
        m_searchResultArea->reset();
    }
    m_staticDialog->raise();
    m_staticDialog->activateWindow();

    // Re-enable the dialog:
    setEnabled(true);
    setCursor(Qt::ArrowCursor);
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

void CSearchDialog::startSearch(const QList<const CSwordModuleInfo*> modules,
                                const QString &searchText)
{
    m_searchResultArea->reset();
    m_searchOptionsArea->reset();
    setModules(modules);
    setSearchText(searchText);

    startSearch();
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
    m_searchResultArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    verticalLayout->addWidget(m_searchResultArea);

    QLabel* hint = new QLabel(tr("Drag any verse reference onto an open Bible window"), this);
    verticalLayout->addWidget(hint);

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
