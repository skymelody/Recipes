//
// Created by skymelody on 2019/12/10.
//

#include "Mp4Parser.h"
#include "Box.h"
#include "ByteConvert.h"


#include <iostream>
using namespace std;

bool Mp4Parser::Parse(const std::string &filepath) {
    if (!boxes.empty()) {
        Clean();
    }

    FileStreamReader reader(filepath);
    if (!reader.HaveBytes()) {
        return false;
    }

    while (reader.HaveBytes()) {
        Box* box = GetBoxFromStream(reader);
        if (!box->Parse(reader)) {
            Clean();
            return false;
        }
        boxes.push_back(box);
    }
    return true;
}

Mp4Parser::~Mp4Parser() {
    Clean();
}

void Mp4Parser::Clean() {
    for (auto* box : boxes) {
        delete box;
    }
    boxes.clear();
}

Box *Mp4Parser::FindBox(const vector<Box*>& boxVec, const std::string &type) {
    for (Box* box : boxVec) {
        if (box != nullptr && box->Type() == type) {
            return box;
        }
        if (box != nullptr) {
            Box* target = FindBox(box->Childs(), type);
            if (target != nullptr) {
                return target;
            }
        }
    }
    return nullptr;
}

Box *Mp4Parser::FindVideoBox(const vector<Box*>& boxVec, const std::string &type) {
    Box* hdlr = FindBox(boxVec, "hdlr");
    if (hdlr == nullptr) {
        return nullptr;
    }
    Box* parent = hdlr->Father();
    return FindBox(parent->Childs(), type);
}

std::vector<Box *> Mp4Parser::Boxes() const {
    return boxes;
}
