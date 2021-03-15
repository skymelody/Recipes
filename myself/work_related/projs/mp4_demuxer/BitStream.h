//
// Created by skymelody on 2019/12/13.
//

#ifndef MP4TOH264_BITSTREAM_H
#define MP4TOH264_BITSTREAM_H

#include <vector>
#include <deque>
#include <cstdint>

using Byte = unsigned char;


class BitStream {
public:
    explicit BitStream(const std::vector<Byte>& data);
    BitStream(const BitStream&) = delete;
    BitStream& operator=(const BitStream&) = delete;
    uint32_t GetBitsRead();

    void SkipBits(int n);
    int ReadBit();
    uint32_t ReadBits(int n);


private:
    std::deque<Byte> data;
    int bit_cached;
    int bitRead;
    Byte cache;
};


#endif //MP4TOH264_BITSTREAM_H
