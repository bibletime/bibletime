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

#ifndef MODULE_CHOOSER_H
#define MODULE_CHOOSER_H

#include <QObject>

class QtQuick2ApplicationViewer;
class QStringList;

namespace btm {

class GridChooser;

class BtWindowInterface;

class ModuleChooser : public QObject {
    Q_OBJECT

public:
    ModuleChooser(QtQuick2ApplicationViewer* viewer, BtWindowInterface* bibleVerse);
    void open();

private slots:
    void moduleSelectedSlot();

private:
    void showGridChooser(const QStringList& list);
    void setProperties(const QStringList& list);

    QtQuick2ApplicationViewer* viewer_;
    BtWindowInterface* bibleVerse_;
};

} // end namespace

#endif
