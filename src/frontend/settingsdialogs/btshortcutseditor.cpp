/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/settingsdialogs/btshortcutseditor.h"

#include <QAction>
#include <QGroupBox>
#include <QHeaderView>
#include <QKeySequence>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QSpacerItem>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/settingsdialogs/btshortcutsdialog.h"


// *************** BtShortcutsEditorItem *******************************************************************
// BtShortcutsEditorItem is the widget for the first column of the BtShortcutsEditor
// It holds extra information about the action

class BtShortcutsEditorItem : public QTableWidgetItem {
    public:
        BtShortcutsEditorItem(QAction* action);
        ~BtShortcutsEditorItem();
        void commitChanges();
        QKeySequence getDefaultKeys();
        void setDefaultKeys(QKeySequence keys);
        void setFirstHotkey(QKeySequence keys);
        void setSecondHotkey(const QString& keys);
        QAction* getAction();
        void deleteHotkeys();

    private:
        QAction *m_action;
        QKeySequence *m_newFirstHotkey;
        QKeySequence *m_newSecondHotkey;
        QKeySequence m_defaultKeys;
};

BtShortcutsEditorItem::BtShortcutsEditorItem(QAction* action)
        : m_action(action), m_newFirstHotkey(0), m_newSecondHotkey(0) {
    QList<QKeySequence> list = m_action->shortcuts();
    if (list.count() > 0)
        m_newFirstHotkey = new QKeySequence(list.at(0));
    if (list.count() > 1)
        m_newSecondHotkey = new QKeySequence(list.at(1));
}

BtShortcutsEditorItem::~BtShortcutsEditorItem() {
    delete m_newFirstHotkey;
    delete m_newSecondHotkey;
}

QAction* BtShortcutsEditorItem::getAction() {
    return m_action;
}

QKeySequence BtShortcutsEditorItem::getDefaultKeys() {
    return m_defaultKeys;
}

void BtShortcutsEditorItem::setDefaultKeys(QKeySequence keys) {
    m_defaultKeys = keys;
}

void BtShortcutsEditorItem::setFirstHotkey(QKeySequence keys) {
    if (m_newFirstHotkey == 0)
        m_newFirstHotkey = new QKeySequence();
    *m_newFirstHotkey = keys;
}

void BtShortcutsEditorItem::setSecondHotkey(const QString& keys) {
    if (m_newSecondHotkey == 0)
        m_newSecondHotkey = new QKeySequence();
    *m_newSecondHotkey = QKeySequence(keys);
}

// Deletes hotkey information
void BtShortcutsEditorItem::deleteHotkeys() {
    delete m_newFirstHotkey;
    m_newFirstHotkey = 0;
    delete m_newSecondHotkey;
    m_newSecondHotkey = 0;
}

// Moves the hotkey information into the QAction variable
void BtShortcutsEditorItem::commitChanges() {
    QString actionName = text();
    QList<QKeySequence> list;
    if ( (m_newFirstHotkey != 0) && (*m_newFirstHotkey != QKeySequence()) ) {
        list << *m_newFirstHotkey;
    }
    if ( (m_newSecondHotkey != 0) && (*m_newSecondHotkey != QKeySequence()) )
        list << *m_newSecondHotkey;

    if (m_action != 0)
        m_action->setShortcuts(list);
}


// ******************* BtShortcutsEditor *******************************************************

BtShortcutsEditor::BtShortcutsEditor(BtActionCollection* collection, QWidget* parent)
        : QWidget(parent), m_dlg(new BtShortcutsDialog(this)), m_table(0), m_shortcutChooser(0), m_noneButton(0), m_defaultButton(0),
        m_customButton(0), m_defaultLabelValue(0), m_currentRow(-1) {
    init();
    addCollection(collection);
    bool ok = connect(m_dlg, SIGNAL(keyChangeRequest(const QString&)), this, SLOT(makeKeyChangeRequest(const QString&)) );
    Q_ASSERT(ok);
}

BtShortcutsEditor::BtShortcutsEditor(QWidget* parent)
        : QWidget(parent), m_table(0) {
    init();
}

// initialize this widget
void BtShortcutsEditor::init() {
    QVBoxLayout* vBox = new QVBoxLayout(this);
    setLayout(vBox);

    m_table = createShortcutsTable();
    vBox->addWidget(m_table);

    m_shortcutChooser = createShortcutChooser();
    vBox->addWidget(m_shortcutChooser);
}

// get the shortcut editor item from the zeroth column of the table
BtShortcutsEditorItem* BtShortcutsEditor::getShortcutsEditor(int row) {
    QTableWidgetItem* item = m_table->item(row, 0);
    BtShortcutsEditorItem* btItem = dynamic_cast<BtShortcutsEditorItem*>(item);
    return btItem;
}

