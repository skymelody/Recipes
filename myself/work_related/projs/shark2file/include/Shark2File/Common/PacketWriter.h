//
// Created by skymelody on 2020/1/15.
//

#ifndef SHARK2FILE_PACKETWRITER_H
#define SHARK2FILE_PACKETWRITER_H
#include <unordered_map>
#include <functional>
#include <thread>
#include "Shark2File/Common/ErrorCode.h"
#include "Shark2File/Common/Options.h"


const int THREAD_NUM = 4;

template <typename PacketKey>
using PacketMapIter = typename std::unordered_map<PacketKey, std::vector<pcpp::RawPacket>>::iterator;

template <typename PacketKey, typename Output>
void DoWritePackets(
		std::vector<PacketMapIter<PacketKey>>& mapIters,
		int offset,
		int nelements,
		std::function<Output (const PacketKey& key)>& genOutputFunc) {
	for (int i = 0; i < nelements; ++i) {
		auto& iter = mapIters[offset+i];
		const PacketKey& key = iter->first;
		std::vector<pcpp::RawPacket> &packets = iter->second;
		Output output = genOutputFunc(key);
		output.WritePackets(packets);
	}
}

template <typename PacketKey, typename Output>
void WritePackets(std::unordered_map<PacketKey, std::vector<pcpp::RawPacket>>& packetMap,
		               std::function<Output (const PacketKey& key)> genOutputFunc, bool useMutiThreads) {

	if (useMutiThreads) {
		std::vector<PacketMapIter<PacketKey>> mapIters;
		for (auto it = packetMap.begin(); it != packetMap.end(); ++it) {
			mapIters.push_back(it);
		}

		int packetsPerThread = mapIters.size() / THREAD_NUM;
		std::vector<std::thread> workers;
		for (int i = 0; i < THREAD_NUM; ++i) {
			int nelements =
					(i==THREAD_NUM-1 ? packetMap.size()-i*packetsPerThread : packetsPerThread);
			workers.emplace_back(
					DoWritePackets<PacketKey, Output>,
					std::ref(mapIters),
					i*packetsPerThread,
					nelements,
					std::ref(genOutputFunc)
					);
		}
		for (int i = 0; i < THREAD_NUM; ++i) {
			workers[i].join();
		}

	}

	else{
		for (auto& entry : packetMap) {
			const PacketKey &key = entry.first;
			std::vector<pcpp::RawPacket> &packets = entry.second;
			Output output = genOutputFunc(key);
			output.WritePackets(packets);
		}
	}
}

#endif //SHARK2FILE_PACKETWRITER_H
