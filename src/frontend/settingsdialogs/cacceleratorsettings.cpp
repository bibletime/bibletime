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

#include "cacceleratorsettings.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../bibletime.h"
#include "../bibletimeapp.h"
#include "../displaywindow/btactioncollection.h"
#include "../displaywindow/cbiblereadwindow.h"
#include "../displaywindow/cbookreadwindow.h"
#include "../displaywindow/ccommentaryreadwindow.h"
#include "../displaywindow/clexiconreadwindow.h"
#include "../displaywindow/creadwindow.h"
#include "../messagedialog.h"
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

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    BT_CONNECT(m_typeChooser, SIGNAL(activated(QString const &)),
        SLOT(slotKeyChooserTypeChanged(QString const&)) );
#else
    BT_CONNECT(m_typeChooser, SIGNAL(textActivated(QString const &)),
        SLOT(slotKeyChooserTypeChanged(QString const&)));
#endif

    // m_*.title strings are empty here, they are filled and added to the stacked widget in the retranslateUi() function
    m_keyChooserStack = new QStackedWidget(this);
    retranslateUi();

    // create shortcuteditors

    // ------ Application -------------- //
    m_application.actionCollection = new BtActionCollection(this);
    BibleTime::insertKeyboardActions( m_application.actionCollection);
    m_application.actionCollection->readShortcuts("Application shortcuts");
    m_application.keyChooser = new BtShortcutsEditor(m_application.actionCollection, m_keyChooserStack);
    m_keyChooserStack->addWidget(m_application.keyChooser);
    BT_CONNECT(m_application.keyChooser,
               SIGNAL(keyChangeRequest(BtShortcutsEditor *, QString const &)),
               this,
               SLOT(completeKeyChangeRequest(BtShortcutsEditor *,
                                             QString const &)));

    // ----- All display windows ------ //
    m_general.actionCollection = new BtActionCollection(this);
    CDisplayWindow::insertKeyboardActions( m_general.actionCollection);
    m_general.actionCollection->readShortcuts("Displaywindow shortcuts");
    m_general.keyChooser = new BtShortcutsEditor(m_general.actionCollection, m_keyChooserStack);
    m_keyChooserStack->addWidget(m_general.keyChooser);
    BT_CONNECT(m_general.keyChooser,
               SIGNAL(keyChangeRequest(BtShortcutsEditor *, QString const &)),
               this,
               SLOT(completeKeyChangeRequest(BtShortcutsEditor *,
                                             QString const &)));

    // ----- Bible windows ------ //
    m_bible.actionCollection = new BtActionCollection(this);
    CBibleReadWindow::insertKeyboardActions( m_bible.actionCollection);
    m_bible.actionCollection->readShortcuts("Bible shortcuts");
    m_bible.keyChooser = new BtShortcutsEditor(m_bible.actionCollection, m_keyChooserStack);
    m_keyChooserStack->addWidget(m_bible.keyChooser);
    BT_CONNECT(m_bible.keyChooser,
               SIGNAL(keyChangeRequest(BtShortcutsEditor *, QString const &)),
               this,
               SLOT(completeKeyChangeRequest(BtShortcutsEditor *,
                                             QString const &)));

    // ----- Commentary windows ------ //
    m_commentary.actionCollection = new BtActionCollection(this);
    CCommentaryReadWindow::insertKeyboardActions( m_commentary.actionCollection);
    m_commentary.actionCollection->readShortcuts("Commentary shortcuts");
    m_commentary.keyChooser = new BtShortcutsEditor(m_commentary.actionCollection, m_keyChooserStack);
    m_keyChooserStack->addWidget(m_commentary.keyChooser);
    BT_CONNECT(m_commentary.keyChooser,
               SIGNAL(keyChangeRequest(BtShortcutsEditor *, QString const &)),
               this,
               SLOT(completeKeyChangeRequest(BtShortcutsEditor *,
                                             QString const &)));

    // ----- Lexicon windows ------ //
    m_lexicon.actionCollection = new BtActionCollection(this);
    CLexiconReadWindow::insertKeyboardActions(  m_lexicon.actionCollection );
    m_lexicon.actionCollection->readShortcuts("Lexicon shortcuts");
    m_lexicon.keyChooser = new BtShortcutsEditor(m_lexicon.actionCollection, m_keyChooserStack);
    m_keyChooserStack->addWidget(m_lexicon.keyChooser);
    BT_CONNECT(m_lexicon.keyChooser,
               SIGNAL(keyChangeRequest(BtShortcutsEditor *, QString const &)),
               this,
               SLOT(completeKeyChangeRequest(BtShortcutsEditor *,
                                             QString const &)));

    // ----- Book windows ------ //
    m_book.actionCollection = new BtActionCollection(this);
    CBookReadWindow::insertKeyboardActions( m_book.actionCollection);
    m_book.actionCollection->readShortcuts("Book shortcuts");
    m_book.keyChooser = new BtShortcutsEditor(m_book.actionCollection, m_keyChooserStack);
    m_keyChooserStack->addWidget(m_book.keyChooser);
    BT_CONNECT(m_book.keyChooser,
               SIGNAL(keyChangeRequest(BtShortcutsEditor *, QString const &)),
               this,
               SLOT(completeKeyChangeRequest(BtShortcutsEditor *,
                                             QString const &)));

    mainLayout->addWidget(m_keyChooserStack);
    slotKeyChooserTypeChanged(m_application.title);

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

