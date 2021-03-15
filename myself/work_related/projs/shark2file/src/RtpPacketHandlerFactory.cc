//
// Created by skymelody on 2019/12/23.
//

#include "Shark2File/Rtp/RtpPacketHandlerFactory.h"
#include "Shark2File/Rtp/RtpPacketHandler.h"

PacketHandler *RtpPacketHandlerFactory::CreatePacketHandler() {
    return new RtpPacketHandler;
}
