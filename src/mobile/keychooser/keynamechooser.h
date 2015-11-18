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

#ifndef KEY_NAME_CHOOSER_H
#define KEY_NAME_CHOOSER_H

#include <QObject>

class CSwordLDKey;
class QtQuick2ApplicationViewer;
class QQmlComponent;
class QQuickItem;
class BtModuleTextModel;

/**
    \brief Handles signals and properties of KeyNameChooser.qml

    The KeyNameChooser.qml object is in the main.qml. It need to
    be called from a lexicon Window.qml. This module assists this
    process by setting the visibility and model to open the KEY_NAME_CHOOSER
    and then catches the signals to finish the choice and close the chooser.
 */


namespace btm {

class BtWindowInterface;

class KeyNameChooser : public QObject {
    Q_OBJECT

public:
    KeyNameChooser(QtQuick2ApplicationViewer* viewer,
                   BtWindowInterface* windowInterface);
    void open(BtModuleTextModel* model);

signals:
    void referenceChanged(int index);

private slots:
    void selected(int index);

private:
    void findKeyNameChooserObject();
    void openChooser(bool open);
    void setProperties();

    QtQuick2ApplicationViewer* m_viewer;
    BtWindowInterface* m_windowInterface;
    CSwordLDKey* m_key;
    QQuickItem* m_keyNameChooserObject;

};

} // end namespace

#endif
