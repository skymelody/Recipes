//
// Created by skymelody on 2020/1/15.
//

#include <cassert>
#include "Shark2File/Rtp/RtpLocalOutputer.h"
#include "Shark2File/Common/FileUtils.h"
#include "Shark2File/Common/Log.h"


RtpLocalOutputer::RtpLocalOutputer(const std::pair<std::string, std::string> &outputPath) :
	outputPath_(outputPath) {}

ErrorCode RtpLocalOutputer::WritePackets(const std::vector<pcpp::RawPacket> &packets) {
	pcpp::PcapFileWriterDevice srcWriter(outputPath_.first.c_str());
	pcpp::PcapFileWriterDevice destWriter(outputPath_.second.c_str());
	if (!srcWriter.open(FileSystem::Exist(outputPath_.first))) {
		LOG("Open Src Output %s Failed"
		    CLRF, outputPath_.first.c_str());
		srcWriter.close();
		destWriter.close();
		return ErrorCode::FAILED;
	}
	if (!destWriter.open(FileSystem::Exist(outputPath_.second))) {
		LOG("Open Dest Output %s Failed"
		    CLRF, outputPath_.second.c_str());
		srcWriter.close();
		destWriter.close();
		return ErrorCode::FAILED;
	}

	for (const auto& packet : packets) {

		if (!srcWriter.writePacket(packet)) {
			LOG("Write a Packet To %s Failed"
				CLRF, outputPath_.first.c_str());
		}
		if (!destWriter.writePacket(packet)) {
			LOG("Write a Packet To %s Failed"
			    CLRF, outputPath_.second.c_str());
		}
	}
	srcWriter.close();
	destWriter.close();
	return ErrorCode::OK;
}


static RtpLocalOutputer GenRtpOrRtcpOutputCommon(const Options& options, const RtpKey& key, bool isRtp) {
	std::string outputBase = options.OutputPath();
	if (outputBase.empty()) {
		outputBase = FileSystem::Current();
	}

	if (outputBase.back() != PATH_SEPARATOR[0]) {
		outputBase.append(PATH_SEPARATOR);
	}
	if (isRtp) {
		outputBase.append("rtp").append(PATH_SEPARATOR);
	}
	else {
		outputBase.append("rtcp").append(PATH_SEPARATOR);
	}

	std::pair<std::string, std::string> output{outputBase, outputBase};
	output.first.append("src")
			.append(PATH_SEPARATOR)
			.append(key.srcIp)
			.append(PATH_SEPARATOR);
	output.second.append("dest")
			.append(PATH_SEPARATOR)
			.append(key.destIp)
			.append(PATH_SEPARATOR);

	if (!FileSystem::Exist(output.first)) {
		FileSystem::MakeDir(output.first);
	}
	if (!FileSystem::Exist(output.second)) {
		FileSystem::MakeDir(output.second);
	}
	output.first.append(std::to_string(key.ssrc))
			.append(".pcap");
	output.second.append(std::to_string(key.ssrc))
			.append(".pcap");

	return RtpLocalOutputer(output);

}

RtpLocalOutputer GenRtcpLocalOutput(const Options& options, const RtpKey& key) {
	return GenRtpOrRtcpOutputCommon(options, key, false);
}

RtpLocalOutputer GenRtpLocalOutput(const Options& options, const RtpKey& key) {
	return GenRtpOrRtcpOutputCommon(options, key, true);
}
