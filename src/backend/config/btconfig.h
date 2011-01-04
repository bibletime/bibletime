#ifndef BTCONFIG_H
#define BTCONFIG_H

#include <QtCore/QHash>
#include <QtCore/QSet>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QSettings>

//#include "tests/btconfig_test.h"

class BtConfig
{
private:
    BtConfig();
    BtConfig(const BtConfig& other) {} // stub, do not use
    BtConfig& operator=(const BtConfig& other) {return *this;} // stub, do not use

    friend class BtConfigTest;
    BtConfig(QString settingsFile); //used by test

    static BtConfig* m_instance; // singleton instance

    QHash<QString,QVariant> m_defaults;
    QSet<QString> m_sessionSettings;
    QSettings m_settings;
public:
    BtConfig& getInstance();
    ~BtConfig();

    QVariant getValue(QString key);
    void setValue(QString key, QVariant value);
    bool hasValue(QString key);
    void deleteValue(QString key);
};

#endif // BTCONFIG_H
