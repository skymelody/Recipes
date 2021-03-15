#ifndef UPLOAD_FILE_HANDLER_INCLUDED
#define UPLOAD_FILE_HANDLER_INCLUDED
#include <Poco/Net/PartHandler.h>
#include <string>

/* 
    该类的功能是:
    1. 获取上传的视频文件的文件名和文件类型。
    2. 将上传的视频文件保存至本地。
*/
class UploadFileHandler : public Poco::Net::PartHandler {
public:
    void handlePart(const Poco::Net::MessageHeader &header, std::istream &stream) override;
    std::string GetFileName() const;
    std::string GetOriginalFilePath() const;

private:
    void SaveToDisk(std::istream& stream);

private:
    std::string _fileName;
    std::string _fileType;
    std::string _fileSavedPath;

};

#endif