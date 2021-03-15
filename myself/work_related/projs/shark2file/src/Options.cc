//
// Created by skymelody on 2019/12/24.
//

#include "Shark2File/Common/Options.h"
#include "Shark2File/Common/Log.h"
#include "Shark2File/Common/FileUtils.h"
#include "Shark2File/Common/Utils.h"


void Options::SetInputPath(const std::string &inputFilePath) {
    inputPath_ = inputFilePath;
}

void Options::SetOutputPath(const std::string &outputFilePath) {
    outputPath_ = outputFilePath;
}


const std::string& Options::InputPath() const {
    return inputPath_;
}

const std::string& Options::OutputPath() const {
    return outputPath_;
}

const std::string& Options::NetInterface() const {
    return netInterface_;
}

const std::string& Options::WireSharkPath() const {
    return wireSharkPath_;
}



const std::string &Options::CapturePacketOutPath() const {
	return capturePacketOutPath_;
}

bool Options::ListNetInterface() const {
	return listCard_;
}

bool Options::UseWireShark() const {
	return !wireSharkPath_.empty();
}

const std::vector<Options::ExtractPacketConfig>& Options::ExtractPacketConfigs() const {
	return extractPacketConfigs_;
}

bool Options::ShouldExtractDataFromPacket() const {
	return !extractPacketConfigs_.empty();
}


std::string Options::ToString() const {
	std::string res;
	res.append("-in     ").append(inputPath_).append(CLRF)
	   .append("-out    ").append(outputPath_).append(CLRF)
	   .append("-i      ").append(netInterface_).append(CLRF)
	   .append("-ws     ").append(wireSharkPath_).append(CLRF)
	   .append("-wsout  ").append(capturePacketOutPath_).append(CLRF)
	   .append("-D      ").append(std::to_string(listCard_)).append(CLRF)
	   .append("-pd");

	for (const auto& cfg : extractPacketConfigs_) {
		res.append("\t")
		   .append(" ")
		   .append(std::to_string(cfg.payloadType))
		   .append(" ")
		   .append(std::string(1, cfg.avType))
		   .append(CLRF);
	}
	return res;
}


Options::Options() : listCard_(false) {}

bool Options::SetupOptions(const std::vector<std::string> &args) {
	int i = 0;
	int nargs = args.size();
	while (i < nargs) {
		const std::string& arg = args[i];
		if (arg == "-in") {
			inputPath_ = args[++i];
		}
		else if (arg == "-out") {
			outputPath_ = args[++i];
		}
		else if (arg == "-i") {
			netInterface_ = args[++i];
		}
		else if (arg == "-ws") {
			wireSharkPath_ = args[++i];
		}
		else if (arg == "-wsout") {
			capturePacketOutPath_ = args[++i];
		}
		else if (arg == "-D") {
			listCard_ = true;
		}
		else if (arg == "-pd") {
			++i;
			int j = 0;
			for (j = i; j < nargs && args[j][0] != '-'; ++j) {}


			while (i < j) {
				int payloadType = 0;
				try {
					payloadType = std::stoi(args[i]);
				}
				catch (std::exception& e) {
					return false;
				}

				char avType = args[i+1][0];
				if (avType != 'a' && avType != 'v') {
					return false;
				}
				extractPacketConfigs_.push_back({payloadType, avType});
				i+=2;
			}

			i = j;
			continue;
		}
		else {
			return false;
		}
		++i;
	}


	if (!UseWireShark() && inputPath_.empty()) {
		LOG("\033[30;41mUse Wireshark Or Specify Input File\033[0m"
		    CLRF
		    CLRF);
		return false;
	}

	if (UseWireShark() && inputPath_.empty()) {
		if (capturePacketOutPath_.empty()) {
			capturePacketOutPath_.append(FileSystem::Current())
								 .append(PATH_SEPARATOR)
								 .append(CurrentTimeToStr());
		}
		else {
			auto pos = capturePacketOutPath_.find_last_of('.');
			capturePacketOutPath_ = capturePacketOutPath_.substr(0, pos);
		}
		inputPath_ = capturePacketOutPath_ + ".pcap";
		if (outputPath_.empty()) {
			outputPath_ = capturePacketOutPath_;
		}
	}
	else {
		wireSharkPath_ = "";
		if (outputPath_.empty()) {
			auto startpos = inputPath_.find_last_of('/')+1;
			auto endpos = inputPath_.find_last_of('.');
			outputPath_ = inputPath_.substr(startpos, endpos-startpos);
		}
	}
	return true;
}




