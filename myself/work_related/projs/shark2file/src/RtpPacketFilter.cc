//
// Created by skymelody on 2019/12/23.
//

#include "PcapPlusPlus/IPv4Layer.h"
#include "Shark2File/Rtp/RtpPacketFilter.h"
#include "PcapPlusPlus/UdpLayer.h"

bool RtpPacketFilter::Filter(pcpp::RawPacket &rawPacket) {
    // check if packet is udp

    pcpp::Packet packet(&rawPacket);
    auto* udplayer = packet.getLayerOfType<pcpp::UdpLayer>();
	if (udplayer == nullptr) {
		return false;
	}

	auto* udpheader = udplayer->getUdpHeader();
	uint16_t srcPort = ((udpheader->portSrc&0xff00) >> 8) | ((udpheader->portSrc&0x00ff) << 8);
	uint16_t destPort = ((udpheader->portDst&0xff00) >> 8) | ((udpheader->portDst&0x00ff) << 8);
    if (srcPort <= 1000 || destPort <= 1000) {
    	return false;
    }

    auto* iplayer = packet.getLayerOfType<pcpp::IPv4Layer>();

    auto* payloadLayer = udplayer->getNextLayer();
    if (payloadLayer == nullptr) {
        return false;
    }


    // check if packet is rtp
    int packetSize = payloadLayer->getDataLen();
    if (packetSize < 12) {
        return false;
    }

    uint8_t* packetData = payloadLayer->getData();
    // currently, rtp version is 2
    if ((packetData[0] >> 6) != 2) {
        return false;
    }

    if (packetData[0] & 0x20) {
        // padding present
        size_t paddingLen = packetData[packetSize-1];
        if (paddingLen + 12 > packetSize) {
            return false;
        }
        packetSize -= paddingLen;
    }

    int ncsrcs = packetData[0] & 0x0f;
    size_t payloadOffset = 12+4*ncsrcs;

    if (packetSize < payloadOffset) {
        return false;
    }

    if (packetData[0] & 0x10) {
        // header extension present
        if (packetSize < payloadOffset + 4) {
            return false;
        }

        const uint8_t* extensionData = packetData + payloadOffset;
        size_t extensionLen = 4 * (extensionData[2] << 8 | extensionData[3]);
        if (packetSize < payloadOffset + 4 + extensionLen) {
            return false;
        }
    }


    return true;
}
