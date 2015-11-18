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

#ifndef VIEW_MANAGER_H
#define VIEW_MANAGER_H

#include <QObject>
#include <QList>

class QtQuick2ApplicationViewer;
class QStringList;

namespace btm {
class BtWindowInterface;
}

namespace btm {

class ViewManager : public QObject {
    Q_OBJECT

public:
    ViewManager();

    void show();
    QtQuick2ApplicationViewer* getViewer() const;

private:
    void initialize_main_qml();
    void initialize_string_list_chooser_model();

    QtQuick2ApplicationViewer* viewer_;
};

} // end namespace

#endif
