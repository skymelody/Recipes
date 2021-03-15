//
// Created by skymelody on 2019/12/10.
//

#include "UnknowBox.h"
#include "ByteConvert.h"
#include <iostream>
using namespace std;

size_t UnknowBox::ParseAttr(FileStreamReader &reader) {
    int skipSize = BytesToInt(size) - headerSize;
    reader.SkipNByte(skipSize);
    return skipSize;
}
