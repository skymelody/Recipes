#ifndef JSON_CONFIGURATION_READER_INCLUDED
#define JSON_CONFIGURATION_READER_INCLUDED
#include "configuration_reader.h"
#include "json_configuration_reader_impl.h"

class JsonConfigurationReader : public ConfigurationReader {
public:
    JsonConfigurationReader(JsonConfiguraionReaderImpl* impl);
    void LoadConfiguration(Configuration& configuration) override;
    ~JsonConfigurationReader();

private:
    JsonConfiguraionReaderImpl* _impl;
};

#endif