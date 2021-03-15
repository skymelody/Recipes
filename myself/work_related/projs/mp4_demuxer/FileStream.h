//
// Created by skymelody on 2019/12/9.
//

#ifndef MP4TOH264_FILESTREAM_H
#define MP4TOH264_FILESTREAM_H
#include <cstdint>
#include <string>

using Byte = uint8_t;

class FileStream {
public:
    FileStream();
    virtual ~FileStream();
    FileStream(const FileStream&) = delete;
    FileStream& operator=(const FileStream&) = delete;
    virtual void Open(const std::string& filePath) = 0;
    virtual int Read(Byte* buf, int n) = 0;
    virtual bool Eof() = 0;
    virtual void SkipN(int n) = 0;
    virtual void Rewind(int n) = 0;
    virtual size_t CurrentOffset() = 0;
};


#endif //MP4TOH264_FILESTREAM_H
