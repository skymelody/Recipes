//
// Created by skymelody on 2019/12/9.
//

#ifndef MP4TOH264_FILESTREAMREADER_H
#define MP4TOH264_FILESTREAMREADER_H
#include "FileStream.h"



class FileStreamReader {
public:
    explicit FileStreamReader(const std::string filePath);
    explicit FileStreamReader(FILE* file);

    ~FileStreamReader();

    int ReadNByte(Byte* buf, int n);

    void SkipNByte(int n);

    size_t CurrentOffset() const;

    bool HaveBytes() const;

    void Rewind(int n);
private:
    /* how many data have been read */
    size_t ndataRead;
    FileStream* fileStream;
};


#endif //MP4TOH264_FILESTREAMREADER_H
