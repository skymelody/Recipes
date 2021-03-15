#ifndef TRANSCODING_REQUEST_HANDLER_INCLUDED
#define TRANSCODING_REQUEST_HANDLER_INCLUDED
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPRequestHandler.h>

class TranscodingRequestHandler : public Poco::Net::HTTPRequestHandler {
public:
    void handleRequest
        (
            Poco::Net::HTTPServerRequest& request, 
            Poco::Net::HTTPServerResponse& response
        ) override;

};
#endif