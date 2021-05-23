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

#include "bttextbrowser.h"

#include <memory>
#include <QApplication>
#include <QDrag>
#include <QMouseEvent>
#include "../backend/keys/cswordkey.h"
#include "../backend/managers/referencemanager.h"
#include "../backend/managers/cswordbackend.h"
#include "BtMimeData.h"


BtTextBrowser::BtTextBrowser(QWidget *parent)
    : QTextBrowser(parent),
      m_isDragging(false),
      m_mousePressed(false)
{
    setTextInteractionFlags(Qt::TextSelectableByMouse);
}

void BtTextBrowser::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_mousePressed = true;
        m_isDragging = false;
        m_dragUrl = anchorAt(event->pos());
        m_startPos = QPoint(event->x(), event->y());
    }
    QTextBrowser::mousePressEvent(event);
}

void BtTextBrowser::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_mousePressed = false;
    }
}

void BtTextBrowser::mouseMoveEvent(QMouseEvent *event) {
    // If we have not started dragging, and the left mouse button is down, start the drag
    if (!m_isDragging && m_mousePressed) {
        QPoint current(event->x(), event->y());
        if ((current - m_startPos).manhattanLength() > qApp->startDragDistance()) {
            QString moduleName;
            QString keyName;
            ReferenceManager::Type type;
            if ( !ReferenceManager::decodeHyperlink(m_dragUrl, moduleName, keyName, type) ) {
                QTextBrowser::mouseMoveEvent(event);
                return;
            }
            QDrag* drag = new QDrag(this);
            BTMimeData* mimedata = new BTMimeData(moduleName, keyName, QString());
            drag->setMimeData(mimedata);

            //add real Bible text from module/key
            if (CSwordModuleInfo *module = CSwordBackend::instance()->findModuleByName(moduleName)) {
                std::unique_ptr<CSwordKey> key(CSwordKey::createInstance(module));
                key->setKey(keyName);
                mimedata->setText(key->strippedText());
            }

            m_isDragging = true;
            drag->exec(Qt::CopyAction, Qt::CopyAction);
            m_mousePressed = false;
            return;
        }
    }
    QTextBrowser::mouseMoveEvent(event);
}
