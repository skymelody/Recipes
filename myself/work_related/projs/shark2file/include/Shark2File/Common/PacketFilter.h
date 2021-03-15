//
// Created by skymelody on 2019/12/23.
//

#ifndef SHARK2FILE_PACKETFILTER_H
#define SHARK2FILE_PACKETFILTER_H

#include "PcapPlusPlus/Packet.h"

class PacketFilter {
public:
    virtual bool Filter(pcpp::RawPacket& packet) = 0;
};


#endif //SHARK2FILE_PACKETFILTER_H
