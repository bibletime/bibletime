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

#pragma once

#include <memory>
#include <QtTest/QtTest>
#include <QString>
#include "backend/managers/cswordbackend.h"

class BtInstallThread;
class CSwordModuleInfo;
class QEventLoop;

class test_BtInstallThread : public QObject
{
    Q_OBJECT

public Q_SLOTS:
    void slotOneItemCompleted(int moduleIndex, bool successful);
    void slotThreadFinished();

private Q_SLOTS:
    void initTestCase();
    void installModules();
    void cleanupTestCase();

private:
    void initBackend();
    void getInstallPath();
    void removeModulesIfInstalled();
    void findModulesToInstall();

    BtInstallThread * m_thread = nullptr;
    QEventLoop * m_eventLoop = nullptr;
    std::unique_ptr<CSwordBackend const> m_installBackend;
    QList<CSwordModuleInfo *> m_modules;
    QString m_destination;
};

