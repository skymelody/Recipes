//
// Created by skymelody on 2019/12/24.
//

#ifndef SHARK2FILE_FILEUTILS_H
#define SHARK2FILE_FILEUTILS_H

#include <string>

#if defined(__linux__)
#define PATH_SEPARATOR "/"
#elif defined(_WIN32)
#define PATH_SEPARATOR "\\"
#endif


namespace FileSystem {
    std::string Current();
    bool IsDir(const std::string& dirPath);
    bool Exist(const std::string& dirPath);
    bool MakeDir(std::string path);

    /*
                    12345

                     rtp

                src        dest

            ssrc_file1    ssrc_file2
    */
    std::string GenerateDir(const std::string& root,
                            const std::string& packetType,
                            const std::string& ip,
                            bool src);
}



#endif //SHARK2FILE_FILEUTILS_H
