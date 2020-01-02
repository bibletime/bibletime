/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btshortcutseditor.h"

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
#include "../displaywindow/btactioncollection.h"
#include "../../util/btconnect.h"
#include "btshortcutsdialog.h"


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
        : m_action(action), m_newFirstHotkey(nullptr), m_newSecondHotkey(nullptr) {
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
    if (m_newFirstHotkey == nullptr)
        m_newFirstHotkey = new QKeySequence();
    *m_newFirstHotkey = keys;
}

void BtShortcutsEditorItem::setSecondHotkey(const QString& keys) {
    if (m_newSecondHotkey == nullptr)
        m_newSecondHotkey = new QKeySequence();
    *m_newSecondHotkey = QKeySequence(keys);
}

// Deletes hotkey information
void BtShortcutsEditorItem::deleteHotkeys() {
    delete m_newFirstHotkey;
    m_newFirstHotkey = nullptr;
    delete m_newSecondHotkey;
    m_newSecondHotkey = nullptr;
}

// Moves the hotkey information into the QAction variable
void BtShortcutsEditorItem::commitChanges() {
    QList<QKeySequence> list;
    if ( (m_newFirstHotkey != nullptr) && (*m_newFirstHotkey != QKeySequence()) ) {
        list << *m_newFirstHotkey;
    }
    if ( (m_newSecondHotkey != nullptr) && (*m_newSecondHotkey != QKeySequence()) )
        list << *m_newSecondHotkey;

    if (m_action != nullptr)
        m_action->setShortcuts(list);
}


// ******************* BtShortcutsEditor *******************************************************

BtShortcutsEditor::BtShortcutsEditor(BtActionCollection* collection, QWidget* parent)
        : QWidget(parent), m_dlg(new BtShortcutsDialog(this)), m_table(nullptr), m_shortcutChooser(nullptr), m_noneButton(nullptr), m_defaultButton(nullptr),
        m_customButton(nullptr), m_defaultLabelValue(nullptr), m_currentRow(-1) {

    QVBoxLayout * const vBox = new QVBoxLayout(this);
    setLayout(vBox);

    // Create the action and shortcuts table:
    m_table = new QTableWidget{this};
    m_table->setColumnCount(3);
    m_table->setAlternatingRowColors(true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setHorizontalHeaderLabels({tr("Action\nname"),
                                        tr("First\nshortcut"),
                                        tr("Second\nshortcut")});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_table->horizontalHeader()->resizeSection(0, 180);
    m_table->horizontalHeader()->resizeSection(1, 100);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->setVisible(false);
    m_table->setShowGrid(false);
    BT_CONNECT(m_table, &QTableWidget::cellClicked,
               this,    &BtShortcutsEditor::changeRow);
    vBox->addWidget(m_table);

    // Create the area below the table where the shortcuts are edited:
    m_shortcutChooser =
            new QGroupBox(tr("Shortcut for selected action name"), this);
    m_shortcutChooser->setFlat(false);
    {
        QVBoxLayout * const vLayout = new QVBoxLayout(m_shortcutChooser);
        {
            QHBoxLayout * const hLayout = new QHBoxLayout();
            vLayout->addLayout(hLayout);

            m_noneButton = new QRadioButton(tr("None"), m_shortcutChooser);
            hLayout->addWidget(m_noneButton);
            BT_CONNECT(m_noneButton, &QRadioButton::clicked,
                       this,         &BtShortcutsEditor::noneButtonClicked);

            m_defaultButton =
                    new QRadioButton(tr("Default"), m_shortcutChooser);
            hLayout->addWidget(m_defaultButton);
            BT_CONNECT(
                    m_defaultButton, &QRadioButton::clicked,
                    this,            &BtShortcutsEditor::defaultButtonClicked);

            m_customButton = new QRadioButton(tr("Custom"), m_shortcutChooser);
            hLayout->addWidget(m_customButton);
            BT_CONNECT(m_customButton, &QRadioButton::clicked,
                       this,           &BtShortcutsEditor::customButtonClicked);

            m_customPushButton = new QPushButton(m_shortcutChooser);
            m_customPushButton->setMinimumWidth(140);
            hLayout->addWidget(m_customPushButton);

            hLayout->addItem(new QSpacerItem(1,
                                             1,
                                             QSizePolicy::Expanding,
                                             QSizePolicy::Minimum));
        }

        QHBoxLayout * const hLayout = new QHBoxLayout();
        vLayout->addLayout(hLayout);

        hLayout->addWidget(new QLabel(tr("Default key:"), m_shortcutChooser));

        m_defaultLabelValue = new QLabel(m_shortcutChooser);
        hLayout->addWidget(m_defaultLabelValue);

        hLayout->addItem(new QSpacerItem(1,
                                         1,
                                         QSizePolicy::Expanding,
                                         QSizePolicy::Minimum));
    }
    vBox->addWidget(m_shortcutChooser);

    collection->foreachQAction([this](QAction & action,
                                      QKeySequence const & defaultKeys)
        {
            int const count = m_table->rowCount();
            m_table->insertRow(count);

            {
                BtShortcutsEditorItem * const item =
                        new BtShortcutsEditorItem{&action};
                try {
                    /// \todo Remove this & hack and use Qt properties instead:
                    item->setText(action.text().replace(QRegExp("&(.)"), "\\1"));
                    item->setIcon(action.icon());
                    item->setDefaultKeys(defaultKeys);
                    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                    m_table->setItem(count, 0, item);
                } catch (...) {
                    delete item;
                    throw;
                }
            }

            QList<QKeySequence> keys = action.shortcuts();

            {
                QTableWidgetItem * const item = new QTableWidgetItem;
                try {
                    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                    item->setToolTip(tr("Select to change key"));
                    if (keys.count() > 0)
                        item->setText(keys[0].toString());
                    m_table->setItem(count, 1, item);
                } catch (...) {
                    delete item;
                    throw;
                }
            }

            {
                QTableWidgetItem * const item = new QTableWidgetItem;
                try {
                    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                    item->setToolTip(tr("Select to change key"));
                    if (keys.count() > 1)
                        item->setText(keys[1].toString());
                    m_table->setItem(count, 2, item);
                } catch (...) {
                    delete item;
                    throw;
                }
            }
        });
    m_table->sortItems(0);
    m_table->selectRow(0);
    changeRow(0, 0);
    BT_CONNECT(m_dlg, &BtShortcutsDialog::keyChangeRequest,
               this,  &BtShortcutsEditor::makeKeyChangeRequest);
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
        if (btItem != nullptr)
            btItem->commitChanges();
    }
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
