/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2019 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

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

