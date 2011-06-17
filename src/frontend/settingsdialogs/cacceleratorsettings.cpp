/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/settingsdialogs/cacceleratorsettings.h"

#include <QComboBox>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QMessageBox>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>
#include "bibletime.h"
#include "backend/config/cbtconfig.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/displaywindow/cbiblereadwindow.h"
#include "frontend/displaywindow/cbookreadwindow.h"
#include "frontend/displaywindow/ccommentaryreadwindow.h"
#include "frontend/displaywindow/clexiconreadwindow.h"
#include "frontend/displaywindow/creadwindow.h"
#include "frontend/settingsdialogs/cconfigurationdialog.h"
#include "util/cresmgr.h"
#include "util/directory.h"


CAcceleratorSettingsPage::CAcceleratorSettingsPage(CConfigurationDialog *parent)
        : BtConfigDialog::Page(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout* layoutForWindowTypeChooser = new QHBoxLayout();
    mainLayout->addLayout(layoutForWindowTypeChooser);

    QLabel* label = new QLabel(tr("Choose action group:"), this);
    layoutForWindowTypeChooser->addWidget(label);

    m_typeChooser = new QComboBox(this);
    layoutForWindowTypeChooser->addWidget(m_typeChooser);

    bool ok = connect(m_typeChooser, SIGNAL(activated(const QString&)),
                      SLOT(slotKeyChooserTypeChanged(const QString&)) );
    Q_ASSERT(ok);

    m_keyChooserStack = new QStackedWidget(this);

    m_application.title = tr("Main Window");
    m_general = WindowType(tr("All text windows"));
    m_bible = WindowType(tr("Bible windows"));
    m_commentary = WindowType(tr("Commentary windows"));
    m_lexicon = WindowType(tr("Lexicon windows"));
    m_book = WindowType(tr("Book windows"));

    m_typeChooser->addItem(m_application.title);
    m_typeChooser->addItem(m_general.title);
    m_typeChooser->addItem(m_bible.title);
    m_typeChooser->addItem(m_commentary.title);
    m_typeChooser->addItem(m_lexicon.title);
    m_typeChooser->addItem(m_book.title);

    // create shortcuteditors

    // ------ Application -------------- //
    m_application.actionCollection = new BtActionCollection(this);
    BibleTime::insertKeyboardActions( m_application.actionCollection);
    CBTConfig::setupAccelSettings(CBTConfig::application, m_application.actionCollection);
    m_application.keyChooser = new BtShortcutsEditor(m_application.actionCollection, m_keyChooserStack);
    m_keyChooserStack->addWidget(m_application.keyChooser);
    ok = connect(m_application.keyChooser, SIGNAL(keyChangeRequest(BtShortcutsEditor*, const QString&)),
                 this, SLOT(completeKeyChangeRequest(BtShortcutsEditor*, const QString&)));
    Q_ASSERT(ok);

    // ----- All display windows ------ //
    m_general.actionCollection = new BtActionCollection(this);
    CDisplayWindow::insertKeyboardActions( m_general.actionCollection);
    CBTConfig::setupAccelSettings(CBTConfig::allWindows, m_general.actionCollection);
    m_general.keyChooser = new BtShortcutsEditor(m_general.actionCollection, m_keyChooserStack);
    m_keyChooserStack->addWidget(m_general.keyChooser);
    ok = connect(m_general.keyChooser, SIGNAL(keyChangeRequest(BtShortcutsEditor*, const QString&)),
                 this, SLOT(completeKeyChangeRequest(BtShortcutsEditor*, const QString&)));
    Q_ASSERT(ok);

    // ----- Bible windows ------ //
    m_bible.actionCollection = new BtActionCollection(this);
    CBibleReadWindow::insertKeyboardActions( m_bible.actionCollection);
    CBTConfig::setupAccelSettings(CBTConfig::bibleWindow, m_bible.actionCollection);
    m_bible.keyChooser = new BtShortcutsEditor(m_bible.actionCollection, m_keyChooserStack);
    m_keyChooserStack->addWidget(m_bible.keyChooser);
    ok = connect(m_bible.keyChooser, SIGNAL(keyChangeRequest(BtShortcutsEditor*, const QString&)),
                 this, SLOT(completeKeyChangeRequest(BtShortcutsEditor*, const QString&)));
    Q_ASSERT(ok);

    // ----- Commentary windows ------ //
    m_commentary.actionCollection = new BtActionCollection(this);
    CCommentaryReadWindow::insertKeyboardActions( m_commentary.actionCollection);
    CBTConfig::setupAccelSettings(CBTConfig::commentaryWindow,    m_commentary.actionCollection);
    m_commentary.keyChooser = new BtShortcutsEditor(m_commentary.actionCollection, m_keyChooserStack);
    m_keyChooserStack->addWidget(m_commentary.keyChooser);
    ok = connect(m_commentary.keyChooser, SIGNAL(keyChangeRequest(BtShortcutsEditor*, const QString&)),
                 this, SLOT(completeKeyChangeRequest(BtShortcutsEditor*, const QString&)));
    Q_ASSERT(ok);

    // ----- Lexicon windows ------ //
    m_lexicon.actionCollection = new BtActionCollection(this);
    CLexiconReadWindow::insertKeyboardActions(  m_lexicon.actionCollection );
    CBTConfig::setupAccelSettings(CBTConfig::lexiconWindow, m_lexicon.actionCollection);
    m_lexicon.keyChooser = new BtShortcutsEditor(m_lexicon.actionCollection, m_keyChooserStack                                         );
    m_keyChooserStack->addWidget(m_lexicon.keyChooser);
    ok = connect(m_lexicon.keyChooser, SIGNAL(keyChangeRequest(BtShortcutsEditor*, const QString&)),
                 this, SLOT(completeKeyChangeRequest(BtShortcutsEditor*, const QString&)));
    Q_ASSERT(ok);

    // ----- Book windows ------ //
    m_book.actionCollection = new BtActionCollection(this);
    CBookReadWindow::insertKeyboardActions( m_book.actionCollection);
    CBTConfig::setupAccelSettings(CBTConfig::bookWindow, m_book.actionCollection);
    m_book.keyChooser = new BtShortcutsEditor(m_book.actionCollection, m_keyChooserStack);
    m_keyChooserStack->addWidget(m_book.keyChooser);
    ok = connect(m_book.keyChooser, SIGNAL(keyChangeRequest(BtShortcutsEditor*, const QString&)),
                 this, SLOT(completeKeyChangeRequest(BtShortcutsEditor*, const QString&)));
    Q_ASSERT(ok);

    mainLayout->addWidget(m_keyChooserStack);
    slotKeyChooserTypeChanged(m_application.title);

    m_typeChooser->setFocus(Qt::MouseFocusReason);
    qDebug() << "CAcceleratorSettingsPage::CAcceleratorSettingsPage end";
}