// saves shortcut keys into the QAction
void BtShortcutsEditor::commitChanges() {
    int rows = m_table->rowCount();
    for (int row = 0; row < rows; row++) {
        BtShortcutsEditorItem* btItem = getShortcutsEditor(row);
        if (btItem != 0)
            btItem->commitChanges();
    }
}

// puts actions and shortcut keys into QTableWidget
void BtShortcutsEditor::addCollection(BtActionCollection* collection, const QString& title) {
    Q_UNUSED(title); /// \todo Is this correct?

    Q_FOREACH(QAction * const action, collection->actions()) {
        /// \todo Is the following check really necessary?
        if (action) {
            int count = m_table->rowCount();
            m_table->insertRow(count);

            QString name = action->text().remove('&');
            QList<QKeySequence> keys = action->shortcuts();
            QIcon icon = action->icon();
            QKeySequence defaultKeys = collection->getDefaultShortcut(action);

            BtShortcutsEditorItem* item = new BtShortcutsEditorItem(action);
            item->setText(name);
            item->setIcon(icon);
            item->setDefaultKeys(defaultKeys);
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            m_table->setItem(count, 0, item);

            QTableWidgetItem* item1 = new QTableWidgetItem();
            item1->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            item1->setToolTip(tr("Select to change key"));
            if (keys.count() > 0)
                item1->setText(keys[0].toString());
            m_table->setItem(count, 1, item1);

            QTableWidgetItem* item2 = new QTableWidgetItem();
            item2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            item2->setToolTip(tr("Select to change key"));
            if (keys.count() > 1)
                item2->setText(keys[1].toString());
            m_table->setItem(count, 2, item2);
        }
    }
    m_table->sortItems(0);
    m_table->selectRow(0);
    changeRow(0, 0);
}

