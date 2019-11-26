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

#include <QObject>

class test_CSwordBackend : public QObject {
    Q_OBJECT

private slots:
    void initTestCase(); 

    void instance();
    void moduleList();
    void setFilterOptions();

    void booknameLanguage_data();
    void booknameLanguage();

    void findModuleByDescription();
    void findModuleByName();
    void findModuleByPointer();
    void getConfig();

    void optionName_data();
    void optionName();

    void configOptionName_data();
    void configOptionName();

    void translatedOptionName_data();
    void translatedOptionName();

#if 0
    void takeModulesFromList();
#endif

    void getPointerList();
    void getConstPointerList();

    void cleanupTestCase();
};

