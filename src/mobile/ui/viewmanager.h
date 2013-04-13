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
    void showBibleVerseDialog(BtWindowInterface* verse);
    QtQuick2ApplicationViewer* getViewer() const;

private:
    void initialize_main_qml();
    void initialize_string_list_chooser_model();

    QtQuick2ApplicationViewer* viewer_;
};

} // end namespace

#endif
