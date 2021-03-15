//
// Created by skymelody on 2019/12/23.
//

#ifndef SHARK2FILE_RTPPACKETHANDLERFACTORY_H
#define SHARK2FILE_RTPPACKETHANDLERFACTORY_H


#include "Shark2File/Common/PacketHandlerFactory.h"
class RtpPacketHandlerFactory : public PacketHandlerFactory{
public:
    PacketHandler *CreatePacketHandler() override;
};


#endif //SHARK2FILE_RTPPACKETHANDLERFACTORY_H