// complete the keyChangeRequest
void CAcceleratorSettingsPage::completeKeyChangeRequest(BtShortcutsEditor* shortcutsEditor, const QString& keys) {
    // check the BtShortcutsEditor's for shortcut conflicts
    // Either clear the conflicts and set the new shortcut or do nothing.

    QList<BtShortcutsEditor*> list = getShortcutsEditorListForGroup(shortcutsEditor);
    QString conflicts = findConflictsWithKeys(keys, list);
    if (!conflicts.isEmpty()) {
        QString message = QObject::tr("This shortcut conflicts with the shortcut for the following actions:");
        message.append("<br/><br/>");
        message.append(conflicts);

        if (message::showQuestion(this,
            QObject::tr("Do you want to clear the conflicting shortcuts and continue?"),
            message, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes) {
            clearConflictsWithKeys(keys, list);
            shortcutsEditor->changeShortcutInDialog(keys);
        }
    }
    else {
        shortcutsEditor->changeShortcutInDialog(keys);
    }
}

void CAcceleratorSettingsPage::clearConflictsWithKeys(const QString& keys, const QList<BtShortcutsEditor*> list) {
    QString conflicts;
    for (int i = 0; i < list.count(); i++) {
        list.at(i)->clearConflictWithKeys(keys);
    }
}

QString CAcceleratorSettingsPage::findConflictsWithKeys(const QString& keys, const QList<BtShortcutsEditor*> list) {
    QString conflicts;
    for (int i = 0; i < list.count(); i++) {
        QString conflict = list.at(i)->findConflictWithKeys(keys);
        if (!conflict.isEmpty()) {
            QString conflictMsg(tr("\n   \"%1\" in the \"%2\" group").arg(conflict).arg(getTitleForEditor(list.at(i))));
            conflicts.append(conflictMsg);
        }
    }
    return conflicts;
}

QString CAcceleratorSettingsPage::getTitleForEditor(BtShortcutsEditor* editor) {
    if (editor == m_application.keyChooser)
        return m_application.title;
    if (editor == m_general.keyChooser)
        return m_general.title;
    if (editor == m_bible.keyChooser)
        return m_bible.title;
    if (editor == m_commentary.keyChooser)
        return m_commentary.title;
    if (editor == m_lexicon.keyChooser)
        return m_lexicon.title;
    if (editor == m_book.keyChooser)
        return m_book.title;
    return QString();
}

// Gets list of shortcuts editors that can conflict with a key change in the current shortcut editor
QList<BtShortcutsEditor*> CAcceleratorSettingsPage::getShortcutsEditorListForGroup(BtShortcutsEditor* currentEditor) {
    QList<BtShortcutsEditor*> list;

    list.append(m_application.keyChooser);
    list.append(m_general.keyChooser);
    if (   (currentEditor == m_application.keyChooser) || (currentEditor == m_general.keyChooser) ) {
        list.append(m_bible.keyChooser);
        list.append(m_commentary.keyChooser);
        list.append(m_lexicon.keyChooser);
        list.append(m_book.keyChooser);
    }
    else {
        list.append(currentEditor);
    }
    return list;
}

void CAcceleratorSettingsPage::save() {
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

void CAcceleratorSettingsPage::slotKeyChooserTypeChanged(const QString& title) {
    Q_UNUSED(title); /// \todo Is this correct?

    int index = m_typeChooser->currentIndex();
    m_keyChooserStack->setCurrentIndex(index);

}
