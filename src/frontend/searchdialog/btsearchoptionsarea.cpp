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

#include "btsearchoptionsarea.h"

#include <QDebug>
#include <QEvent>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include "../../backend/config/btconfig.h"
#include "../../backend/drivers/btconstmoduleset.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../backend/managers/cswordbackend.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../../util/tool.h"
#include "btsearchmodulechooserdialog.h"
#include "btsearchsyntaxhelpdialog.h"
#include "crangechooserdialog.h"


namespace {
auto const SearchTypeKey = QStringLiteral("GUI/SearchDialog/searchType");
} // anonymous namespace

namespace Search {

BtSearchOptionsArea::BtSearchOptionsArea(QWidget *parent )
        : QWidget(parent) {
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

CSwordModuleSearch::SearchType BtSearchOptionsArea::searchType() {
    if (m_typeAndButton->isChecked()) {
        return CSwordModuleSearch::AndType;
    }
    if (m_typeOrButton->isChecked()) {
        return CSwordModuleSearch::OrType;
    }
    return CSwordModuleSearch::FullType;
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

void BtSearchOptionsArea::initView() {
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
    m_searchButton->setIcon(CResMgr::searchdialog::icon());
    m_searchButton->setToolTip(tr("Start to search the text in the chosen works"));
    gridLayout->addWidget(m_searchButton, 0, 2);

    m_chooseModulesButton = new QPushButton(tr("Ch&oose..."), searchGroupBox);
    m_chooseModulesButton->setIcon(CResMgr::searchdialog::icon_chooseWorks());
    m_chooseModulesButton->setToolTip( tr("Choose works for the search"));
    gridLayout->addWidget(m_chooseModulesButton, 2, 2);

    m_chooseRangeButton = new QPushButton(tr("S&etup..."), searchGroupBox);
    m_chooseRangeButton->setIcon(CResMgr::searchdialog::icon_setupScope());
    m_chooseRangeButton->setToolTip(tr("Configure predefined scopes for search"));
    gridLayout->addWidget(m_chooseRangeButton, 3, 2);

    // ************* Search type (AND/OR) selector ***************************************
    QHBoxLayout* typeSelectorLayout = new QHBoxLayout();
    int tsLeft, tsTop, tsRight, tsBottom;
    // Added space looks nicer and enhances readability
    typeSelectorLayout->getContentsMargins(&tsLeft, &tsTop, &tsRight, &tsBottom);
    typeSelectorLayout->setContentsMargins(
        tsLeft,
        0,
        tsRight,
        tsBottom + util::tool::mWidth(*this, 1));
    typeSelectorLayout->setSpacing(typeSelectorLayout->spacing()
                                   + util::tool::mWidth(*this, 1));
    QHBoxLayout* fullButtonLayout = new QHBoxLayout();
    fullButtonLayout->setSpacing(util::tool::mWidth(*this, 1) / 2);
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
    gridLayout->addLayout(typeSelectorLayout, 1, 1, 1, -1, Qt::AlignLeft | Qt::AlignTop);

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

void BtSearchOptionsArea::initConnections() {
    auto startSearch =
            [this]{
                m_searchTextCombo->addToHistory(
                            m_searchTextCombo->currentText());
                Q_EMIT sigStartSearch();
            };
    BT_CONNECT(m_searchButton, &QPushButton::clicked, startSearch);
    BT_CONNECT(m_searchTextCombo->lineEdit(), &QLineEdit::returnPressed,
               std::move(startSearch));
    BT_CONNECT(m_chooseModulesButton, &QPushButton::clicked,
               this,                  &BtSearchOptionsArea::chooseModules);
    BT_CONNECT(m_chooseRangeButton, &QPushButton::clicked,
               [this]{
                   CRangeChooserDialog(getUniqueWorksList(), this).exec();
                   refreshRanges();
               });
    BT_CONNECT(m_modulesCombo, qOverload<int>(&QComboBox::activated),
               [this](int const index) {
                   BtConstModuleList moduleList;
                   for (auto const & name
                        : m_modulesCombo->itemText(index).split(
                            QStringLiteral(", ")))
                       moduleList.append(
                            CSwordBackend::instance().findModuleByName(name));
                   // Set the list and the combobox list and text:
                   setModules(moduleList);
               });
    BT_CONNECT(m_helpLabel, &QLabel::linkActivated,
               [this]{
                   auto * const dlg = new BtSearchSyntaxHelpDialog(this);
                   dlg->setAttribute(Qt::WA_DeleteOnClose);
                   dlg->show();
               });
    #if 0
    BT_CONNECT(m_searchTextCombo, &CHistoryComboBox::editTextChanged,
               this,              &BtSearchOptionsArea::slotValidateText);
    #endif
}

/** Sets the modules used by the search. */
void BtSearchOptionsArea::setModules(const BtConstModuleList &modules) {
    QString t;

    m_modules.clear(); //remove old modules
    for (auto * const modulePtr : modules) {
        /// \todo Check for containsRef compat
        if (!modulePtr) //don't operate on null modules.
            continue;
        qDebug() << "new module:" << modulePtr->name();
        if (!m_modules.contains(modulePtr)) {
            m_modules.append(modulePtr);
            t.append(modulePtr->name());
            if (modulePtr != modules.last())
                t += QStringLiteral(", "); // so that it will become a readable list (WLC, LXX, GerLut...)
        }
    }
    //m_modulesLabel->setText(t);
    int existingIndex = m_modulesCombo->findText(t);
    qDebug() << "index of the module list string which already exists in combobox:" << existingIndex;
    if (existingIndex >= 0) {
        m_modulesCombo->removeItem(existingIndex);
    }
    if (m_modulesCombo->count() > 10) {
        m_modulesCombo->removeItem(m_modulesCombo->count() - 1);
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
    btConfig().setValue(QStringLiteral("history/searchModuleHistory"),
                        historyList);
}

QStringList BtSearchOptionsArea::getUniqueWorksList() {
    QSet<QString> moduleSet;
    for (auto const & value
         : btConfig().value<QStringList>(
             QStringLiteral("history/searchModuleHistory")))
        for (auto const & name : value.split(QStringLiteral(", ")))
            moduleSet.insert(name);
    return moduleSet.values();
}

void BtSearchOptionsArea::chooseModules() {
    BtSearchModuleChooserDialog dlg(this);
    auto & ms = modules();
    dlg.setCheckedModules(QSet<CSwordModuleInfo const *>(ms.begin(), ms.end()));
    if (dlg.exec() == QDialog::Accepted) {
        auto const cms(dlg.checkedModules());
        setModules(BtConstModuleList(cms.begin(), cms.end()));
    }
}

void BtSearchOptionsArea::reset() {
    int index = m_rangeChooserCombo->findText(
                    btConfig().value<QString>(
                        QStringLiteral("searchScopeCurrent")));
    if (index >= 0)
        m_rangeChooserCombo->setCurrentIndex(index);
    else
        m_rangeChooserCombo->setCurrentIndex(0);
    m_searchTextCombo->clearEditText();
}

void BtSearchOptionsArea::saveSettings() {
    btConfig().setValue(QStringLiteral("searchScopeCurrent"),
                        m_rangeChooserCombo->currentText());
    btConfig().setValue(QStringLiteral("properties/searchTexts"),
                        m_searchTextCombo->historyItems());
    CSwordModuleSearch::SearchType t = CSwordModuleSearch::FullType;
    if (m_typeAndButton->isChecked()) {
        t = CSwordModuleSearch::AndType;
    }
    if (m_typeOrButton->isChecked()) {
        t = CSwordModuleSearch::OrType;
    }
    btConfig().setValue(SearchTypeKey, t);
}

void BtSearchOptionsArea::readSettings() {
    auto const texts =
            btConfig().value<QStringList>(
                QStringLiteral("properties/searchTexts"));
    //for some reason the slot was called when setting the upmost item
    #if 0
    disconnect(m_searchTextCombo, &CHistoryComboBox::editTextChanged,
               this,              &BtSearchOptionsArea::slotValidateText);
    #endif
    for (auto const & text : texts)
        if (text.size() > 0)
            m_searchTextCombo->addItem(text);
    #if 0
    BT_CONNECT(m_searchTextCombo, &CHistoryComboBox::editTextChanged,
               this,              &BtSearchOptionsArea::slotValidateText);
    #endif

    m_modulesCombo->insertItems(
                0,
                btConfig().value<QStringList>(
                    QStringLiteral("history/searchModuleHistory")));
    for (int i = 0; i < m_modulesCombo->count(); ++i) {
        m_modulesCombo->setItemData(i, m_modulesCombo->itemText(i), Qt::ToolTipRole);
    }

    int stype = btConfig().value<int>(SearchTypeKey, CSwordModuleSearch::AndType);
    switch (stype) {
        case CSwordModuleSearch::AndType:
            m_typeAndButton->setChecked(true);
            break;
        case CSwordModuleSearch::OrType:
            m_typeOrButton->setChecked(true);
            break;
        default:
            m_typeFreeButton->setChecked(true);
    }
}

void BtSearchOptionsArea::refreshRanges() {
    //the first option is fixed, the others can be edited using the "Setup ranges" button.
    m_rangeChooserCombo->clear();
    m_rangeChooserCombo->insertItem(0, tr("[No search scope]"));
    /// \todo what about this?
    //m_rangeChooserCombo->insertItem(tr("Last search result"));

    //insert the user-defined ranges
    QStringList scopeModules = getUniqueWorksList();
    m_rangeChooserCombo->insertItems(1, btConfig().getSearchScopesForCurrentLocale(scopeModules).keys());
}

sword::ListKey BtSearchOptionsArea::searchScope() {
    QStringList scopeModules = getUniqueWorksList();
    if (m_rangeChooserCombo->currentIndex() > 0) { //is not "no scope"
        QString const scope = btConfig().getSearchScopesForCurrentLocale(scopeModules)[
                                    m_rangeChooserCombo->currentText()];
        if (!scope.isEmpty())
            return BtConfig::parseVerseListWithModules(scope, scopeModules);
    }
    return sword::ListKey();
}

void BtSearchOptionsArea::addToHistory(const QString& text) {
    m_searchTextCombo->addToHistory(text);
}

bool BtSearchOptionsArea::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
        if (obj == m_searchTextCombo->view() || obj == m_searchTextCombo || obj == m_searchTextCombo->lineEdit()) {
            obj->event(event);
            // don't handle this event in parent
            event->accept();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

#if 0
void BtSearchOptionsArea::slotValidateText(QString const & newText) {
    static const QRegExp re("\\b(AND|OR)\\b");
    if (newText.isEmpty() || !newText.contains(re)) {
        if (!m_typeAndButton->isEnabled()) {
            m_typeOrButton->setEnabled(true);
            m_typeAndButton->setEnabled(true);
            m_typeAndButton->setToolTip(
                    tr("All of the words (AND is added between the words)"));
            m_typeOrButton->setToolTip(tr("Some of the words"));
        }
    } else {
        if (m_typeAndButton->isEnabled()) {
            m_typeOrButton->setChecked(true);
            m_typeOrButton->setEnabled(false);
            m_typeAndButton->setEnabled(false);
            m_typeAndButton->setToolTip(
                    tr("Full syntax is used because text includes AND or OR"));
            m_typeOrButton->setToolTip(
                    tr("Full syntax is used because text includes AND or OR"));
        }
    }
}
#endif

} // namespace Search

