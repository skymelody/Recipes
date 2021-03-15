//
// Created by skymelody on 2019/12/10.
//

#ifndef MP4TOH264_MP4PARSER_H
#define MP4TOH264_MP4PARSER_H

#include <string>
#include <vector>
#include "Box.h"

class Mp4Parser {
public:
    bool Parse(const std::string& filepath);
    ~Mp4Parser();

    /* return nullptr if not found */
    Box* FindBox(const std::vector<Box*>& boxVec, const std::string& type);
    Box* FindVideoBox(const std::vector<Box*>& boxVec, const std::string& type);

    std::vector<Box*> Boxes() const;
private:
    void Clean();
private:
    std::vector<Box*> boxes;
};


#endif //MP4TOH264_MP4PARSER_H
