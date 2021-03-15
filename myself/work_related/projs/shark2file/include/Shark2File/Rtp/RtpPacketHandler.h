//
// Created by skymelody on 2019/12/23.
//

#ifndef SHARK2FILE_RTPPACKETHANDLER_H
#define SHARK2FILE_RTPPACKETHANDLER_H

#include "Shark2File/Common/PacketHandler.h"
#include <unordered_map>


class RtpPacketHandler : public PacketHandler {
public:
    ErrorCode Handle(std::vector<pcpp::RawPacket> &packets) override;

private:
	/*
    ErrorCode Write(
            const std::unordered_map<RtpKey, std::vector<pcpp::Packet>>& srcIpPacketMap,
            const std::unordered_map<RtpKey, std::vector<pcpp::Packet>>& destIpPacketMap,
            const std::unordered_map<RtpKey, std::vector<pcpp::Packet>>& rtcpSrcPacketMap,
            const std::unordered_map<RtpKey, std::vector<pcpp::Packet>>& rtcpDestPacketMap);

    ErrorCode WriteMediaData(
            const std::unordered_map<RtpKey, std::vector<pcpp::Packet>>& packetsMap,
            const std::string& outpath,
            int payLoad,
            char avType);
            */

};


#endif //SHARK2FILE_RTPPACKETHANDLER_H
