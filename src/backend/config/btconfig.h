#ifndef BTCONFIG_H
#define BTCONFIG_H

class BtConfig
{
private:
    BtConfig();
    BtConfig(const BtConfig& other) {} // stub, do not use
    BtConfig& operator=(const BtConfig& other) {return *this;} // stub, do not use

    static BtConfig* m_instance;
public:
    BtConfig& getInstance();
    ~BtConfig();
};

#endif // BTCONFIG_H
