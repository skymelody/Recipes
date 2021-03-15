//
// Created by skymelody on 2019/12/23.
//

#ifndef SHARK2FILE_RTPPACKETFILTER_H
#define SHARK2FILE_RTPPACKETFILTER_H

#include "Shark2File/Common/PacketFilter.h"

class RtpPacketFilter : public PacketFilter{
public:
    bool Filter(pcpp::RawPacket &packet) override;
};


#endif //SHARK2FILE_RTPPACKETFILTER_H
