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

#ifndef GRID_CHOOSER_H
#define GRID_CHOOSER_H

#include <QEventLoop>
#include <QObject>
#include <QList>

class QtQuick2ApplicationViewer;
class QQuickItem;
class QStringList;
class QEventLoop;

namespace btm {

class BtWindowInterface;

class GridChooser : public QObject {
    Q_OBJECT

public:
    GridChooser(QtQuick2ApplicationViewer* viewer);
    ~GridChooser();
    void open(const QStringList& stringList, const QString& highlight = QString(), const QString& title = QString());

signals:
    void accepted(const QString& value);
    void canceled();

private slots:
    void gridChooserAccepted(QString value);
    void gridChooserCanceled();

private:
    void setProperties(const QStringList& list, const QString& hightlight, const QString& title);

    QtQuick2ApplicationViewer* viewer_;
    QQuickItem* gridChooserObject_;
    QEventLoop eventLoop_;
};

} // end namespace

#endif
