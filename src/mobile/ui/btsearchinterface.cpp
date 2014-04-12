/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btsearchinterface.h"

//#include "backend/config/btconfig.h"
//#include "backend/drivers/cswordbiblemoduleinfo.h"
//#include "backend/drivers/cswordbookmoduleinfo.h"
//#include "backend/drivers/cswordlexiconmoduleinfo.h"
//#include "backend/drivers/cswordmoduleinfo.h"
//#include "backend/keys/cswordkey.h"
//#include "backend/keys/cswordtreekey.h"
//#include "backend/managers/cswordbackend.h"
//#include "backend/models/btmoduletextmodel.h"
//#include "backend/rendering/centrydisplay.h"
//#include "backend/rendering/cdisplayrendering.h"
//#include "mobile/btmmain.h"
//#include "mobile/keychooser/bookkeychooser.h"
//#include "mobile/keychooser/keynamechooser.h"
//#include "mobile/keychooser/versechooser.h"
//#include "mobile/ui/modulechooser.h"
//#include "mobile/ui/viewmanager.h"
//#include <QDebug>
//#include <QFile>
//#include <QObject>
//#include <QQmlContext>
//#include <QQmlEngine>
//#include <QQuickItem>
//#include <QStringList>


namespace btm {

BtSearchInterface::BtSearchInterface(QObject* parent)
    : QObject(parent) {
}

BtSearchInterface::~BtSearchInterface() {
}

bool BtSearchInterface::performSearch() {
    return true;
}

QString BtSearchInterface::getSearchText() const {
    return m_searchText;
}

QString BtSearchInterface::getFindChoice() const {
    return m_findChoice;
}

QString BtSearchInterface::getModuleList() const {
    return m_moduleList;
}

void BtSearchInterface::setSearchText(const QString& searchText) {
    m_searchText = searchText;
}

void BtSearchInterface::setFindChoice(const QString& findChoice) {
    m_findChoice = findChoice;
}

void BtSearchInterface::setModuleList(const QString& moduleList) {
    m_moduleList = moduleList;
}

} // end namespace
