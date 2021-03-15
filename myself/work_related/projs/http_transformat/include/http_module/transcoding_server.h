#ifndef TRANSCODING_SERVER_INCLUDED
#define TRANSCODING_SERVER_INCLUDED

#include <vector>
#include <Poco/Util/ServerApplication.h>

class TranscodingServer : public Poco::Util::ServerApplication {
private:
    int main(const std::vector<std::string>& args) override;
};

#endif