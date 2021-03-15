//
// Created by skymelody on 2020/1/16.
//

#include <algorithm>
#include "PcapPlusPlus/IPv4Layer.h"
#include "PcapPlusPlus/UdpLayer.h"
#include "Shark2File/Rtp/RtpLocalMediaDataExtractor.h"
#include "Shark2File/Common/FileUtils.h"
#include "Shark2File/Common/Log.h"


static void SortRtpPackets(std::vector<pcpp::RawPacket>& packets) {
	std::sort(packets.begin(), packets.end(), [](pcpp::RawPacket& rp1, pcpp::RawPacket& rp2) {
		pcpp::Packet p1(&rp1);
		pcpp::Packet p2(&rp2);
		auto* rtpLayer1 = p1.getLayerOfType<pcpp::UdpLayer>()->getNextLayer();
		auto* rtpLayer2 = p2.getLayerOfType<pcpp::UdpLayer>()->getNextLayer();
		uint8_t* rtpData1 = rtpLayer1->getData();
		uint8_t* rtpData2 = rtpLayer2->getData();
		int seq1 = int(rtpData1[2] << 8) | rtpData1[3];
		int seq2 = int(rtpData2[2] << 8) | rtpData2[3];
		return seq1  < seq2;
	});
}

static void AdjustRtpDataOffsetAndDataLen(uint8_t*& rtpData, int& dataLen) {
	if (rtpData[0] & 0x20) {
		int paddintLen = rtpData[dataLen-1];
		dataLen -= paddintLen;
	}
	int ncsrc = rtpData[0] & 0x0f;
	int payloadOffset = 12+4*ncsrc;
	if (rtpData[0] & 0x10) {
		uint8_t* extensionData = rtpData + payloadOffset;
		int extensionLen = 4 * (extensionData[2] << 8 | extensionData[3]);
		payloadOffset += extensionLen;
	}
	rtpData += payloadOffset;
	dataLen -= payloadOffset;
}

void HandleSingleNalu(FILE* outStream, uint8_t* rtpData, int dataLen) {
	fwrite("\00\00\00\01", 1, 4, outStream);
	fwrite(rtpData, 1, dataLen, outStream);
}

void HandleFuaNalu(FILE* outStream, uint8_t* rtpData, int dataLen) {
	uint8_t indicator = rtpData[0];
	uint8_t header = rtpData[1];
	uint8_t flag = header & 0xe0;
	if (flag == 0x80) {
		fwrite("\00\00\00\01", 1, 4, outStream);
		uint8_t nalHeader = (indicator&0xe0) | (header & 0x1f);
		fwrite(&nalHeader, 1, 1, outStream);
		fwrite(rtpData+2, 1, dataLen-2, outStream);
	}
	else {
		fwrite(rtpData+2, 1, dataLen-2, outStream);
	}
}

void HandleStapANalu(FILE* outStream, uint8_t* rtpData, int dataLen) {
	++rtpData;
	do {
		int nalSize = int(rtpData[0]<<8 | rtpData[1]);
		fwrite("\00\00\00\01", 1, 4, outStream);
		fwrite(rtpData+2, 1, nalSize, outStream);
		rtpData += nalSize+2;
		dataLen -= nalSize+2;
	}
	while (dataLen > 0);
}

static ErrorCode WriteVideoPackets(std::vector<pcpp::RawPacket>& packets, const std::string& outputPath) {
	auto pkts = packets;
	SortRtpPackets(pkts);

	const char* mode = FileSystem::Exist(outputPath) ? "ab+" : "wb+";
	FILE* outStream = fopen(outputPath.c_str(), mode);
	int cnt = 0;

	for (auto& packet : pkts) {
		++cnt;
		pcpp::Packet parsedPacket(&packet);
		auto* rtpLayer = parsedPacket.getLayerOfType<pcpp::UdpLayer>()->getNextLayer();
		uint8_t* rtpData = rtpLayer->getData();
		int dataLen = rtpLayer->getDataLen();
		AdjustRtpDataOffsetAndDataLen(rtpData, dataLen);

		if (dataLen <= 0) {
			continue;
		}

		uint8_t type = rtpData[0] & 0x1f;
		if (type > 0 && type < 24) {
			HandleSingleNalu(outStream, rtpData, dataLen);
		}
		else if (type == 28) {
			HandleFuaNalu(outStream, rtpData, dataLen);
		}
		else if (type == 24) {
			HandleStapANalu(outStream, rtpData, dataLen);
		}
		else {
			auto* iplayer = parsedPacket.getLayerOfType<pcpp::IPv4Layer>();
			LOG("Unsupported Nal Type %d, Expect 0 < Type <= 24 or Type = 28"
			    CLRF, type);
		}

		if (cnt % 200 == 0) {
			fflush(outStream);
		}
	}
	fclose(outStream);
	return ErrorCode::OK;
}


