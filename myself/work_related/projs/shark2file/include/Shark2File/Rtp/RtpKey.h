//
// Created by skymelody on 2020/1/15.
//

#ifndef SHARK2FILE_RTPKEY_H
#define SHARK2FILE_RTPKEY_H

#include <string>
#include "PcapPlusPlus/Packet.h"
#include "Shark2File/Common/PacketKey.h"

struct RtpKey : public PacketKey {
	RtpKey() : ssrc(-1), srcPort(-1), destPort(-1) {}

	bool Valid() const override {
		return !srcIp.empty() &&
		       !destIp.empty() &&
		       ssrc != -1 &&
		       srcPort != -1 &&
		       destPort != -1;
	}

	bool operator==(const RtpKey& key) const {
		return srcIp  == key.srcIp &&
		       destIp == key.destIp &&
		       srcPort == key.srcPort &&
		       destPort == key.destPort;
		       ssrc   == key.ssrc;
	}


	std::string srcIp;
	std::string destIp;
	int srcPort;
	int destPort;
	int ssrc;
};

namespace std {
	template<>
	struct hash<RtpKey> {
		size_t operator()(const RtpKey& key) const {
			return hash<string>()(key.srcIp) ^
			       hash<string>()(key.destIp) ^
			       hash<int>()(key.srcPort) ^
			       hash<int>()(key.destPort) ^
			       hash<int>()(key.ssrc);
		}
	};
}

RtpKey GenRtpKey(pcpp::RawPacket& packet);
RtpKey GenRtcpKey(pcpp::RawPacket& packet);


#endif //SHARK2FILE_RTPKEY_H
