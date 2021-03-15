#ifndef TRANSCODING_REQUEST_HANDLER_FACTORY_INCLUDED
#define TRANSCODING_REQUEST_HANDLER_FACTORY_INCLUDED

#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPRequestHandler.h>

class TranscodingRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
private:
    Poco::Net::HTTPRequestHandler* createRequestHandler
        (
            const Poco::Net::HTTPServerRequest &request
        ) override;
};

#endif
