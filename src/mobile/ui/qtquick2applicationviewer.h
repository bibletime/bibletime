/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

// checksum 0xfde6 version 0x80001
/*
  This file was generated by the Qt Quick 2 Application wizard of Qt Creator.
  QtQuick2ApplicationViewer is a convenience class containing mobile device specific
  code such as screen orientation handling. Also QML paths and debugging are
  handled here.
  It is recommended not to modify this file, since newer versions of Qt Creator
  may offer an updated version of it.
*/

#ifndef QTQUICK2APPLICATIONVIEWER_H
#define QTQUICK2APPLICATIONVIEWER_H

#include <QtQuick/QQuickView>

class QtQuick2ApplicationViewer : public QQuickView {
    Q_OBJECT

public:
    explicit QtQuick2ApplicationViewer(QWindow *parent = nullptr);
    virtual ~QtQuick2ApplicationViewer();

    void setMainQmlFile(const QString &file);
    void addImportPath(const QString &path);

    void showExpanded();

private:
    class QtQuick2ApplicationViewerPrivate *d;
};

#endif // QTQUICK2APPLICATIONVIEWER_H