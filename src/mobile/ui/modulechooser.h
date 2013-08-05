#ifndef MODULE_CHOOSER_H
#define MODULE_CHOOSER_H

#include <QObject>
#include <QList>

class QtQuick2ApplicationViewer;
class QQmlComponent;
class QQuickItem;
class QStringList;

namespace btm {

class GridChooser;

class BtWindowInterface;

class ModuleChooser : public QObject {
    Q_OBJECT

    enum State {
        CLOSED,
        MODULE
    };

public:
    ModuleChooser(QtQuick2ApplicationViewer* viewer, BtWindowInterface* bibleVerse);
    void open();
    QString moduleName() const;

private slots:
    void stringAccepted(const QString& value);
    void stringCanceled();

private:
    void showGridChooser(const QStringList& list);
    void setProperties(const QStringList& list);

    QtQuick2ApplicationViewer* viewer_;
    GridChooser* gridChooser_;
    int state_;
    QString moduleName_;
    BtWindowInterface* bibleVerse_;
};

} // end namespace

#endif
