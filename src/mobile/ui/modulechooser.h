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
