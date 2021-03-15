#include "http_module/transcoding_request_handler.h"
#include "http_module/upload_file_handler.h"
#include "video_module/ffmpeg_video_converter.h"
#include "video_module/video.h"
#include "config_module/configuration.h"
#include "Poco/Net/HTMLForm.h"


using namespace std;
using namespace Poco::Net;

#include <iostream>

void TranscodingRequestHandler::handleRequest (
        Poco::Net::HTTPServerRequest &request, 
        Poco::Net::HTTPServerResponse &response
    ) 
{
    UploadFileHandler handler;
    HTMLForm form(request, request.stream(), handler);

    string convertType = form.get("type");
    const Configuration& config = GetConfigutation();

    /* 判断要转换的格式是不是支持的格式 */
    if (config.supportedVideoTypes.count(convertType)) {
        string inputPath = handler.GetOriginalFilePath();
        string outputPath = config.convertedFileSavedDirectory + handler.GetFileName() + convertType;
        Video input(inputPath);
        Video output(outputPath);
        FFMpegVideoConverter converter;
        if (converter.Convert(input, output)) {

        }
        else {
            cout << "failed" << endl;
        }
    }
    else {

    }
    response.send().flush();
    response.setStatus(HTTPServerResponse::HTTP_OK);

}
