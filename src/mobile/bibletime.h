#pragma once

#include <QObject>

namespace btm {

class BibleTime : public QObject {
    Q_OBJECT

public:
    BibleTime(QObject* parent = 0);

private:
    void initBackends();
    void initSwordConfigFile();

};

}
