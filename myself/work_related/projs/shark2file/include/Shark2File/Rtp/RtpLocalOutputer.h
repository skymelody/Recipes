//
// Created by skymelody on 2020/1/15.
//

#ifndef SHARK2FILE_RTPLOCALOUTPUTER_H
#define SHARK2FILE_RTPLOCALOUTPUTER_H


#include <utility>
#include <string>
#include "PcapPlusPlus/PcapFileDevice.h"
#include "PcapPlusPlus/Packet.h"
#include "Shark2File/Common/Options.h"
#include "Shark2File/Common/ErrorCode.h"
#include "Shark2File/Rtp/RtpKey.h"

class RtpLocalOutputer {
public:
    explicit RtpLocalOutputer(const std::pair<std::string, std::string>& outputPath);

    ErrorCode WritePackets(const std::vector<pcpp::RawPacket>& packets);

private:
	std::pair<std::string, std::string> outputPath_;
};

RtpLocalOutputer GenRtcpLocalOutput(const Options& options, const RtpKey& key);
RtpLocalOutputer GenRtpLocalOutput(const Options& options, const RtpKey& key);

#endif //SHARK2FILE_RTPLOCALOUTPUTER_H
