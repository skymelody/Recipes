//
// Created by skymelody on 2019/12/13.
//

#include "BitStream.h"
#include <cstdint>

const int BITS_NUM = 8;

static inline int BitMask(int n) {
    return (1<<n) - 1;
}

BitStream::BitStream(const std::vector<Byte>& data) : data(data.begin(), data.end()), bit_cached(0), cache(0), bitRead(0) {}

int BitStream::ReadBit() {
    uint32_t res;
    if (bit_cached == 0) {
        cache = data.front();
        data.pop_front();

        // sizeof(Byte)*8 - 1
        bit_cached = BITS_NUM - 1;
        res = cache >> (bit_cached);
    }
    else {
        res = (cache >> (--bit_cached)) & 1;
    }
    ++bitRead;

    return res;
}

void BitStream::SkipBits(int n) {
    if (n < bit_cached) {
        bit_cached -= n;
    }
    else {
        n -= bit_cached;
        // skip n byte;
        int nbyte = n/8;
        n %= 8;
        for (int i = 0; i < nbyte; ++i) {
            data.pop_front();
        }

        if (n != 0) {
            cache = data.front();
            data.pop_front();
            bit_cached = BITS_NUM - n;
        }
        else {
            cache = 0;
            bit_cached = 0;
        }
    }
}

uint32_t BitStream::ReadBits(int n) {
    int ntoread = n;
    if (n == 0) { return 0; }
    uint32_t res;
    if (bit_cached >= n) {
        bit_cached -= n;
        res = (cache >> bit_cached) & BitMask(n);
    }
    else {
        Byte byte = data.front();
        data.pop_front();

        Byte tmpCache = cache & BitMask(bit_cached);
        n -= bit_cached;
        bit_cached = BITS_NUM - n;
        res = bit_cached ? (byte >> bit_cached) | (tmpCache << n) : byte;
        cache = byte;
    }
    bitRead += ntoread;
    return res;
}

uint32_t BitStream::GetBitsRead() {
    return bitRead;
}
