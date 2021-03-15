//
// Created by skymelody on 2019/12/9.
//

#ifndef MP4TOH264_BYTECONVERT_H
#define MP4TOH264_BYTECONVERT_H

#include <cstdint>
#include <string>
#include <cstring>
using Byte = uint8_t;


template <typename IntType>
IntType LittleEndToBigEnd(IntType val) {
    const int n = sizeof(IntType);
    IntType ret = 0;
    Byte* p = (Byte*)&ret;
    Byte* pval = (Byte*)&val;
    for (int i = 0; i < n; ++i) {
        p[i] = pval[n-i-1];
    }
    return ret;
}

template <typename IntType>
IntType BytesToInt(IntType bytes) {
    const int n = sizeof(IntType);
    Byte buf[n];
    memmove(buf, &bytes, n);

    IntType res = 0;
    int offset = 0;
    for (int i = n-1; i >= 0; --i) {
        res += buf[i] << (offset * 8);
        ++offset;
    }
    return res;
}

template <typename IntType>
void IntToBytes(IntType val, Byte* byteArr, const int n) {
    for (int i = n-1; i >= 0; --i) {
        byteArr[i] = (val % 256) & 0xff;
        val /= 256;
    }
}

template <typename ByteArr>
std::string BytesToStr(ByteArr bytes) {
    const int n = sizeof(ByteArr);
    char buf[n];
    memmove(buf, &bytes, n);
    return std::string(buf, n);
}

#endif //MP4TOH264_BYTECONVERT_bianmaH
