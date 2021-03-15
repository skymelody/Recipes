//
// Created by skymelody on 2019/12/9.
//

#ifndef MP4TOH264_CFILESTREAM_H
#define MP4TOH264_CFILESTREAM_H

#include <cstdio>
#include "FileStream.h"

class CFileStream : public FileStream {
public:
    CFileStream();

    CFileStream(const std::string& filePath);

    CFileStream(FILE* file);

    ~CFileStream() override ;

    void Open(const std::string &filePath) override;

    int Read(Byte *buf, int n) override;

    bool Eof() override;

    void SkipN(int n) override;

    size_t CurrentOffset() override;

    void Rewind(int n) override;

private:
    FILE* file;
};


#endif //MP4TOH264_CFILESTREAM_H
