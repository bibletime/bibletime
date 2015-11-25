
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

    void takeModulesFromList();

    void getPointerList();
    void getConstPointerList();

    void cleanupTestCase();
};

