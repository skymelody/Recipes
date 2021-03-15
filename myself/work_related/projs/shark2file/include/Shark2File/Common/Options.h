//
// Created by skymelody on 2019/12/24.
//

#ifndef SHARK2FILE_OPTIONS_H
#define SHARK2FILE_OPTIONS_H

#include <string>
#include <vector>

class Options {
public:
	struct ExtractPacketConfig {
		int payloadType;
		char avType;
	};

    Options();
    bool SetupOptions(const std::vector<std::string>& args);

public:
    void SetInputPath(const std::string& inputFilePath);
    void SetOutputPath(const std::string& outputFilePath);

    const std::string& InputPath() const;
    const std::string& OutputPath() const;
    const std::string& NetInterface() const;
    const std::string& WireSharkPath() const;
	const std::string& CapturePacketOutPath() const;
    bool ListNetInterface() const;
	bool UseWireShark() const;
	bool ShouldExtractDataFromPacket() const;
	const std::vector<ExtractPacketConfig>& ExtractPacketConfigs() const;


    std::string ToString() const;

private:
    std::string inputPath_;
    std::string outputPath_;
    std::string netInterface_;
    std::string wireSharkPath_;
    std::string capturePacketOutPath_;
    bool listCard_;

    std::vector<ExtractPacketConfig> extractPacketConfigs_;
};


#endif //SHARK2FILE_OPTIONS_H
