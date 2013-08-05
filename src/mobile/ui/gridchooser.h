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
