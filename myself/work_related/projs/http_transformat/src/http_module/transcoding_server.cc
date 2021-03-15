#include "http_module/transcoding_server.h"
#include "http_module/transcoding_request_handler_factory.h"
#include "config_module/configuration.h"

#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPServerParams.h"

using namespace std;
using namespace Poco;
using namespace Poco::Net;

#include <iostream>
int TranscodingServer::main(const std::vector<std::string>& args) {
    const Configuration& config =  GetConfigutation();

    HTTPServerParams::Ptr param = new HTTPServerParams();
    param->setTimeout(config.httpTimeout);
    param->setMaxQueued(config.maxConnectionQueued);
    param->setMaxThreads(config.threadNum);

    SocketAddress sa(config.listeningPort);
    ServerSocket socket(sa);
    HTTPServer server(new TranscodingRequestHandlerFactory(), socket, param);
    server.start();
    waitForTerminationRequest();
    server.stop();
    return 0;
}