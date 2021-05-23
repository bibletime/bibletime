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
    : QTextBrowser(parent)
{
    setTextInteractionFlags(Qt::TextSelectableByMouse);
}

void BtTextBrowser::keyPressEvent(QKeyEvent * event) {
    QTextBrowser::keyPressEvent(event);
    if (event->isAccepted())
        m_readyToStartDrag = false;
}

void BtTextBrowser::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() == Qt::LeftButton) {
        m_startPos = event->pos();
        m_readyToStartDrag = true;
    } else {
        m_readyToStartDrag = false;
    }
    QTextBrowser::mousePressEvent(event);
}

void BtTextBrowser::mouseReleaseEvent(QMouseEvent *event) {
    m_readyToStartDrag = false;
    QTextBrowser::mouseReleaseEvent(event);
}

void BtTextBrowser::mouseMoveEvent(QMouseEvent *event) {
    // If we have not started dragging, and the left mouse button is down, start the drag
    if (m_readyToStartDrag) {
        auto const dragManhattanLength =
                (event->pos() - m_startPos).manhattanLength();
        if (dragManhattanLength >= qApp->startDragDistance()) {
            if (auto const decodedLink =
                        ReferenceManager::decodeHyperlink(anchorAt(m_startPos)))
            {
                QDrag* drag = new QDrag(this);
                BTMimeData* mimedata = new BTMimeData(decodedLink->module->name(), decodedLink->key, QString());
                drag->setMimeData(mimedata);

                //add real Bible text from module/key
                if (CSwordModuleInfo *module = CSwordBackend::instance()->findModuleByName(decodedLink->module->name())) {
                    std::unique_ptr<CSwordKey> key(CSwordKey::createInstance(module));
                    key->setKey(decodedLink->key);
                    mimedata->setText(key->strippedText());
                }

                drag->exec(Qt::CopyAction, Qt::CopyAction);
                m_readyToStartDrag = false;
                return;
            }
        }
    }
    QTextBrowser::mouseMoveEvent(event);
}
