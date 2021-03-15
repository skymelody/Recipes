//
// Created by skymelody on 2019/12/9.
//

#include "FileStreamReader.h"
#include "CFileStream.h"
#include "CppFileStream.h"

int FileStreamReader::ReadNByte(Byte* buf, int n) {
    return fileStream->Read(buf, n);
}

FileStreamReader::FileStreamReader(const std::string filePath) :
    ndataRead(0), fileStream(new CppFileStream(filePath)){}

FileStreamReader::~FileStreamReader() {
    delete fileStream;
}

void FileStreamReader::SkipNByte(int n) {
    fileStream->SkipN(n);
    ndataRead += n;
}

bool FileStreamReader::HaveBytes() const {
    return !fileStream->Eof();

}

FileStreamReader::FileStreamReader(FILE *file) : ndataRead(0), fileStream(new CFileStream(file)){}

size_t FileStreamReader::CurrentOffset() const {
    return fileStream->CurrentOffset();
}

void FileStreamReader::Rewind(int n) {
    fileStream->Rewind(n);
}

