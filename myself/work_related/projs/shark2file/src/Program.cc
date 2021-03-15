//
// Created by skymelody on 2019/12/21.
//

#include "Shark2File/Common/Program.h"

Program::Program(const std::string &cmd, const ProgramImpl::Args args) :
    ProgramImpl(cmd, args) {}

ErrorCode Program::Execute() {
    return ExecuteImpl();
}

void Program::AddArg(const std::string &arg) {
    AddArgImpl(arg);
}

