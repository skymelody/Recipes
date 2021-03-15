#ifndef VIDEO_INCLUDED
#define VIDEO_INCLUDED

#include <string>

enum VideoType {
    MP4, MKV, FLV
};

class Video {
public:
    Video();
    Video(const std::string& filePath);

    std::string GetFilePath() const;
    void SetFilePath(const std::string& filePath);
private:
    std::string _filePath;
};

inline Video::Video() {}
inline Video::Video(const std::string& filePath): _filePath(filePath) {}

inline std::string Video::GetFilePath() const {
    return _filePath;
}

inline void Video::SetFilePath(const std::string& filePath) {
    _filePath = filePath;
}

#endif