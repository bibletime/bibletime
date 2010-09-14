/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/keychooser/ckeychooser.h"

#include <QAction>
#include <QDebug>
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/drivers/cswordcommentarymoduleinfo.h"
#include "backend/drivers/cswordlexiconmoduleinfo.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordkey.h"
#include "frontend/keychooser/bthistory.h"
#include "frontend/keychooser/cbookkeychooser.h"
#include "frontend/keychooser/clexiconkeychooser.h"
#include "frontend/keychooser/versekeychooser/cbiblekeychooser.h"


CKeyChooser::CKeyChooser(QList<CSwordModuleInfo*>, CSwordKey *, QWidget *parent)
        : QWidget(parent),
        m_history(0) {
    //qDebug() << "CKeyChooser::CKeyChooser";
    m_history = new BTHistory(this);
    QObject::connect(history(), SIGNAL(historyMoved(QString&)), this, SLOT(setKey(QString&)));
}

CKeyChooser::~CKeyChooser() {}

CKeyChooser* CKeyChooser::createInstance(QList<CSwordModuleInfo*> modules, CSwordKey *key, QWidget *parent) {
    if (!modules.count()) {
        return 0;
    }

    switch ( modules.first()->type() ) {
        case CSwordModuleInfo::Commentary:  //Bibles and commentaries use the same key chooser
        case CSwordModuleInfo::Bible:
            return new CBibleKeyChooser(modules, key, parent);
            break;
        case CSwordModuleInfo::Lexicon:
            return new CLexiconKeyChooser(modules, key, parent);
        case CSwordModuleInfo::GenericBook:
            return new CBookKeyChooser(modules, key, parent);
        default:
            return 0;
    }
}


BTHistory* CKeyChooser::history() {
    return m_history;
}

