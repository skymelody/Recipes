#ifndef JSON_CONFIGURATION_READER_POCO_IMPL
#define JSON_CONFIGURATION_READER_POCO_IMPL

#include "json_configuration_reader_impl.h"

class JsonConfiguraionReaderPocoImpl : public JsonConfiguraionReaderImpl {
    void LoadConfigurationImpl(Configuration &configuration) override;
    ~JsonConfiguraionReaderPocoImpl();
};

#endif