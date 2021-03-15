#ifndef JSON_CONFIGURATION_READER_IMPL_INCLUDED
#define JSON_CONFIGURATION_READER_IMPL_INCLUDED

#include "configuration.h"

class JsonConfiguraionReaderImpl {
public:
    virtual void LoadConfigurationImpl(Configuration& configuration) = 0;
    virtual ~JsonConfiguraionReaderImpl();
};

#endif