//
// Created by skymelody on 2019/12/9.
//

#include "CFileStream.h"

CFileStream::CFileStream() : file(nullptr){}

CFileStream::CFileStream(const std::string &filePath) {
    file = fopen(filePath.c_str(), "rb+");
}

CFileStream::~CFileStream() {
    if (file != nullptr) {
        fclose(file);
        file = nullptr;
    }
}

void CFileStream::Open(const std::string &filePath) {
    file = fopen(filePath.c_str(), "rb+");
}


int CFileStream::Read(Byte *buf, int n) {
    int nread = fread(buf, 1, n, file);
    return nread == -1 ? 0 : nread;
}

bool CFileStream::Eof() {
    return feof(file);
}

void CFileStream::SkipN(int n) {
    fseek(file, n, SEEK_CUR);
}

CFileStream::CFileStream(FILE * _file) : file(_file){

}

size_t CFileStream::CurrentOffset() {
    return ftell(file);
}

void CFileStream::Rewind(int n) {
    fseek(file, n, SEEK_CUR);
}


