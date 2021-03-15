//
// Created by skymelody on 2019/12/9.
//

#include "Box.h"
#include "ByteConvert.h"
#include "BoxCreater.h"
#include <fstream>
#include <iostream>
#include <cstring>
using namespace std;

const int BOX_SIZE_SIZE = 4;
const int BOX_TYPE_SIZE = 4;
const int BOX_LARGESIZE_SIZE = 8;
const int BOX_UUID_SIZE = 128;

bool Box::Parse(FileStreamReader& reader) {

    int boxSize = BytesToInt(size) == 0 ? BytesToInt(largeSize) : BytesToInt(size);

    int attrSize = ParseAttr(reader);
    int childSize = boxSize - headerSize - attrSize;



    childSize = ParseChild(reader, childSize);


    bool successful = (headerSize >= 0 && attrSize >= 0 && childSize >= 0) &&
            boxSize == (headerSize + attrSize + childSize);
    if (!successful) {
        cout << "Box : " << BytesToStr(type) << " Parse Failed" << endl;
    }
    return successful;
}


size_t Box::ParseAttr(FileStreamReader& reader) {
    return 0;
}

size_t Box::ParseChild(FileStreamReader& reader, size_t childSize) {
    if (childSize <= 0) {
        return childSize;
    }
    for (int i = childSize; i > 0; ) {
        Box* child = GetBoxFromStream(reader);
        child->father = this;
        childs.push_back(child);
        bool res = child->Parse(reader);
        if (res == 0) {
            childSize = 0;
            break;
        }
        i -= BytesToInt(child->size);
    }
    return childSize;
}

/* read box header and construct box */
Box* GetBoxFromStream(FileStreamReader& reader) {
    Byte buf[8];
    size_t headerSize = 0;

    /* box size */
    uint32_t size = 0;
    int nread = reader.ReadNByte(buf, BOX_SIZE_SIZE);
    memmove(&size, buf, nread);
    headerSize += nread;

    /* box type */
    uint32_t type = 0;
    nread = reader.ReadNByte(buf, BOX_TYPE_SIZE);
    memmove(&type, buf, nread);
    headerSize += nread;

    Box* box = NewBox(BytesToStr(type));
    /* set size and type */
    box->size = size;
    box->type = type;

    uint64_t largeSize = 0;
    if (BytesToInt(size) == 0) {
        nread = reader.ReadNByte(buf, BOX_LARGESIZE_SIZE);
        memmove(&largeSize, buf, nread);
        box->largeSize = largeSize;
        headerSize += nread;
    }

    if (BytesToStr(type) == "uuid") {
        int n = BOX_UUID_SIZE / sizeof(Byte);
        for (int i = 0; i < n; ++i) {
            nread = reader.ReadNByte(buf, 1);
            memmove((box->userType.data())+i, buf, 1);
            headerSize += nread;
        }
    }
    box->headerSize = headerSize;
    cout << box->Type() << "   " << (BytesToInt(box->size) == 0 ? BytesToInt(box->largeSize) : BytesToInt(size))<< "   " << headerSize << endl;
    return box;
}

Box::Box() : size(0), type(0), largeSize(0), headerSize(0), father(nullptr) {}

Box::~Box() {
    for (Box* child : childs) {
        delete child;
    }
    childs.clear();
}

std::string Box::Type() const {
    return BytesToStr(type);
}

std::vector<Box *> Box::Childs() const {
    return childs;
}

Box *Box::Father() const {
    return father;
}

void Box::AddChild(Box* box) {
    size += box->size;
    childs.push_back(box);
}

void Box::Write(FILE* out_file) {
    WriteHeader(out_file);
    WriteAttr(out_file);
    WriteChild(out_file);
}

void Box::WriteHeader(FILE* out_file) {
    uint32_t bsize = LittleEndToBigEnd(size);
    fwrite(&bsize, 4, 1, out_file);
    uint32_t btype = LittleEndToBigEnd(type);
    fwrite(&type, 4, 1, out_file);
}

void Box::WriteAttr(FILE* out_file) {

}

void Box::WriteChild(FILE* out_file) {
    for (Box* child : childs) {
        child->Write(out_file);
    }
}

size_t Box::Size() const {
    return size;
}
