/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/


#include "frontend/displaywindow/cwritewindow.h"

#include "backend/config/btconfig.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordversekey.h"
#include "frontend/display/cwritedisplay.h"
#include "frontend/keychooser/ckeychooser.h"
#include "util/dialogutil.h"
#include "util/btsignal.h"


CWriteWindow::CWriteWindow(QList<CSwordModuleInfo*> modules, CMDIArea* parent)
        : CDisplayWindow(modules, parent), m_writeDisplay(0) {}

void CWriteWindow::insertKeyboardActions( BtActionCollection* const ) {}

void CWriteWindow::initConnections() {
    Q_ASSERT(keyChooser());
    QObject::connect(key()->signaler(), SIGNAL(beforeChanged()), this, SLOT(beforeKeyChange()));
}

void CWriteWindow::initActions() {}


void CWriteWindow::storeProfileSettings(const QString & windowGroup) {
    CDisplayWindow::storeProfileSettings(windowGroup);

    Q_ASSERT(windowGroup.endsWith('/'));
    btConfig().setSessionValue(windowGroup + "writeWindowType",
                               static_cast<int>(writeWindowType()));
}

void CWriteWindow::setDisplayWidget( CDisplay* display ) {
    Q_ASSERT(dynamic_cast<CWriteDisplay*>(display));
    CDisplayWindow::setDisplayWidget((CWriteDisplay*)display);
    m_writeDisplay = (CWriteDisplay*)display;
}

void CWriteWindow::lookupSwordKey( CSwordKey* newKey ) {
    //set the raw text to the display widget
    if (!newKey)
        return;

    if (key() != newKey) { //set passage of newKey to key() if they're different, otherwise we'd get mixed up if we look up newkey which may have a different module set
        key()->setKey(newKey->key());
    }

    if ( modules().count() ) {
        displayWidget()->setText( key()->rawText() );
    }
    setWindowTitle(windowCaption());
}

bool CWriteWindow::queryClose() {
    //save the text if it has changed
    if (m_writeDisplay->isModified()) {
        switch (util::showQuestion( this, tr("Save Text?"), tr("Save text before closing?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes) ) {
            case QMessageBox::Yes: //save and close
                saveCurrentText();
                m_writeDisplay->setModified( false );
                return true;
            case QMessageBox::No: //don't save and close
                return true;
            default: // cancel, don't close
                return false;
        }
    }
    return true;
}

void CWriteWindow::beforeKeyChange() {
    Q_ASSERT(displayWidget());
    Q_ASSERT(keyChooser());
    if (!isReady())
        return;

    // Get current key string for this window
    QString thisWindowsKey;
    CSwordKey* oldKey = key();
    if (oldKey == 0)
        return;
    thisWindowsKey = oldKey->key();
    
    //If the text changed and we'd do a lookup ask the user if the text should be saved
    if (modules().first() && ((CWriteDisplay*)displayWidget())->isModified()) {

        switch (util::showQuestion( this, tr("Save Text?"), tr("Save changed text?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) ) {
            case QMessageBox::Yes: { //save the changes
                saveCurrentText( thisWindowsKey );
                break;
            }
            default: {// set modified to false so it won't ask again
                ((CWriteDisplay*)displayWidget())->setModified(false);
                break;
            }
        }
    }
}

void CWriteWindow::saveCurrentText() {
    if (key()) {
        saveCurrentText(key()->key());
    }
}

