//
// Created by skymelody on 2019/12/10.
//

#include "MdatBox.h"
#include "ByteConvert.h"
#include <iostream>

size_t MdatBox::ParseAttr(FileStreamReader &reader) {
    /* how many data */
    int ndata = BytesToInt(size) - headerSize;

    /* create tmp file to store data */
    file = tmpfile();

    if (file == nullptr) {
        std::cout << "can't create tmp file to store data" << std::endl;
        return 0;
    }

    Byte buf[4096];
    int nread = ndata % 4096;
    nread = reader.ReadNByte(buf, nread);
    ndata -= nread;
    fwrite(buf, 1, nread, file);

    while (ndata > 0) {
        int n = reader.ReadNByte(buf, 4096);
        fwrite(buf, 1, n, file);
        ndata -= n;
        nread += n;
        if (n == 0) {
            break;
        }
    }

    return nread;
}

FILE *MdatBox::GetFile() const {
    fseek(file, 0, SEEK_SET);
    return file;
}

void MdatBox::SetFile(FILE *file) {
    this->file = file;
    fseek(file, 0, SEEK_SET);
    fseek(file, 0, SEEK_END);
    int fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    size += fileSize;
}

MdatBox::MdatBox() {
    size = 8;
    Byte tp[4]{'m', 'd', 'a', 't'};
    memmove(&type, tp, 4);
}

void MdatBox::WriteAttr(FILE *out_file) {
    FILE* infile = GetFile();
    Byte* buf[40960];
    while (!feof(infile)) {
        int n = fread(buf, 1, 40960, infile);
        fwrite(buf, 1, n, out_file);
    }
}
