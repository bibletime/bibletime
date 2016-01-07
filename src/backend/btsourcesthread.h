/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTSOURCESTHREAD_H
#define BTSOURCESTHREAD_H

#include <QThread>

class BtSourcesThread: public QThread {

        Q_OBJECT

    public:
        BtSourcesThread(QObject *parent = nullptr);
        void stop();

    signals:
        void percentComplete(int percent);
        void showMessage(const QString& msg);
        void finished();

    protected:
        void run() override;

    private:
        void updateRemoteLibraries();
        void updateRemoteLibrariesList();
        void updateRemoteLibraryWorks();

        bool m_stop;
};
#endif
