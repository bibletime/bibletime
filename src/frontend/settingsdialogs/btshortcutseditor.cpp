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

#include "btshortcutseditor.h"

#include <QAbstractItemView>
#include <QAction>
#include <QDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QKeySequence>
#include <QLabel>
#include <QList>
#include <QPushButton>
#include <QRadioButton>
#include <QSizePolicy>
#include <QSpacerItem>
#include <Qt>
#include <QtGlobal>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <utility>
#include "../displaywindow/btactioncollection.h"
#include "../../util/btconnect.h"
#include "btshortcutsdialog.h"


namespace {

inline QString removeMnemonics(QString input) {
    auto i = input.indexOf(QLatin1Char('&'));
    if (i < 0)
        return input;

    auto out = input.mid(0, i);
    decltype(i) nextFragmentStart;
    do {
        nextFragmentStart = i + 1;
        i = input.indexOf(QLatin1Char('&'), nextFragmentStart + 1);
        out.append(input.mid(nextFragmentStart, i - nextFragmentStart));
    } while (i >= 0);
    return out;
}

/** Widget for the first column of the BtShortcutsEditor holding extra
    information about the action. */
class BtShortcutsEditorItem : public QTableWidgetItem {

public: // methods:

    BtShortcutsEditorItem(QAction * action,
                          QKeySequence defaultKeys)
        : m_action(action)
        , m_defaultKeys(std::move(defaultKeys))
    {
        setText(removeMnemonics(action->text()));
        setIcon(action->icon());
        setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        QList<QKeySequence> list = m_action->shortcuts();
        if (list.count() > 0)
            m_newFirstHotkey = list.at(0);
        if (list.count() > 1)
            m_newSecondHotkey = list.at(1);
    }

    void commitChanges() {
        if (!m_action)
            return;
        QList<QKeySequence> list;
        if (!m_newFirstHotkey.isEmpty())
            list << m_newFirstHotkey;
        if (!m_newSecondHotkey.isEmpty())
            list << m_newSecondHotkey;

        m_action->setShortcuts(list);
    }

    QKeySequence getDefaultKeys() const { return m_defaultKeys; }

    template <typename ... Args>
    void setFirstHotkey(Args && ... args)
    { m_newFirstHotkey = QKeySequence(std::forward<Args>(args)...); }

    template <typename ... Args>
    void setSecondHotkey(Args && ... args)
    { m_newSecondHotkey = QKeySequence(std::forward<Args>(args)...); }

    void deleteHotkeys() noexcept {
        m_newFirstHotkey = QKeySequence();
        m_newSecondHotkey = QKeySequence();
    }

private: // fields:

    QAction *m_action;
    QKeySequence m_newFirstHotkey;
    QKeySequence m_newSecondHotkey;
    QKeySequence m_defaultKeys;

};

// Get the shortcut editor item from the zeroth column of the table:
inline auto getShortcutsEditor(QTableWidget const & tableWidget, int const row)
{ return dynamic_cast<BtShortcutsEditorItem *>(tableWidget.item(row, 0)); }

} // anonymous namespace

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
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setHorizontalHeaderLabels({tr("Action"),
                                        tr("Shortcut"),
                                        tr("Alternate")});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_table->horizontalHeader()->resizeSection(0, 180);
    m_table->horizontalHeader()->resizeSection(1, 100);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->setVisible(false);
    m_table->setShowGrid(false);
    BT_CONNECT(m_table, &QTableWidget::itemSelectionChanged,
               this,    &BtShortcutsEditor::slotSelectionChanged);
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
            BT_CONNECT(m_customPushButton, &QPushButton::clicked,
                       this, &BtShortcutsEditor::customButtonClicked);
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

    for (auto * const action : collection->actions()) {
        int const count = m_table->rowCount();
        m_table->insertRow(count);

        {
            auto * const item =
                    new BtShortcutsEditorItem(
                        action,
                        BtActionCollection::getDefaultShortcut(*action));
            try {
                /// \todo Remove this & hack and use Qt properties instead:
                m_table->setItem(count, 0, item);
            } catch (...) {
                delete item;
                throw;
            }
        }

        QList<QKeySequence> keys = action->shortcuts();

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
    }
    m_table->sortItems(0);
    m_table->selectRow(0);
    slotSelectionChanged();
    BT_CONNECT(m_dlg, &BtShortcutsDialog::keyChangeRequest,
               [this](QKeySequence const & keys) {
                   Q_EMIT keyChangeRequest(
                               m_table->item(m_currentRow, 0)->text(),
                               keys);
               });
}

