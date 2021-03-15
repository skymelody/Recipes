#include "config_module/json_configuration_reader.h"

JsonConfigurationReader::JsonConfigurationReader(JsonConfiguraionReaderImpl* impl) : _impl(impl) {}

JsonConfigurationReader::~JsonConfigurationReader() {
    delete _impl;
}

void JsonConfigurationReader::LoadConfiguration(Configuration& configuration) {
    _impl->LoadConfigurationImpl(configuration);
}