/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "bibletime.h"

#include <QList>
#include <QMdiSubWindow>
#include "backend/keys/cswordversekey.h"
#include "frontend/cmdiarea.h"

// Sword includes:
#include <versekey.h>
#include <listkey.h>


//helper function
void BibleTime::syncAllModulesByType(const CSwordModuleInfo::ModuleType type, const QString& key) {
    Q_FOREACH (const QMdiSubWindow * const w, m_mdi->usableWindowList()) {
        CDisplayWindow * const d = dynamic_cast<CDisplayWindow*>(w->widget());
        if (d != 0 && !d->modules().isEmpty() && d->modules().first()->type() == type) {
            d->lookupKey(key);
        }
    }
}

void BibleTime::closeAllModuleWindows() {
    m_mdi->closeAllSubWindows();
}

QStringList BibleTime::searchInModule(const QString& moduleName, const QString& searchText) {
    QStringList ret;
    CSwordModuleInfo* mod = CSwordBackend::instance()->findModuleByName(moduleName);

    if (mod) {
        sword::ListKey result;

        //mod->search(searchText, CSwordModuleSearch::multipleWords, sword::ListKey());
        sword::ListKey scope;
        mod->searchIndexed(searchText, scope, result);

        const QString lead = QString("[%1] ").arg(moduleName);

        for ( int i = 0; i < result.Count(); ++i ) {
            sword::SWKey* key = result.getElement(i);
            Q_ASSERT(key);


            if (mod->type() == CSwordModuleInfo::Bible || mod->type() == CSwordModuleInfo::Commentary) {
                sword::VerseKey vk(key->getText());
                ret << lead + QString::fromUtf8( vk.getOSISRef() );
            }
            else {
                ret << lead + QString::fromUtf8( key->getText() );
            }
        }
    }
    return ret;
}

QStringList BibleTime::searchInOpenModules(const QString& searchText) {
    QStringList ret;
    Q_FOREACH (const QMdiSubWindow * const subWindow, m_mdi->subWindowList()) {
        const CDisplayWindow * const w = dynamic_cast<CDisplayWindow*>(subWindow->widget());
        if (w != 0) {
            Q_FOREACH (const CSwordModuleInfo * const mi, w->modules()) {
                ret += searchInModule(mi->name(), searchText);
            }
        }
    }
    return ret;
}

QString BibleTime::getCurrentReference() {
    QString ret = QString::null;

    QMdiSubWindow* activeSubWindow = m_mdi->activeSubWindow();
    if (!activeSubWindow) return ret;

    CDisplayWindow* w = dynamic_cast<CDisplayWindow*>(activeSubWindow->widget());

    if (w) {
        QString modType;
        Q_ASSERT(w->modules().first());
        switch (w->modules().first()->type()) {
            case CSwordModuleInfo::Bible:
                modType = "BIBLE";
                break;
            case CSwordModuleInfo::Commentary:
                modType = "COMMENTARY";
                break;
            case CSwordModuleInfo::GenericBook:
                modType = "BOOK";
                break;
            case CSwordModuleInfo::Lexicon:
                modType = "LEXICON";
                break;
            default:
                modType = "UNSUPPORTED";
                break;
        }

        ret.append("[").append(w->modules().first()->name()).append("] ");
        ret.append("[").append(modType).append("] ");

        CSwordVerseKey* vk = dynamic_cast<CSwordVerseKey*>( w->key() );
        if (vk) {
            ret.append( vk->getOSISRef() );
        }
        else {
            ret.append( w->key()->key() );
        }
    }

    return ret;
}

QStringList BibleTime::getModulesOfType(const QString& type) {
    QStringList ret;
    CSwordModuleInfo::ModuleType modType;

    if (type == "BIBLES") {
        modType = CSwordModuleInfo::Bible;
    }
    else if (type == "COMMENTARIES") {
        modType = CSwordModuleInfo::Commentary;
    }
    else if (type == "LEXICONS") {
        modType = CSwordModuleInfo::Lexicon;
    }
    else if (type == "BOOKS") {
        modType = CSwordModuleInfo::GenericBook;
    }
    else {
        modType = CSwordModuleInfo::Unknown;
    }

    Q_FOREACH(const CSwordModuleInfo * const mi, CSwordBackend::instance()->moduleList()) {
        if (mi->type() == modType) {
            ret.append(mi->name());
        }
    }

    return ret;
}

