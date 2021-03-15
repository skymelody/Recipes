//
// Created by skymelody on 2019/12/12.
//

#include "Mp4Mux.h"
#include "NaluParser.h"
#include "FileStreamReader.h"
#include "Sample.h"
#include "AvcFrameParser.h"
#include <vector>
#include <iostream>

using namespace std;

void DfsPrint(const vector<Box*>& boxes) {
    for (const Box* box : boxes) {
        cout << box->Type() << "   " << box->Size() << endl;
        DfsPrint(box->Childs());
    }
}

bool Mp4Mux::Mux(const std::string &h264Path, const std::string &outputPath) {
    FileStreamReader reader(h264Path);
    AvcFrameParser parser;
    bool res = parser.Parse(reader);
    if (!res) {
        return false;
    }

    FILE* outFile = fopen(outputPath.c_str(), "wb+");
    cout << (outFile == nullptr) << endl;

    vector<Box*> boxes = parser.GetBox();

    for (auto* box : boxes) {
        box->Write(outFile);
    }
    return res;
}
