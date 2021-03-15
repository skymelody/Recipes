//
// Created by skymelody on 2020/1/15.
//

#ifndef SHARK2FILE_PACKETSEPARATOR_H
#define SHARK2FILE_PACKETSEPARATOR_H

#include <vector>
#include <unordered_map>
#include <functional>
#include "PcapPlusPlus/Packet.h"

template <typename SeparateKey>
std::unordered_map<SeparateKey, std::vector<pcpp::RawPacket>>
SeparatePackets(std::vector<pcpp::RawPacket>& packets, std::function<SeparateKey (pcpp::RawPacket&)> genKeyFunc) {

	std::unordered_map<SeparateKey, std::vector<pcpp::RawPacket>> separatedPackets;
	for (auto& packet : packets) {
		SeparateKey key = genKeyFunc(packet);
		if (key.Valid()) {
			separatedPackets[key].push_back(packet);
		}
	}
	return separatedPackets;
}


#endif //SHARK2FILE_PACKETSEPARATOR_H
