/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "keynamechooser.h"

#include <QQuickItem>
#include "backend/keys/cswordldkey.h"
#include "backend/models/btmoduletextmodel.h"
#include "mobile/ui/btwindowinterface.h"
#include "mobile/ui/qtquick2applicationviewer.h"
#include "util/btassert.h"
#include "util/btconnect.h"


namespace btm {

KeyNameChooser::KeyNameChooser(QtQuick2ApplicationViewer* viewer,
                               BtWindowInterface* windowInterface)
    : m_viewer(viewer),
      m_windowInterface(windowInterface),
      m_key(nullptr),
      m_keyNameChooserObject(nullptr) {
    findKeyNameChooserObject();
}

void KeyNameChooser::open(BtModuleTextModel* model) {
    m_keyNameChooserObject->setProperty("model", QVariant::fromValue(model));
    openChooser(true);
}

void KeyNameChooser::openChooser(bool open) {
    BT_ASSERT(m_keyNameChooserObject);
    if (m_keyNameChooserObject == nullptr)
        return;

    m_keyNameChooserObject->disconnect();
    BT_CONNECT(m_keyNameChooserObject, SIGNAL(itemSelected(int)),
               this,                   SLOT(selected(int)));

    m_keyNameChooserObject->setProperty("visible",open);
}

void KeyNameChooser::selected(int index) {
    emit referenceChanged(index);
}

void KeyNameChooser::findKeyNameChooserObject() {
    QQuickItem * rootObject = m_viewer->rootObject();
    if (rootObject != nullptr)
        m_keyNameChooserObject = rootObject->findChild<QQuickItem*>("keyNameChooser");
    BT_ASSERT(m_keyNameChooserObject);
}



} // end namespace
