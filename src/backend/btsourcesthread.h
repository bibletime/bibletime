/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include <QThread>

#include <atomic>
#include <QObject>
#include <QString>


class BtSourcesThread: public QThread {

    Q_OBJECT

public: // methods:

    BtSourcesThread(QObject * parent = nullptr)
        : QThread(parent)
        , m_stop(false)
        , m_finishedSuccessfully(false)
    {}

    void stop() noexcept { m_stop.store(true, std::memory_order_release); }

    bool finishedSuccessfully() const noexcept
    { return m_finishedSuccessfully.load(std::memory_order_acquire); }

Q_SIGNALS:

    void percentComplete(int percent);
    void showMessage(QString const & msg);

protected: // methods:

    void run() override;

private: // methods:

    bool shouldStop() const noexcept
    { return m_stop.load(std::memory_order_acquire); }

private: // fields:

    std::atomic<bool> m_stop;
    std::atomic<bool> m_finishedSuccessfully;

}; /* class BtSourcesThread */
