/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef INSTALLSOURCESTHREAD_H
#define INSTALLSOURCESTHREAD_H

#include <QObject>

class BtInstallMgr;

namespace btm {

class InstallSources : public QObject {
        Q_OBJECT
    public:
        InstallSources(QObject *parent = nullptr);

        ~InstallSources();

    public slots:
        void process();
        void cancel();

    public:
        bool done;

    protected:
        void refreshSourceList();
        void refreshWorks(const QStringList& sourceNames);

        bool m_cancelled;
        BtInstallMgr* m_iMgr;

    signals:
        void finished();
        void error(QString err);
        void percentComplete(int percent, const QString& title);

private:
        bool m_canceled;
};

}
#endif
