#include "http_module/upload_file_handler.h"
#include "config_module/configuration.h"
#include "Poco/Net/NameValueCollection.h"
#include "Poco/Net/MessageHeader.h"
#include <fstream>
#include <chrono>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;
using namespace chrono;
using namespace Poco;
using namespace Poco::Net;


/* 从请求头中获得文件名和其类型，并返回。比如获取 "test.mp4" */
string ExtractFileNameAndTypeFromHeader(const MessageHeader& header) {
    if (header.has("Content-Disposition")) {
        string disp;
        NameValueCollection params;
        MessageHeader::splitParameters(header["Content-Disposition"], disp, params);

        string fileNameAndType = params.get("filename", "unname");

        return fileNameAndType;
    }
    return "";
}


string UploadFileHandler::GetFileName() const {
    return _fileName;
}

string UploadFileHandler::GetOriginalFilePath() const {
    return _fileSavedPath;
}

void UploadFileHandler::SaveToDisk(istream& stream) {
    ofstream outStream(_fileSavedPath, ios::out);
    char buf[4096];
    while (!stream.eof()) {
        stream.read(buf, 4086);
        outStream.write(buf, stream.gcount());
    }
    outStream.flush();
    outStream.close();
}

/* 
    返回当前时间的纳秒数的字符串
*/

static void CreateDir(const std::string& filePath) {
    auto pos = filePath.find_last_of('/');
    string dir = filePath.substr(0, pos);
    if (opendir(dir.c_str()) == nullptr) {
        mkdir(dir.c_str(), 0777);
    }
}

string CurrentTimeToStr() {
    return to_string(duration_cast<nanoseconds>
        (steady_clock::now().time_since_epoch())
        .count());
}

void UploadFileHandler::handlePart(const MessageHeader &header, istream &stream) {

    const Configuration& config = GetConfigutation();
    string fileNameAndType = ExtractFileNameAndTypeFromHeader(header);
    string timeNow = CurrentTimeToStr();

    if (fileNameAndType.empty()) {
        return;
    }
    int pos = fileNameAndType.find('.');
    _fileName = fileNameAndType.substr(0, pos) + timeNow;

    if (pos != string::npos) {
        _fileType = fileNameAndType.substr(pos);
    }


    _fileSavedPath = config.uploadFileSavedDirectory +
                     _fileName + 
                    _fileType;
    SaveToDisk(stream);

}