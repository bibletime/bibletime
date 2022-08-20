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

#include "cacceleratorsettings.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QLabel>
#include <QList>
#include <QMessageBox>
#include <QNonConstOverload>
#include <QStackedWidget>
#include <Qt>
#include <QVBoxLayout>
#include <utility>
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../bibletime.h"
#include "../displaywindow/btactioncollection.h"
#include "../displaywindow/cbiblereadwindow.h"
#include "../displaywindow/cbookreadwindow.h"
#include "../displaywindow/ccommentaryreadwindow.h"
#include "../displaywindow/cdisplaywindow.h"
#include "../displaywindow/clexiconreadwindow.h"
#include "../messagedialog.h"
#include "btshortcutseditor.h"
#include "cconfigurationdialog.h"


CAcceleratorSettingsPage::CAcceleratorSettingsPage(CConfigurationDialog *parent)
        : BtConfigDialog::Page(CResMgr::settings::keys::icon(), parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout* layoutForWindowTypeChooser = new QHBoxLayout();
    mainLayout->addLayout(layoutForWindowTypeChooser);

    m_actionGroupLabel = new QLabel(this);
    layoutForWindowTypeChooser->addWidget(m_actionGroupLabel);

    m_typeChooser = new QComboBox(this);
    layoutForWindowTypeChooser->addWidget(m_typeChooser);

    BT_CONNECT(m_typeChooser, qOverload<int>(&QComboBox::activated),
               [this](int i) { m_keyChooserStack->setCurrentIndex(i); });

    // m_*.title strings are empty here, they are filled and added to the stacked widget in the retranslateUi() function
    m_keyChooserStack = new QStackedWidget(this);
    retranslateUi();

    // create shortcuteditors
    auto const initShortcutEditor =
        [this](WindowType & windowType,
               void (& insertKeyboardActions)(BtActionCollection * const),
               char const * const groupName)
        {
            windowType.actionCollection = new BtActionCollection(this);
            insertKeyboardActions(windowType.actionCollection);
            windowType.actionCollection->readShortcuts(groupName);
            windowType.keyChooser =
                    new BtShortcutsEditor(windowType.actionCollection,
                                          m_keyChooserStack);
            m_keyChooserStack->addWidget(windowType.keyChooser);
            BT_CONNECT(
                windowType.keyChooser,
                &BtShortcutsEditor::keyChangeRequest,
                [this, &windowType](QString const & actionName,
                                    QKeySequence const & keys)
                {
                    /* Check the BtShortcutsEditor's for shortcut conflicts.
                       Either clear the conflicts and set the new shortcut or do
                       nothing. */

                    /* Get the list of shortcuts editors that can conflict with
                       a key change in the current shortcut editor: */
                    QList<WindowType *> list;
                    list.append(&m_application);
                    list.append(&m_general);
                    if ((&windowType == &m_application)
                        || (&windowType == &m_general))
                    {
                        list.append(&m_bible);
                        list.append(&m_commentary);
                        list.append(&m_lexicon);
                        list.append(&m_book);
                    } else {
                        list.append(&windowType);
                    }
                    QStringList conflicts;
                    auto const conflictTr = tr("\"%1\" in the \"%2\" group");
                    for (auto const * const windowType2 : list)
                        if (auto conflict =
                                windowType2->keyChooser->findConflictWithKeys(
                                    keys);
                            !conflict.isEmpty())
                            conflicts.append(conflictTr
                                             .arg(std::move(conflict))
                                             .arg(windowType2->title));

                    if (!conflicts.isEmpty()) {
                        if (message::showQuestion(
                                this,
                                tr("Shortcut conflict detected"),
                                QStringLiteral("%1<ul><li>%2</li></ul>%3")
                                    .arg(
                                        tr("The shortcut \"%1\" you want to "
                                           "assign to \"%2\" in the \"%3\" "
                                           "group conflicts with the following "
                                           "%n shortcut(s):",
                                           nullptr,
                                           conflicts.size())
                                        .arg(keys.toString())
                                        .arg(actionName)
                                        .arg(windowType.title),
                                    conflicts.join(QStringLiteral("</li><li>")),
                                    tr("Do you want to clear these conflicting "
                                       "shortcuts and continue?")),
                                QMessageBox::Yes | QMessageBox::No,
                                QMessageBox::Yes) == QMessageBox::Yes)
                        {
                            // Clear conflicts with keys:
                            for (auto const * const windowType2 : list)
                                windowType2->keyChooser->clearConflictWithKeys(
                                                            keys);

                            windowType.keyChooser->changeShortcutInDialog(keys);
                        }
                    } else {
                        windowType.keyChooser->changeShortcutInDialog(keys);
                    }
                });
        };
    initShortcutEditor(m_application,
                       BibleTime::insertKeyboardActions,
                       "Application shortcuts");
    initShortcutEditor(m_general,
                       CDisplayWindow::insertKeyboardActions,
                       "Displaywindow shortcuts");
    initShortcutEditor(m_bible,
                       CBibleReadWindow::insertKeyboardActions,
                       "Bible shortcuts");
    initShortcutEditor(m_commentary,
                       CCommentaryReadWindow::insertKeyboardActions,
                       "Commentary shortcuts");
    initShortcutEditor(m_lexicon,
                       CLexiconReadWindow::insertKeyboardActions,
                       "Lexicon shortcuts");
    initShortcutEditor(m_book,
                       CBookReadWindow::insertKeyboardActions,
                       "Book shortcuts");

    mainLayout->addWidget(m_keyChooserStack);
    m_keyChooserStack->setCurrentIndex(m_typeChooser->currentIndex());

    m_typeChooser->setFocus(Qt::MouseFocusReason);

    retranslateUi();
}

void CAcceleratorSettingsPage::retranslateUi() {
    setHeaderText(tr("Shortcuts"));

    m_actionGroupLabel->setText(tr("Choose action group:"));

    m_application.title = tr("Main Window");
    m_general.title = tr("All text windows");
    m_bible.title = tr("Bible windows");
    m_commentary.title = tr("Commentary windows");
    m_lexicon.title = tr("Lexicon windows");
    m_book.title = tr("Book windows");

    m_typeChooser->clear();
    m_typeChooser->addItem(m_application.title);
    m_typeChooser->addItem(m_general.title);
    m_typeChooser->addItem(m_bible.title);
    m_typeChooser->addItem(m_commentary.title);
    m_typeChooser->addItem(m_lexicon.title);
    m_typeChooser->addItem(m_book.title);
}

void CAcceleratorSettingsPage::save() const {
    if (m_application.keyChooser)
        m_application.keyChooser->commitChanges();
    if (m_general.keyChooser)
        m_general.keyChooser->commitChanges();
    if (m_bible.keyChooser)
        m_bible.keyChooser->commitChanges();
    if (m_commentary.keyChooser)
        m_commentary.keyChooser->commitChanges();
    if (m_lexicon.keyChooser)
        m_lexicon.keyChooser->commitChanges();
    if (m_book.keyChooser)
        m_book.keyChooser->commitChanges();

    m_application.actionCollection->writeShortcuts("Application shortcuts"); //application
    m_general.actionCollection->writeShortcuts("Displaywindow shortcuts"); //read display windows
    m_bible.actionCollection->writeShortcuts("Bible shortcuts"); //bible
    m_commentary.actionCollection->writeShortcuts("Commentary shortcuts"); //commentary
    m_lexicon.actionCollection->writeShortcuts("Lexicon shortcuts"); //lexicon
    m_book.actionCollection->writeShortcuts("Book shortcuts"); //book
}
