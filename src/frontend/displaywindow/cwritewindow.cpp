/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/


#include "frontend/displaywindow/cwritewindow.h"

#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordversekey.h"
#include "frontend/display/cwritedisplay.h"
#include "frontend/keychooser/ckeychooser.h"
#include "frontend/profile/cprofilewindow.h"
#include "util/dialogutil.h"
#include "util/btsignal.h"

using namespace Profile;

CWriteWindow::CWriteWindow(QList<CSwordModuleInfo*> modules, CMDIArea* parent)
        : CDisplayWindow(modules, parent), m_writeDisplay(0) {}

void CWriteWindow::insertKeyboardActions( BtActionCollection* const ) {}

void CWriteWindow::initConnections() {
    Q_ASSERT(keyChooser());
    QObject::connect(key()->signaler(), SIGNAL(beforeChanged()), this, SLOT(beforeKeyChange()));
}

void CWriteWindow::initActions() {}


void CWriteWindow::storeProfileSettings(CProfileWindow * const settings) {

    settings->writeWindowType = writeWindowType();
    settings->windowGeometry.setRect(parentWidget()->x(),
                                     parentWidget()->y(),
                                     parentWidget()->width(),
                                     parentWidget()->height());

    // settings->setScrollbarPositions( m_htmlWidget->view()->horizontalScrollBar()->value(), m_htmlWidget->view()->verticalScrollBar()->value() );
    settings->type = modules().first()->type();
    settings->maximized = isMaximized() || parentWidget()->isMaximized();

    if (key()) {
        sword::VerseKey* vk = dynamic_cast<sword::VerseKey*>(key());
        QString oldLang;
        if (vk) {
            oldLang = QString::fromLatin1(vk->getLocale());
            vk->setLocale("en"); //save english locale names as default!
        }
        settings->key = key()->key();
        if (vk) {
            vk->setLocale(oldLang.toLatin1());
        }
    }

    QStringList mods;
    Q_FOREACH(const CSwordModuleInfo *m, modules()) {
        mods.append(m->name());
    }
    settings->modules = mods;
}

void CWriteWindow::applyProfileSettings(CProfileWindow * const settings) {
    setUpdatesEnabled(false);

    if (settings->maximized) {
        parentWidget()->showMaximized();
    }
    else {
        const QRect &rect = settings->windowGeometry;
        parentWidget()->resize(rect.width(), rect.height());
        parentWidget()->move(rect.x(), rect.y());
        //setGeometry( settings->geometry() );
    }
    // displayWidget()->view()->horizontalScrollBar()->setValue( settings->scrollbarPositions().horizontal );
    // m_htmlWidget->view()->verticalScrollBar()->setValue( settings->scrollbarPositions().vertical );

    setUpdatesEnabled(true);
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

