#include "config_module/json_configuration_reader_poco_impl.h"

#include <fstream>
#include "Poco/JSON/Parser.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/Path.h"

using namespace std;
using namespace Poco::JSON;
using namespace Poco::Dynamic;

void JsonConfiguraionReaderPocoImpl::LoadConfigurationImpl(Configuration& configuration) {
    string jsonStr;
    string buf;
    ifstream inStream(configuration.configFilePath);
    while (inStream >> buf) {
        jsonStr += buf;
    }
    Parser jsonParser;
    Var result = jsonParser.parse(jsonStr);
    Object::Ptr object = result.extract<Object::Ptr>();
    configuration.listeningPort = object->getValue<int>("listening_port");
    configuration.threadNum = object->getValue<int>("thread_num");
    configuration.httpTimeout = object->getValue<int>("http_timeout");
    configuration.maxConnectionQueued = object->getValue<int>("max_connection_queued");
    configuration.uploadFileSavedDirectory = object->getValue<string>("upload_file_saved_directory");
    configuration.convertedFileSavedDirectory = object->getValue<string>("converted_file_saved_directory");

    auto supportedTypes = object->getArray("supported_video_types");
    for (auto it = supportedTypes->begin(); it != supportedTypes->end(); ++it) {
        configuration.supportedVideoTypes.insert(it->toString());
    }

}

JsonConfiguraionReaderPocoImpl::~JsonConfiguraionReaderPocoImpl() {}