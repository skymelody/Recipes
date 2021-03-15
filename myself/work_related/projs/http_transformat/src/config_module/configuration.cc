#include "config_module/configuration.h"
#include "config_module/json_configuration_reader.h"
#include "config_module/json_configuration_reader_poco_impl.h"
#include "Poco/Path.h"

using namespace std;
using namespace Poco;


Configuration::Configuration() : isConfigured(false) {}


const Configuration& GetConfigutation() {
    static Configuration configuration;
    if (!configuration.isConfigured) {
        Path currentPath(Path::current());
        string configFilePath = currentPath.parent().toString() + "conf/config.json";
        configuration.configFilePath = configFilePath;

        JsonConfigurationReader configureReader(new JsonConfiguraionReaderPocoImpl());
        configureReader.LoadConfiguration(configuration);
        configuration.isConfigured = true;
    }
    return configuration;
}