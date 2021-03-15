//
// Created by skymelody on 2019/12/18.
//

#ifndef MP4TOH264_FTYPBOX_H
#define MP4TOH264_FTYPBOX_H

#include "Box.h"

class FtypBox : public Box {
public:
    void Init(const std::string& major_brand, uint32_t minor_version);

    void AddCompatibleBrand(const std::string& brand);

protected:
    void WriteAttr(FILE *out_file) override;

private:
    uint32_t major_brand_;
    uint32_t minor_version_;
    std::vector<uint32_t> compatible_brands_;
};


#endif //MP4TOH264_FTYPBOX_H
