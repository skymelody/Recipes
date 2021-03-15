//
// Created by skymelody on 2020/1/16.
//

#ifndef SHARK2FILE_RTPLOCALMEDIADATAEXTRACTOR_H
#define SHARK2FILE_RTPLOCALMEDIADATAEXTRACTOR_H

#include <string>
#include <vector>
#include "Shark2File/Common/ErrorCode.h"
#include "Shark2File/Common/Options.h"
#include "Shark2File/Rtp/RtpKey.h"
#include "PcapPlusPlus/Packet.h"

class RtpLocalMediaDataExtractor {
public:
	explicit RtpLocalMediaDataExtractor(const std::string& outputPath, const Options::ExtractPacketConfig& config);
	ErrorCode WritePackets(std::vector<pcpp::RawPacket>& packets);

private:
	bool IsDesiredPacket(pcpp::RawPacket& packet);
private:
	std::string outputPath_;
	Options::ExtractPacketConfig config_;
};

RtpLocalMediaDataExtractor GenRtpLocalMediaDataExtractor(const std::string& mediaDataSaveDir,
                                                         const Options::ExtractPacketConfig &config,
                                                         const RtpKey &key);

#endif //SHARK2FILE_RTPLOCALMEDIADATAEXTRACTOR_H
