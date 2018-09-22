/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "frontend/display/bthtmljsobject.h"

#include <memory>
#include <QDrag>
#include "backend/config/btconfig.h"
#include "backend/keys/cswordkey.h"
#include "backend/managers/referencemanager.h"
#include "backend/managers/cswordbackend.h"
#include "frontend/bibletime.h"
#include "frontend/BtMimeData.h"
#include "frontend/cinfodisplay.h"
#include "frontend/display/bthtmlreaddisplay.h"


using namespace InfoDisplay;

// This class works along with the BtHtmlReadDisplay class. The BtHtmlReadDisplay class loads
// Javascript (bthtml.js) along with the html it displays. This class is added to the Javascript model
// so that Javascript can call this class, or this class can call Javascript.
// Access to DOM objects is implemented in Javascript and is communicated back to c++ through this class

BtHtmlJsObject::BtHtmlJsObject(BtHtmlReadDisplay* display)
        : m_display(display)
{
    m_dndData.isDragging = false;
    m_dndData.mousePressed = false;
}

void BtHtmlJsObject::moveToAnchor(const QString& anchor) {
    // Call gotoAnchor in Javascript
    emit gotoAnchor(anchor);
}

void BtHtmlJsObject::mouseDownLeft(const QString& url, const int& x, const int& y) {
    m_dndData.mousePressed = true;
    m_dndData.isDragging = false;
    m_dndData.startPos = QPoint(x, y);
    m_dndData.url = url;
}

void BtHtmlJsObject::mouseClick(const QString& url) {
    m_dndData.mousePressed = false;
    if (!url.isEmpty() && ReferenceManager::isHyperlink(url)) {
        QString module;
        QString key;
        ReferenceManager::Type type;

        ReferenceManager::decodeHyperlink(url, module, key, type);
        if (module.isEmpty()) {
            module = ReferenceManager::preferredModule( type );
        }
        m_display->connectionsProxy()->emitReferenceClicked(module, key);
    }
}

void BtHtmlJsObject::mouseDownRight(const QString& url, const QString& lemma) {
    m_display->setActiveAnchor(url);
    if (lemma.isEmpty())
        m_display->setLemma(QString::null);
    else
        m_display->setLemma(lemma);
}

// The mouse move event starts in the javascript function "mouseMoveHandler" in bthtml.js. It calls this function
void BtHtmlJsObject::mouseMoveEvent(const QString& attributes, const int& x, const int& y, const bool& shiftKey) {
    if (!m_dndData.isDragging && m_dndData.mousePressed) {
        // If we have not started dragging, but the mouse button is down, create a the mime data
        QPoint current(x, y);
        if ((current - m_dndData.startPos).manhattanLength() > 4 /*qApp->startDragDistance()*/ ) {
            QDrag* drag = nullptr;
            if (!m_dndData.url.isEmpty()) {
                // create a new bookmark drag!
                QString moduleName = QString::null;
                QString keyName = QString::null;
                ReferenceManager::Type type;
                if ( !ReferenceManager::decodeHyperlink(m_dndData.url, moduleName, keyName, type) )
                    return;
                drag = new QDrag(m_display->view());
                BTMimeData* mimedata = new BTMimeData(moduleName, keyName, QString::null);
                drag->setMimeData(mimedata);
                //add real Bible text from module/key
                if (CSwordModuleInfo *module = CSwordBackend::instance()->findModuleByName(moduleName)) {
                    std::unique_ptr<CSwordKey> key(CSwordKey::createInstance(module));
                    key->setKey(keyName);
                    mimedata->setText(key->strippedText()); // This works across applications!
                }
            }
            if (drag) {
                m_dndData.isDragging = true;
                m_dndData.mousePressed = false;
                drag->exec(Qt::CopyAction, Qt::CopyAction);
            }
        }
    }
    else if (m_display->getMouseTracking() &&  !shiftKey) {
        // no mouse button pressed and tracking enabled
        // start timer that updates the mag window
        // Sets timer in javascript. See bthtml.js startTimer()
        emit startTimer(btConfig().value<int>("GUI/magDelay", 400));
        m_prev_attributes = attributes;
        // When the javascript timer interupts, the see timerEvent() in bthtml.js
        // will call the timeOutEvent in this class
    }
}

// called from javascript timerEvent() in bthtml.js
void BtHtmlJsObject::timeOutEvent(const QString & attributes) {
    m_prev_attributes = "";

    Rendering::ListInfoData infoList(Rendering::detectInfo(attributes));

    // Update the mag if valid attributes were found
    if (!(infoList.isEmpty()))
        BibleTime::instance()->infoDisplay()->setInfo(infoList);
}

// clearing the previous attribute effectively stops any time out event
// that is in progress
void BtHtmlJsObject::clearPrevAttribute() {
    m_prev_attributes = "";
}