// create the action and shortcuts table
QTableWidget* BtShortcutsEditor::createShortcutsTable() {
    QTableWidget* table = new QTableWidget(this);
    table->setColumnCount(3);
    table->setAlternatingRowColors(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    QStringList headerList;
    headerList << tr("Action\nname") << tr("First\nshortcut") << tr("Second\nshortcut");
    table->setHorizontalHeaderLabels(headerList);
#if QT_VERSION < 0x050000
    table->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
#else
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
#endif
    table->horizontalHeader()->resizeSection(0, 180);
    table->horizontalHeader()->resizeSection(1, 100);
    table->horizontalHeader()->setStretchLastSection(true);
    table->verticalHeader()->setVisible(false);
    table->setShowGrid(false);
    bool ok = connect(table, SIGNAL(cellClicked(int, int)), this, SLOT(changeRow(int, int)));
    Q_ASSERT(ok);
    return table;
}

// called when a different action name row is selected
void BtShortcutsEditor::changeRow(int row, int column) {
    Q_UNUSED(column); /// \todo Is this correct?

    BtShortcutsEditorItem* item = getShortcutsEditor(row);
    m_currentRow = row;
    QKeySequence defaultKeys = item->getDefaultKeys();

    m_defaultLabelValue->setText(defaultKeys.toString());

    QTableWidgetItem* item1 = m_table->item(row, 1);
    QString shortcut = item1->text();

    QTableWidgetItem* item2 = m_table->item(row, 2);
    QString alternate = item2->text();

    QString bothKeys = shortcut;
    if (!alternate.isEmpty())
        bothKeys = bothKeys + "; " + alternate;
    m_customPushButton->setText(bothKeys);

    if ( bothKeys == defaultKeys.toString())
        m_defaultButton->setChecked(true);
    else if (bothKeys.isEmpty())
        m_noneButton->setChecked(true);
    else
        m_customButton->setChecked(true);
}

// create the area below the table where the shortcuts are edited
QWidget* BtShortcutsEditor::createShortcutChooser() {
    QGroupBox* box = new QGroupBox(tr("Shortcut for selected action name"), this);
    box->setFlat(false);
    QVBoxLayout* vLayout = new QVBoxLayout(box);
    QHBoxLayout* hLayout = new QHBoxLayout();
    vLayout->addLayout(hLayout);

    m_noneButton = new QRadioButton(tr("None"), box);
    hLayout->addWidget(m_noneButton);
    bool ok = connect(m_noneButton, SIGNAL(clicked(bool)), this, SLOT(noneButtonClicked(bool)));
    Q_ASSERT(ok);

    m_defaultButton = new QRadioButton(tr("Default"), box);
    hLayout->addWidget(m_defaultButton);
    ok = connect(m_defaultButton, SIGNAL(clicked(bool)), this, SLOT(defaultButtonClicked(bool)));
    Q_ASSERT(ok);

    m_customButton = new QRadioButton(tr("Custom"), box);
    hLayout->addWidget(m_customButton);
    ok = connect(m_customButton, SIGNAL(clicked(bool)), this, SLOT(customButtonClicked(bool)));
    Q_ASSERT(ok);

    m_customPushButton = new QPushButton(box);
    m_customPushButton->setMinimumWidth(140);
    hLayout->addWidget(m_customPushButton);

    QSpacerItem* spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hLayout->addItem(spacer);

    QHBoxLayout* hLayout2 = new QHBoxLayout();
    vLayout->addLayout(hLayout2);

    QLabel* defaultLabel = new QLabel(tr("Default key:"), box);
    hLayout2->addWidget(defaultLabel);

    m_defaultLabelValue = new QLabel(box);
    hLayout2->addWidget(m_defaultLabelValue);

    QSpacerItem* spacer2 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hLayout2->addItem(spacer2);

    return box;
}


// called when the none radio button is clicked
void BtShortcutsEditor::noneButtonClicked(bool checked) {
    Q_UNUSED(checked); /// \todo Is this correct?

    if (m_currentRow < 0)
        return;
    BtShortcutsEditorItem* item = getShortcutsEditor(m_currentRow);
    m_customPushButton->setText("");
    item->deleteHotkeys();
    item->setFirstHotkey(QKeySequence(""));
    m_table->item(m_currentRow, 1)->setText("");
    m_table->item(m_currentRow, 2)->setText("");
}

// called when the default radio button is clicked
void BtShortcutsEditor::defaultButtonClicked(bool checked) {
    Q_UNUSED(checked); /// \todo Is this correct?

    if (m_currentRow < 0)
        return;
    BtShortcutsEditorItem* item = getShortcutsEditor(m_currentRow);
    QKeySequence defaultKeys = item->getDefaultKeys();
    item->deleteHotkeys();
    item->setFirstHotkey(defaultKeys);
    m_customPushButton->setText(defaultKeys.toString());
    m_table->item(m_currentRow, 1)->setText(defaultKeys.toString());
    m_table->item(m_currentRow, 2)->setText("");
}

// called when the custom radio button is clicked
void BtShortcutsEditor::customButtonClicked(bool checked) {
    Q_UNUSED(checked); /// \todo Is this correct?

    if (m_currentRow < 0)
        return;

    QString priKeys = m_table->item(m_currentRow, 1)->text();
    QString altKeys = m_table->item(m_currentRow, 2)->text();
    m_dlg->setSecondKeys(altKeys);
    m_dlg->setFirstKeys(priKeys);
    int code = m_dlg->exec();
    if (code == QDialog::Accepted) {
        QString newPriKeys = m_dlg->getFirstKeys();
        QString newAltKeys = m_dlg->getSecondKeys();
        if (newPriKeys == newAltKeys)
            newAltKeys = "";
        BtShortcutsEditorItem* item = getShortcutsEditor(m_currentRow);
        item->setFirstHotkey(newPriKeys);
        item->setSecondHotkey(newAltKeys);
        m_table->item(m_currentRow, 1)->setText(newPriKeys);
        m_table->item(m_currentRow, 2)->setText(newAltKeys);
    }
}

// complete the keyChangeRequest
void BtShortcutsEditor::makeKeyChangeRequest(const QString& keys) {
    // signal the application that this BtShortcutsEditor wants to change this shortcut (keys)
    // The application will check other BtShortcutsEditors and put out a message if the shortcut
    // is already in use. If the user requests reassignment, the application calls the BtShortcutsEditors to
    // reassign and set the shortcut.
    keyChangeRequest(this, keys);
}

// used by application to complete the keyChangeRequest signal
// stores "keys" into the custom shortcuts dialog field
void BtShortcutsEditor::changeShortcutInDialog(const QString& keys) {
    m_dlg->changeSelectedShortcut(keys);
}

// clears any shortcut keys in the table matching the specified keys
void BtShortcutsEditor::clearConflictWithKeys(const QString& keys) {
    QString conflict;
    for (int row = 0; row < m_table->rowCount(); row++) {
        BtShortcutsEditorItem* item = getShortcutsEditor(row);
        if (m_table->item(row, 1)->text() == keys) {
            m_table->item(row, 1)->setText("");
            item->setFirstHotkey(QKeySequence(""));
        }
        if (m_table->item(row, 2)->text() == keys) {
            m_table->item(row, 2)->setText("");
            item->setSecondHotkey(QKeySequence("").toString());
        }
    }
}

// finds any shortcut keys in the table matching the specified keys - returns the Action Name for it.
QString BtShortcutsEditor::findConflictWithKeys(const QString& keys) {
    QString conflict;
    for (int i = 0; i < m_table->rowCount(); i++) {
        if ( (m_table->item(i, 1)->text() == keys) || (m_table->item(i, 2)->text() == keys) )
            return m_table->item(i, 0)->text();
    }
    return conflict;
}
