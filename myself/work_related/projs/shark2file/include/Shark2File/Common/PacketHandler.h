//
// Created by skymelody on 2019/12/23.
//

#ifndef SHARK2FILE_PACKETHANDLER_H
#define SHARK2FILE_PACKETHANDLER_H

#include <vector>
#include <memory>
#include "PcapPlusPlus/Packet.h"
#include "Shark2File/Common/ErrorCode.h"
#include "Shark2File/Common/Options.h"
#include "Shark2File/Common/PacketSeparator.h"
class PacketHandler {
public:
    virtual ErrorCode Handle(std::vector<pcpp::RawPacket>& packets) = 0;

    Options GetOptions() const {
        return options_;
    }
    void SetOption(const Options& options) {
        options_ = options;
    }


protected:
    Options options_;
};



#endif //SHARK2FILE_PACKETHANDLER_H
