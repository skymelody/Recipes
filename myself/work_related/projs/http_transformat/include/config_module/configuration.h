#ifndef CONFIGURATION_INCLUDED
#define CONFIGURATION_INCLUDED
#include <string>
#include <unordered_set>


class Configuration {
public:
    int listeningPort;
    int threadNum;
    int httpTimeout;
    int maxConnectionQueued;
    std::string uploadFileSavedDirectory;
    std::string convertedFileSavedDirectory;
    std::unordered_set<std::string> supportedVideoTypes;
    std::string configFilePath;

private:
    Configuration();
    friend const Configuration& GetConfigutation();
    bool isConfigured;
};

const Configuration& GetConfigutation();

#endif