#include <QtTest/QtTest>
#include "backend/keys/cswordversekey.h"

class test_CSwordVerseKey : public QObject
{
    Q_OBJECT
public:


private slots:
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

