
#include <QObject>

class test_BtListModel : public QObject {
    Q_OBJECT

private slots:
    void initTestCase(); 

    void appendItem_data();
    void appendItem();

};

