//
// Created by skymelody on 2019/12/9.
//

#include "CppFileStream.h"
#include <cstring>

CppFileStream::CppFileStream(const std::string &filepath) {
    file.open(filepath);
}

void CppFileStream::Open(const std::string &filePath) {
    file.open(filePath);
}

int CppFileStream::Read(Byte *buf, int n) {
    const int bufSize = n;
    char tmp[bufSize];
    file.read(tmp, bufSize);
    int nread = file.gcount();
    memmove(buf, tmp, nread);
    return nread;
}

bool CppFileStream::Eof() {
    return file.peek() == EOF;
}

CppFileStream::~CppFileStream() {
    if (file.is_open()) {
        file.close();
    }
}

void CppFileStream::SkipN(int n) {
    file.seekg(n, std::ios::cur);
}

size_t CppFileStream::CurrentOffset() {
    return file.tellg();
}

void CppFileStream::Rewind(int n) {
    file.seekg(n, std::ios::cur);
}

