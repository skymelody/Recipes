//
// Created by skymelody on 2019/12/21.
//

#ifndef SHARK2FILE_LINUX_PROGRAM_H
#define SHARK2FILE_LINUX_PROGRAM_H

#include "Shark2File/Common/ErrorCode.h"
#include <vector>
#include <string>

class ProgramImpl {
public:
    using Args = std::vector<std::string>;

protected:
    ProgramImpl(const std::string& cmd, const Args& args);

    void AddArgImpl(const std::string& arg);
    ErrorCode ExecuteImpl();
private:
    std::string cmd_;
    Args args_;
};


#endif //SHARK2FILE_PROGRAMIMPL_H
