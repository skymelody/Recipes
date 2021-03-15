
// Created by skymelody on 2019/12/13.
//

#include "NaluParser.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

const int NaluParser::BUFFER_SIZE = 10240;
// load buffer when buffer size < this value
const int NaluParser::LOW_BOUND_LOAD_BUFFER = 4;

const std::vector<Byte> startCode3 {0x00, 0x00, 0x01};
const std::vector<Byte> startCode4 {0x00, 0x00, 0x00, 0x01};




static bool IsStartCode4(const vector<Byte>& buffer, int cur) {
    return buffer.size()-cur >= startCode4.size() &&
            (buffer[cur] == 0 && buffer[cur+1] == 0 && buffer[cur+2] == 0 && buffer[cur+3] == 1);
}

static bool IsStartCode3(const vector<Byte>& buffer, int cur) {
    return buffer.size()-cur >= 3 &&
        (buffer[cur] == 0 && buffer[cur+1] == 0 && buffer[cur+2] == 1);
}


static void CopyDataToNalu() {

}

void NaluParser::LoadBuffer(FileStreamReader& reader) {
    auto end = copy(buffer.begin()+cur, buffer.end(), buffer.begin());
    int ncopy = end-buffer.begin();
    int ntoread = buffer.size() - ncopy;
    int nread = reader.ReadNByte(buffer.data()+ncopy, ntoread);
    cur = 0;
    buffer.resize(nread+ncopy);

}

bool NaluParser::GetOneNalu(Nalu &nalu, FileStreamReader &reader) {
    if (buffer.size() - cur <= LOW_BOUND_LOAD_BUFFER) {
        LoadBuffer(reader);
    }

    if (IsStartCode4(buffer, cur)) {
        nalu.prefixLen = startCode4.size();
        cur += startCode4.size();
    }
    else if (IsStartCode3(buffer, cur)) {
        nalu.prefixLen = startCode3.size();
        cur += startCode3.size();
    }
    else if (buffer.size()-cur < startCode3.size()) {
        return false;
    }


    bool nextNaluFound = false;

    while (!nextNaluFound) {
        auto code3 = buffer.end();
        auto code4 = buffer.end();

        int bufferSize = buffer.size() - cur;

        // search 0 0 1
        if (bufferSize > startCode3.size()) {
            code3 = std::search(buffer.begin()+cur, buffer.end(), startCode3.begin(), startCode3.end());
        }
        if (code3 != buffer.end() && code3 > buffer.begin()+cur) {
            // find code 4
            if (*(code3-1) == 0) {
                code4 = code3-1;
            }
        }
        /*
        // search 0 0 0 1
        if (bufferSize > startCode4.size()) {
            code4 = std::search(buffer.begin()+cur, buffer.end(), startCode4.begin(), startCode4.end());
        }
        */


        // if startcode not found
        if (code3 == buffer.end() && code4 == buffer.end()) {

            // end of file and last nalu
            if (!reader.HaveBytes()) {
                nalu.data.insert(nalu.data.end(), buffer.begin()+cur, buffer.end());
                cur = buffer.size();
                break;
            }

            if (bufferSize > LOW_BOUND_LOAD_BUFFER) {
                // copy data in cur---->buffer.size()-4;
                // 1 2 3 4 5 6 7 8 .
                int naluCurrentSize  = nalu.data.size();
                int ntocopy = (buffer.end()-4) - (buffer.begin()+cur);
                nalu.data.resize(naluCurrentSize + ntocopy);
                memcpy(nalu.data.data()+naluCurrentSize, buffer.data()+cur, ntocopy);
                // copy(buffer.begin()+cur, buffer.end()-4, back_inserter(nalu.data));
                int ncopy =  (buffer.end()-4) - (buffer.begin()+cur);
                bufferSize -= ncopy;
                cur += ncopy;
            }
            if (bufferSize <= LOW_BOUND_LOAD_BUFFER) {
                LoadBuffer(reader);
            }
        }
        else {
            // we found start code
            auto end = buffer.end();
            if (code4 != buffer.end() && code3 != buffer.end()) {
                end = (code4 > code3 ? code3 : code4);
            }
            else {
                end = (code4 == buffer.end() ? code3 : code4);
            }
            int naluCur = nalu.data.size();
            int ntocopy = end - (buffer.begin()+cur);
            nalu.data.resize(naluCur+ntocopy);
            // copy(buffer.begin()+cur, end, back_inserter(nalu.data));
            memcpy(nalu.data.data()+naluCur, buffer.data()+cur, ntocopy);
            int ncopy = end - (buffer.begin()+cur);
            cur += ncopy;
            nextNaluFound = true;
        }
    }

    return true;
}



NaluParser::NaluParser() : buffer(BUFFER_SIZE), cur(BUFFER_SIZE) {}