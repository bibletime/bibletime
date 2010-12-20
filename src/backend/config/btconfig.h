#ifndef BTCONFIG_H
#define BTCONFIG_H

#include <QtCore/QHash>
#include <QtCore/QSet>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QSettings>

class BtConfig
{
private:
    BtConfig();
    BtConfig(const BtConfig& other) {} // stub, do not use
    BtConfig& operator=(const BtConfig& other) {return *this;} // stub, do not use

    static BtConfig* m_instance; // singleton instance

    QHash<QString,QVariant> m_defaults;
    QSet<QString> m_sessionSettings;
    QSettings m_settings;
public:
    BtConfig& getInstance();
    ~BtConfig();
};

#endif // BTCONFIG_H
