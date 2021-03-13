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

#include <QtTest/QtTest>
#include "backend/keys/cswordversekey.h"

class test_CSwordVerseKey : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase();

    void CSwordVerseKey_module_constructor();
    void CSwordVerseKey_copy_constructor();
    void CSwordVerseKey_versekey_module_constructor();

    void setKey_data();
    void setKey();
    void next_data();
    void next();
    void previous_data();
    void previous();
    void setModule();

    void cleanupTestCase();

private:
    CSwordKey* m_key;
    CSwordModuleInfo* m_moduleKJV;
    CSwordModuleInfo* m_moduleKJVA;
    CSwordModuleInfo* m_moduleJosephus;
};

