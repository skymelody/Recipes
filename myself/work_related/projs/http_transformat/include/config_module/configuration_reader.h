#ifndef CONFIGURATION_READER_INCLUDED
#define CONFIGURATION_READER_INCLUDED

#include "configuration.h"

class ConfigurationReader {
public:
    virtual void LoadConfiguration(Configuration& configuration) = 0;
    virtual ~ConfigurationReader();
};

#endif