#include "http_module/transcoding_request_handler.h"
#include "http_module/transcoding_request_handler_factory.h"

using namespace Poco::Net;

HTTPRequestHandler* TranscodingRequestHandlerFactory::createRequestHandler
    (
        const HTTPServerRequest &request
    ) {
    if (request.getMethod() == HTTPRequest::HTTP_POST) {
        return new TranscodingRequestHandler();
    }
    return nullptr;
}