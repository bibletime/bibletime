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

#ifndef INDEX_THREAD_H
#define INDEX_THREAD_H

#include <QList>
#include <QMutex>
#include <QThread>

class CSwordModuleInfo;

class IndexThread: public QThread {

        Q_OBJECT

    public:
        explicit IndexThread(const QList<CSwordModuleInfo*>& modules, QObject* const parent = nullptr);
        void stopIndex();

    signals:
        void indexingProgress(int progressPercent);
        void indexingFinished();
        void beginIndexingModule(const QString& moduleName);
        void endIndexingModule(const QString& moduleName, bool success);

    protected:
        void run() override;

    private:
        void indexModule();

    private slots:
        void slotModuleProgress(int percentComplete);

    private:
        QList<CSwordModuleInfo*> m_modules;
        int m_currentModuleIndex;
        bool m_stopRequested;
        QMutex m_stopRequestedMutex;
};

#endif
