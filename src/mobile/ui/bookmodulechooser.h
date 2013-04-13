#ifndef BOOK_MODULE_CHOOSER_H
#define BOOK_MODULE_CHOOSER_H

#include <QObject>
#include <QList>

class QtQuick2ApplicationViewer;
class QQmlComponent;
class QQuickItem;
class QStringList;

namespace btm {

class GridChooser;

class BtBookInterface;

class BookModuleChooser : public QObject {
    Q_OBJECT

    enum State {
        CLOSED,
        MODULE
    };

public:
    BookModuleChooser(QtQuick2ApplicationViewer* viewer, BtBookInterface* bibleVerse);
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
    BtBookInterface* bookInterface_;
};

} // end namespace

#endif
