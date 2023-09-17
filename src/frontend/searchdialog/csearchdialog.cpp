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

#include "csearchdialog.h"

#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSizePolicy>
#include <QString>
#include <QRegularExpression>
#include <QVBoxLayout>
#include <QWidget>
#include <utility>
#include "../../backend/config/btconfig.h"
#include "../../backend/cswordmodulesearch.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../btmoduleindexdialog.h"
#include "../messagedialog.h"
#include "btindexdialog.h"
#include "btsearchoptionsarea.h"
#include "btsearchresultarea.h"


namespace {
const QString GeometryKey = "GUI/SearchDialog/geometry";
} // anonymous namespace

namespace Search {

CSearchDialog::CSearchDialog(QWidget * const parent)
    : QDialog(parent)
{
    setWindowIcon(CResMgr::searchdialog::icon());
    setWindowTitle(tr("Search"));
    setAttribute(Qt::WA_DeleteOnClose);


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

    QSpacerItem* spacerItem = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(spacerItem);

    m_analyseButton = new QPushButton(tr("&Analyze results..."), this);
    m_analyseButton->setToolTip(tr("Show a graphical analysis of the search result"));
    horizontalLayout->addWidget(m_analyseButton);

    m_manageIndexes = new QPushButton(tr("&Manage Indexes..."), this);
    m_manageIndexes->setToolTip(tr("Recreate search indexes"));
    horizontalLayout->addWidget(m_manageIndexes);

    m_closeButton = new QPushButton(this);
    m_closeButton->setText(tr("&Close"));
    m_closeButton->setIcon(CResMgr::searchdialog::icon_close());
    horizontalLayout->addWidget(m_closeButton);

    verticalLayout->addLayout(horizontalLayout);

    // Load dialog settings:
    restoreGeometry(btConfig().value<QByteArray>(GeometryKey, QByteArray()));

    // Search button is clicked
    // Return/Enter is pressed in the search text field
    BT_CONNECT(m_searchOptionsArea, &BtSearchOptionsArea::sigStartSearch,
               this,                &CSearchDialog::startSearch);
    BT_CONNECT(m_closeButton, &QPushButton::clicked,
               this, &CSearchDialog::close);

    BT_CONNECT(m_analyseButton, &QPushButton::clicked,
               m_searchResultArea, &BtSearchResultArea::showAnalysis);

    BT_CONNECT(m_manageIndexes, &QPushButton::clicked,
               [this] { BtIndexDialog(this).exec(); });
}

CSearchDialog::~CSearchDialog() // Save dialog settings:
{ btConfig().setValue(GeometryKey, saveGeometry()); }

void CSearchDialog::startSearch() {
    QString originalSearchText(m_searchOptionsArea->searchText());

    // first check the search string for errors
    {
        QString TestString(originalSearchText);
        static QRegularExpression const ReservedWords(
                    QStringLiteral("heading:|footnote:|morph:|strong:"));
        if (TestString.replace(ReservedWords, QString()).simplified().isEmpty()) {
            return;
        }
    }
    QString searchText = CSwordModuleSearch::prepareSearchText(originalSearchText, m_searchOptionsArea->searchType());

    // Insert search text into history list of combobox
    m_searchOptionsArea->addToHistory(originalSearchText);

    auto const & searchModules = m_searchOptionsArea->modules();

    // Check that we have the indices we need for searching
    /// \warning indexing is some kind of internal optimization, so we leave
    /// modules const, but unconst them here only
    QList<CSwordModuleInfo*> unindexedModules;
    for (auto const * const m : searchModules)
        if (!m->hasIndex())
            unindexedModules.append(const_cast<CSwordModuleInfo*>(m));

    if (unindexedModules.size() > 0) {
        // Build the list of module names:
        QStringList moduleNameList;
        for (auto const * const m : unindexedModules)
            moduleNameList.append(m->name());

        // Ask the user about unindexed modules:
        auto const result =
                message::showQuestion(
                    this,
                    tr("Missing indices"),
                    QStringLiteral("%1<br><center>%2</center><br>%3")
                    .arg(tr("The following modules need to be indexed before "
                            "they can be searched in:"),
                         moduleNameList.join(QStringLiteral(", ")),
                         tr("Indexing could take a long time. Click \"Yes\" to "
                            "index the modules and start the search, or \"No\" "
                            "to cancel the search.")),
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::Yes);

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

    // Disable the dialog:
    setEnabled(false);
    setCursor(Qt::WaitCursor);

    // Execute search:
    CSwordModuleSearch::Results searchResult;
    try {
        searchResult =
                CSwordModuleSearch::search(searchText,
                                           searchModules,
                                           m_searchOptionsArea->searchScope());
    } catch (...) {
        QString msg;
        try {
            throw;
        } catch (std::exception const & e) {
            msg = e.what();
        } catch (...) {
            msg = tr("<UNKNOWN EXCEPTION>");
        }

        message::showWarning(this,
                             tr("Search aborted"),
                             tr("An internal error occurred while executing "
                                "your search:<br/><br/>%1").arg(msg));
        // Re-enable the dialog:
        setEnabled(true);
        setCursor(Qt::ArrowCursor);
        return;
    }

    // Display the search results:
    if (!searchResult.empty()) {
        m_searchResultArea->setSearchResult(m_searchOptionsArea->searchText(),
                                            std::move(searchResult));
        m_analyseButton->setEnabled(true);
    } else {
        m_searchResultArea->reset();
    }
    raise();
    activateWindow();

    // Re-enable the dialog:
    setEnabled(true);
    setCursor(Qt::ArrowCursor);
}

void CSearchDialog::reset(BtConstModuleList modules, QString const & searchText)
{
    m_searchOptionsArea->reset();
    m_searchResultArea->reset();
    m_analyseButton->setEnabled(false);

    auto const haveModules = !modules.isEmpty();
    if (haveModules) {
        m_searchOptionsArea->setModules(std::move(modules));
    } else {
        m_searchOptionsArea->chooseModules();
    }

    m_searchOptionsArea->setSearchText(searchText);
    if (isHidden())
        show();

    if (haveModules && !searchText.isEmpty())
        startSearch();

    // moved these to after the startSearch() because
    // the progress dialog caused them to loose focus.
    raise();
    activateWindow();
}

} //end of namespace Search
