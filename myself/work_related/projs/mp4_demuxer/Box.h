//
// Created by skymelody on 2019/12/9.
//

#ifndef MP4TOH264_BOX_H
#define MP4TOH264_BOX_H

#include <cstdint>
#include <array>
#include <vector>
#include "FileStreamReader.h"
#include "ByteConvert.h"
#include "arpa/inet.h"


class Box;
Box* GetBoxFromStream(FileStreamReader& reader);

class Box {
public:
    Box();
    virtual ~Box();
    bool Parse(FileStreamReader& reader);
    std::string Type() const;
    std::vector<Box*> Childs() const;
    Box* Father() const;
    void AddChild(Box* box);
    void Write(FILE* out_file);

    size_t Size() const;
protected:
    virtual size_t ParseAttr(FileStreamReader& reader);
    size_t ParseChild(FileStreamReader& reader, size_t childSize);

    void WriteHeader(FILE* out_file);
    virtual void WriteAttr(FILE* out_file);
    void WriteChild(FILE* out_file);

protected:
    uint32_t size;
    uint32_t type;
    uint64_t largeSize;

    size_t headerSize;
    std::array<uint8_t, 16> userType;

    Box* father;
    std::vector<Box*> childs;

    friend Box* GetBoxFromStream(FileStreamReader& reader);
};


#endif //MP4TOH264_BOX_H
