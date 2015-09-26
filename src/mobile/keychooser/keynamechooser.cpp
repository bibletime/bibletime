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

#include "backend/keys/cswordldkey.h"
#include "backend/models/btmoduletextmodel.h"
#include "mobile/ui/btwindowinterface.h"
#include "mobile/ui/qtquick2applicationviewer.h"
#include <QQuickItem>

namespace btm {

KeyNameChooser::KeyNameChooser(QtQuick2ApplicationViewer* viewer,
                               BtWindowInterface* windowInterface)
    : m_viewer(viewer),
      m_windowInterface(windowInterface),
      m_key(0),
      m_keyNameChooserObject(0) {
    findKeyNameChooserObject();
}

void KeyNameChooser::open(BtModuleTextModel* model) {
    m_keyNameChooserObject->setProperty("model", QVariant::fromValue(model));
    openChooser(true);
}

void KeyNameChooser::openChooser(bool open) {
    Q_ASSERT(m_keyNameChooserObject != 0);
    if (m_keyNameChooserObject == 0)
        return;

    m_keyNameChooserObject->disconnect();
    bool ok = connect(m_keyNameChooserObject, SIGNAL(itemSelected(int)),
                      this, SLOT(selected(int)));
    Q_ASSERT(ok);

    m_keyNameChooserObject->setProperty("visible",open);
}

void KeyNameChooser::selected(int index) {
    emit referenceChanged(index);
}

void KeyNameChooser::findKeyNameChooserObject() {
    QQuickItem * rootObject = m_viewer->rootObject();
    if (rootObject != 0)
        m_keyNameChooserObject = rootObject->findChild<QQuickItem*>("keyNameChooser");
    Q_ASSERT(m_keyNameChooserObject != 0);
}



} // end namespace
