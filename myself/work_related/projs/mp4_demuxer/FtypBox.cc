//
// Created by skymelody on 2019/12/18.
//

#include "FtypBox.h"
#include <cstring>

void FtypBox::Init(const std::string& major_brand, uint32_t minor_version) {
    Byte tp[4]{'f', 't', 'y', 'p'};
    memmove(&type, tp, 4);

    const int brand_size = major_brand.size();
    Byte brand[brand_size];
    memmove(brand, major_brand.c_str(), brand_size);
    memmove(&major_brand_, brand, brand_size);
    minor_version_ = minor_version;
    size = brand_size + 12;
}

void FtypBox::AddCompatibleBrand(const std::string &brand) {
    const int brand_size = brand.size();
    Byte bytes[brand_size];
    memmove(bytes, brand.c_str(), brand_size);
    uint32_t compBrand = 0;
    memmove(&compBrand, bytes, 4);
    compatible_brands_.push_back(compBrand);
    size += 4;
}

void FtypBox::WriteAttr(FILE *out_file) {
    fwrite(&major_brand_, 1, 4, out_file);
    uint32_t bminor_version = LittleEndToBigEnd(minor_version_);
    fwrite(&bminor_version, 1, 4, out_file);
    for (uint32_t brand : compatible_brands_) {
        fwrite(&brand, 1, 4, out_file);
    }
}