// saves shortcut keys into the QAction
void BtShortcutsEditor::commitChanges() {
    int const rows = m_table->rowCount();
    for (int row = 0; row < rows; row++)
        if (auto * const btItem = getShortcutsEditor(*m_table, row))
            btItem->commitChanges();
}

// called when a different action name row is selected
void BtShortcutsEditor::slotSelectionChanged() {
    m_currentRow = m_table->currentRow();
    auto & item = *getShortcutsEditor(*m_table, m_currentRow);
    auto const defaultKeys = item.getDefaultKeys();

    m_defaultLabelValue->setText(defaultKeys.toString());

    QTableWidgetItem* item1 = m_table->item(m_currentRow, 1);
    QString shortcut = item1->text();

    QTableWidgetItem* item2 = m_table->item(m_currentRow, 2);
    QString alternate = item2->text();

    QString bothKeys = shortcut;
    if (!alternate.isEmpty())
        bothKeys = bothKeys + QStringLiteral("; ") + alternate;
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
    Q_UNUSED(checked) /// \todo Is this correct?

    if (m_currentRow < 0)
        return;
    auto & item = *getShortcutsEditor(*m_table, m_currentRow);
    m_customPushButton->setText(QString());
    item.deleteHotkeys();
    item.setFirstHotkey();
    m_table->item(m_currentRow, 1)->setText(QString());
    m_table->item(m_currentRow, 2)->setText(QString());
}

// called when the default radio button is clicked
void BtShortcutsEditor::defaultButtonClicked(bool checked) {
    Q_UNUSED(checked) /// \todo Is this correct?

    if (m_currentRow < 0)
        return;
    auto & item = *getShortcutsEditor(*m_table, m_currentRow);
    auto const defaultKeys = item.getDefaultKeys();
    item.deleteHotkeys();
    item.setFirstHotkey(defaultKeys);
    m_customPushButton->setText(defaultKeys.toString());
    m_table->item(m_currentRow, 1)->setText(defaultKeys.toString());
    m_table->item(m_currentRow, 2)->setText(QString());
}

// called when the custom radio button is clicked
void BtShortcutsEditor::customButtonClicked(bool checked) {
    Q_UNUSED(checked) /// \todo Is this correct?

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
            newAltKeys = QString();
        auto & item = *getShortcutsEditor(*m_table, m_currentRow);
        item.setFirstHotkey(newPriKeys);
        item.setSecondHotkey(newAltKeys);
        m_table->item(m_currentRow, 1)->setText(newPriKeys);
        m_table->item(m_currentRow, 2)->setText(newAltKeys);
    }
}

// used by application to complete the keyChangeRequest signal
// stores "keys" into the custom shortcuts dialog field
void BtShortcutsEditor::changeShortcutInDialog(QKeySequence const & keys)
{ m_dlg->changeSelectedShortcut(keys); }

// clears any shortcut keys in the table matching the specified keys
void BtShortcutsEditor::clearConflictWithKeys(QKeySequence const & keys) {
    auto const keyString(keys.toString());
    for (int row = 0; row < m_table->rowCount(); row++) {
        auto & item = *getShortcutsEditor(*m_table, row);
        if (m_table->item(row, 1)->text() == keyString) {
            m_table->item(row, 1)->setText(QString());
            item.setFirstHotkey();
        }
        if (m_table->item(row, 2)->text() == keyString) {
            m_table->item(row, 2)->setText(QString());
            item.setSecondHotkey();
        }
    }
}

// finds any shortcut keys in the table matching the specified keys - returns the Action Name for it.
QString
BtShortcutsEditor::findConflictWithKeys(QKeySequence const & keys) const {
    auto const keyString(keys.toString());
    for (int i = 0; i < m_table->rowCount(); i++)
        if ((m_table->item(i, 1)->text() == keyString)
            || (m_table->item(i, 2)->text() == keyString))
            return m_table->item(i, 0)->text();
    return {};
}
