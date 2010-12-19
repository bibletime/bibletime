#include "btconfig.h"

#include <cstddef> // NULL macro

/*
 * set the instance variable initially to NULL, so it can be safely checked
 * whether the variable has been initialized yet.
 */
BtConfig* BtConfig::m_instance = NULL;

BtConfig::BtConfig()
{

}

BtConfig::~BtConfig()
{

}

BtConfig& BtConfig::getInstance()
{
    if(m_instance == NULL)
        m_instance = new BtConfig;
    return *m_instance;
}