CAcceleratorSettingsPage::~CAcceleratorSettingsPage() {
}

// complete the keyChangeRequest
void CAcceleratorSettingsPage::completeKeyChangeRequest(BtShortcutsEditor* shortcutsEditor, const QString& keys) {
    // check the BtShortcutsEditor's for shortcut conflicts
    // Either clear the conflicts and set the new shortcut or do nothing.

    QList<BtShortcutsEditor*> list = getShortcutsEditorListForGroup(shortcutsEditor);
    QString conflicts = findConflictsWithKeys(keys, list);
    if (!conflicts.isEmpty()) {
        QString message = QObject::tr("This shortcut conflicts with the shortcut for the following actions:");
        message.append("\n");
        message.append(conflicts);

        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText(message);
        msgBox.setInformativeText(QObject::tr("Do you want to clear the conflicting shortcuts and continue?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        if ( ret == QMessageBox::Yes) {
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
    if (editor == m_application.keyChooser)
        return m_lexicon.title;
    if (editor == m_book.keyChooser)
        return m_book.title;
    return QString::null;
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

    CBTConfig::saveAccelSettings(CBTConfig::application, m_application.actionCollection);        //application
    CBTConfig::saveAccelSettings(CBTConfig::allWindows, m_general.actionCollection);            //read display windows
    CBTConfig::saveAccelSettings(CBTConfig::bibleWindow, m_bible.actionCollection);                //bible
    CBTConfig::saveAccelSettings(CBTConfig::commentaryWindow, m_commentary.actionCollection);    //commentary
    CBTConfig::saveAccelSettings(CBTConfig::lexiconWindow, m_lexicon.actionCollection);            //lexicon
    CBTConfig::saveAccelSettings(CBTConfig::bookWindow,    m_book.actionCollection);                //book
}

void CAcceleratorSettingsPage::slotKeyChooserTypeChanged(const QString& title) {
    Q_UNUSED(title); /// \todo Is this correct?

    int index = m_typeChooser->currentIndex();
    m_keyChooserStack->setCurrentIndex(index);

}

const QIcon &CAcceleratorSettingsPage::icon() const {
    return util::directory::getIcon(CResMgr::settings::keys::icon);
}

QString CAcceleratorSettingsPage::header() const {
    return tr("Shortcuts");
}
