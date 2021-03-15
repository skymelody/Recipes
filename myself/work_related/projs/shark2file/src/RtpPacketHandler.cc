//
// Created by skymelody on 2019/12/23.
//

#include "Shark2File/Rtp/RtpPacketHandler.h"
#include "Shark2File/Rtp/RtpKey.h"
#include "Shark2File/Rtp/RtpLocalOutputer.h"
#include "Shark2File/Rtp/RtpLocalMediaDataExtractor.h"
#include "Shark2File/Common/PacketSeparator.h"
#include "Shark2File/Common/PacketWriter.h"


ErrorCode RtpPacketHandler::Handle(std::vector<pcpp::RawPacket> &packets) {

	auto rtpPacketMap = SeparatePackets<RtpKey>(packets, GenRtpKey);
	auto rtcpPacketMap = SeparatePackets<RtpKey>(packets, GenRtcpKey);

	// write rtp
	auto genLocalRtpOutputFunc = std::bind(GenRtpLocalOutput, options_, std::placeholders::_1);
	WritePackets<RtpKey, RtpLocalOutputer>(rtpPacketMap, genLocalRtpOutputFunc, false);

	// write rtcp
	auto genLocalRtcpOutputFunc = std::bind(GenRtcpLocalOutput, options_, std::placeholders::_1);
	WritePackets<RtpKey, RtpLocalOutputer>(rtcpPacketMap, genLocalRtcpOutputFunc, false);

	// Extract Media Data
	if (options_.ShouldExtractDataFromPacket()) {
		const auto& configs = options_.ExtractPacketConfigs();

		for (const auto& config : configs) {
			auto genLocalMediDataExtractorFunc =
					std::bind(GenRtpLocalMediaDataExtractor,
							  options_.OutputPath(),
							  config,
							  std::placeholders::_1);
			WritePackets<RtpKey, RtpLocalMediaDataExtractor>(rtpPacketMap, genLocalMediDataExtractorFunc, false);
		}
	}



	return ErrorCode::OK;
}

