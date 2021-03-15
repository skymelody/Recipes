//
// Created by skymelody on 2019/12/23.
//

#include "PcapPlusPlus/IPv4Layer.h"
#include "Shark2File/Common/PacketHandlerFactory.h"
#include "Shark2File/Common/Options.h"

ErrorCode StartHandler(std::shared_ptr<PacketHandlerFactory> factory, std::shared_ptr<PacketFilter> filter, const Options& options) {

    // read packets from file
    const std::string& inputFile = options.InputPath();
    pcpp::PcapFileReaderDevice reader(inputFile.c_str());
    reader.open();

    assert(reader.open());

	std::unique_ptr<PacketHandler> handler(factory->CreatePacketHandler());
	handler->SetOption(options);

	std::vector<pcpp::RawPacket> packets;
	int64_t flushCnt = 0;
	for (; ;) {
		pcpp::RawPacket packet;
		bool res = reader.getNextPacket(packet);

    	if (flushCnt == 10000 || !res) {
    		if (handler->Handle(packets) == ErrorCode::FAILED) {
    			return ErrorCode::FAILED;
    		}
    		flushCnt = 0;
    		packets.clear();

		    if (!res) {
			    break;
		    }

    	}


		if (filter->Filter(packet)) {
        	packets.push_back(packet);
        }
        ++flushCnt;
    }

	return ErrorCode::OK;
}
