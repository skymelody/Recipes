//
// Created by skymelody on 2019/12/21.
//

#ifndef SHARK2FILE_PROGRAM_H
#define SHARK2FILE_PROGRAM_H

#if defined(__linux__)
#include "Shark2File/Common/LinuxProgram.h"
#elif defined(_WIN32)
#include "Shark2File/Common/WinProgram.h"
#endif


class Program : private ProgramImpl {
public:
    explicit Program(const std::string& cmd, const Args args=Args());
    Program(const Program&) = delete;
    Program& operator=(const Program&) = delete;

    ErrorCode Execute();
    void AddArg(const std::string& arg);
};


#endif //SHARK2FILE_PROGRAM_H
