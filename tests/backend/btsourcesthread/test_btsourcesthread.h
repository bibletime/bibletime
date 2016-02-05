
#include <QtTest/QtTest>

class BtSourcesThread;
class QEventLoop;

class test_BtSourcesThread : public QObject
{
    Q_OBJECT
public:

    // Public slots can be used as part of a test.
    // They are not automatically ran.
public slots:
    void slotThreadFinished();
    void slotShowMessage(const QString & msg);

    // Private slots are automatically ran as tests.
private slots:
    void initTestCase();
    void installRemoteSources();
    void cleanupTestCase();

private:
    BtSourcesThread * m_thread;
    QEventLoop * m_eventLoop;
};

