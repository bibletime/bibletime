/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTSOURCESTHREAD_H
#define BTSOURCESTHREAD_H

#include <QThread>

#include <atomic>


class BtSourcesThread: public QThread {

    Q_OBJECT

public: /* Methods: */

    inline BtSourcesThread(QObject * parent = nullptr)
        : QThread(parent)
        , m_stop(false)
        , m_finishedSuccessfully(false)
    {}

    void stop() noexcept { m_stop.store(true, std::memory_order_release); }

    inline bool finishedSuccessfully() const noexcept
    { return m_finishedSuccessfully.load(std::memory_order_acquire); }

signals:

    void percentComplete(int percent);
    void showMessage(QString const & msg);

protected: /* Methods: */

    void run() override;

private: /* Methods: */

    inline bool shouldStop() const noexcept
    { return m_stop.load(std::memory_order_acquire); }

private: /* Fields: */

    std::atomic<bool> m_stop;
    std::atomic<bool> m_finishedSuccessfully;

}; /* class BtSourcesThread */

#endif