static ErrorCode WriteAudioPackets(std::vector<pcpp::RawPacket>& packets, const std::string& outputPath) {
	auto pkts = packets;
	SortRtpPackets(pkts);

	const char* mode = FileSystem::Exist(outputPath) ? "ab+" : "wb+";
	FILE* outStream = fopen(outputPath.c_str(), mode);

	int cnt = 0;

	for (auto& packet : pkts) {
		++cnt;

		pcpp::Packet parsedPacket(&packet);
		auto* rtpLayer = parsedPacket.getLayerOfType<pcpp::UdpLayer>()->getNextLayer();
		uint8_t* rtpData = rtpLayer->getData();
		int dataLen = rtpLayer->getDataLen();
		AdjustRtpDataOffsetAndDataLen(rtpData, dataLen);

		if (dataLen <= 0) {
			continue;
		}
		fwrite(rtpData, 1, dataLen, outStream);

		if (cnt % 200 == 0) {
			fflush(outStream);
		}
	}
	fclose(outStream);
	return ErrorCode::OK;
}

RtpLocalMediaDataExtractor::RtpLocalMediaDataExtractor(const std::string &outputPath,
		const Options::ExtractPacketConfig &config) : outputPath_(outputPath), config_(config){}


ErrorCode RtpLocalMediaDataExtractor::WritePackets(std::vector<pcpp::RawPacket> &packets) {
	if (outputPath_.empty()) {
		return ErrorCode::FAILED;
	}
	if (packets.empty() || !IsDesiredPacket(packets[0])) {
		return ErrorCode::OK;
	}


	if (config_.avType == 'v') {
		return WriteVideoPackets(packets, outputPath_);
	}
	else if (config_.avType == 'a') {
		return WriteAudioPackets(packets, outputPath_);
	}

	return ErrorCode::OK;
}


bool RtpLocalMediaDataExtractor::IsDesiredPacket(pcpp::RawPacket& packet) {
	pcpp::Packet parsedPacket(&packet);
	auto* udpLayer = parsedPacket.getLayerOfType<pcpp::UdpLayer>();
	auto* rtpLayer = udpLayer->getNextLayer();
	uint8_t* rtpData = rtpLayer->getData();
	int dataLen = rtpLayer->getDataLen();
	if (dataLen <= 12) {
		return false;
	}
	int payloadType = rtpData[1] & 0x7f;
	return payloadType == config_.payloadType;
}


RtpLocalMediaDataExtractor GenRtpLocalMediaDataExtractor(const std::string& mediaDataSaveDir,
		const Options::ExtractPacketConfig &config,
		const RtpKey &key) {
	std::string outputPath = mediaDataSaveDir;
	if (outputPath.back() != PATH_SEPARATOR[0]) {
		outputPath.append(PATH_SEPARATOR);
	}

	if (config.avType == 'a') {
		outputPath.append("audio").append(PATH_SEPARATOR);
	}
	else if (config.avType == 'v') {
		outputPath.append("video").append(PATH_SEPARATOR);
	}
	else {
		LOG("Unsupported AvType %c"
				    CLRF, config.avType);
		return RtpLocalMediaDataExtractor("", config);
	}

	// create output dir base
	if (!FileSystem::Exist(outputPath)) {
		FileSystem::MakeDir(outputPath);
	}

	outputPath.append("SRC_")
	          .append(key.srcIp)
	          .append("_PORT_")
	          .append(std::to_string(key.srcPort))
	          .append("_to_")
	          .append("DEST_")
	          .append(key.destIp)
	          .append("_PORT_")
	          .append(std::to_string(key.destPort))
	          .append("_SSRC_")
	          .append(std::to_string(key.ssrc));
	if (config.avType == 'a') {
		outputPath.append(".g711");
	}
	else if (config.avType == 'v') {
		outputPath.append(".h264");
	}

	return RtpLocalMediaDataExtractor(outputPath, config);
}
