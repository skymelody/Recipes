//
// Created by skymelody on 2019/12/9.
//

#ifndef MP4TOH264_CPPFILESTREAM_H
#define MP4TOH264_CPPFILESTREAM_H
#include "FileStream.h"
#include <fstream>

class CppFileStream : public FileStream {
public:
    CppFileStream() = default;

    CppFileStream(const std::string& filepath);

    ~CppFileStream() override ;

    void Open(const std::string &filePath) override;

    int Read(Byte *buf, int n) override;

    bool Eof() override;

    void SkipN(int n) override;

    size_t CurrentOffset() override;

    void Rewind(int n) override;

private:
    std::fstream file;
};


#endif //MP4TOH264_CPPFILESTREAM_H
