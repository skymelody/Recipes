//
// Created by skymelody on 2019/12/23.
//

#ifndef SHARK2FILE_PACKETHANDLERFACTORY_H
#define SHARK2FILE_PACKETHANDLERFACTORY_H


#include <memory>
#include <cassert>
#include "Shark2File/Common/PacketHandler.h"
#include "Shark2File/Common/PacketFilter.h"
#include "PcapPlusPlus/PcapFileDevice.h"

class PacketHandlerFactory {
public:
    virtual PacketHandler* CreatePacketHandler() = 0;
};

ErrorCode StartHandler(std::shared_ptr<PacketHandlerFactory> factory, std::shared_ptr<PacketFilter> filter, const Options& options);

#endif //SHARK2FILE_PACKETHANDLERFACTORY_H
