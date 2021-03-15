//
// Created by skymelody on 2019/12/21.
//


#include "Shark2File/Common/LinuxProgram.h"
#include "Shark2File/Common/Log.h"
#include <unistd.h>
#include <sys/wait.h>
#include <memory>




ProgramImpl::ProgramImpl(const std::string& cmd,
                         const Args& args) :
                         cmd_(cmd), args_(args){
    args_.push_back(cmd);
}

void ProgramImpl::AddArgImpl(const std::string &arg) {
    args_.push_back(arg);
}

ErrorCode ProgramImpl::ExecuteImpl(){
    int childid = fork();
    // child
    if (childid == 0) {
        const int nargs = args_.size();
        std::unique_ptr<char*[]> args(new char*[nargs+1]);
        for (int i = 0; i < nargs; ++i) {
            args[i] = const_cast<char*>(args_[i].c_str());
        }
        args[nargs] = nullptr;



        if (execv(cmd_.c_str(), args.get()) != 0) {
        	LOG("Tshark Exec Failed"
			    CLRF);
            exit(-1);
        }
        exit(0);
    }
    else if (childid > 0) {
        signal(SIGINT, SIG_IGN);
        int childStatus = 0;
        int pid = waitpid(childid, &childStatus, 0);
        signal(SIGINT, SIG_DFL);
        if (pid != childid) {
            return ErrorCode::FAILED;
        }
        if (WEXITSTATUS(childStatus) == (uint8_t)-1) {
            return ErrorCode::FAILED;
        }
    }
    else {
        return ErrorCode::FAILED;
    }
    return ErrorCode::OK;
}

