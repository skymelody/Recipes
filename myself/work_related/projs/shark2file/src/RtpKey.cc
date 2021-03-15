//
// Created by skymelody on 2020/1/15.
//

#include "Shark2File/Rtp/RtpKey.h"
#include "PcapPlusPlus/IPv4Layer.h"
#include "PcapPlusPlus/UdpLayer.h"


static void GenRtpOrRtcpKey(pcpp::RawPacket& packet, RtpKey& key, bool genRtp) {
	pcpp::RawPacket rawPacket = packet;
	pcpp::Packet parsedPacket(&rawPacket);
	auto* udpLayer = parsedPacket.getLayerOfType<pcpp::UdpLayer>();
	uint8_t* data = udpLayer->getData();
	uint8_t flag = data[9];
	bool isRtp = !(flag >= 200 && flag <= 206);
	if ((genRtp && !isRtp) || (!genRtp && isRtp)) {
		return;
	}
	int ssrcOffset = 8 + (isRtp ? 8 : 4);
	uint8_t buf[4]{0};
	buf[0] = data[ssrcOffset+3];
	buf[1] = data[ssrcOffset+2];
	buf[2] = data[ssrcOffset+1];
	buf[3] = data[ssrcOffset];
	memmove(&key.ssrc, buf, 4);

	auto* ipLayer = parsedPacket.getLayerOfType<pcpp::IPv4Layer>();
	key.srcIp = ipLayer->getSrcIpAddress().toString();
	key.destIp = ipLayer->getDstIpAddress().toString();
	key.srcPort = udpLayer->getUdpHeader()->portSrc;
	key.destPort = udpLayer->getUdpHeader()->portDst;
}

RtpKey GenRtpKey(pcpp::RawPacket &packet) {
	RtpKey key;
	GenRtpOrRtcpKey(packet, key, true);
	return key;
}

RtpKey GenRtcpKey(pcpp::RawPacket &packet) {
	RtpKey key;
	GenRtpOrRtcpKey(packet, key, false);
	return key;
}
