//
// Created by skymelody on 2019/12/21.
//

#include "Shark2File/Common/WinProgram.h"
#include <memory>
#include <windows.h>

#ifdef __linux__
#define CLRF "\n"
#else
#define CLRF "\r\n"
#endif

ProgramImpl::ProgramImpl(const std::string& cmd,
                         const Args& args) :
                         cmd_(cmd), args_(args){
    args_.push_back(cmd);
}

void ProgramImpl::AddArgImpl(const std::string &arg) {
    args_.push_back(arg);
}

ErrorCode ProgramImpl::ExecuteImpl() {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    std::string cmd;
    for (int i = 0; i < args_.size(); ++i) {
        if (i != 0) {
            cmd.append(" ");
        }
        cmd.append(args_[i]);
    }

    int n = cmd.size();
    std::unique_ptr<char[]> path(new char[n+1]);
    memmove(path.get(), cmd.c_str(), cmd.size());
    path[n] = '\0';

    if (!CreateProcess(nullptr, path.get(), nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi)) {
        ::printf("ִ��Wiresharkʧ��"
            CLRF);
        return ErrorCode::FAILED;
    }

    SetConsoleCtrlHandler(nullptr, TRUE);
    WaitForSingleObject(pi.hProcess, INFINITE);
    SetConsoleCtrlHandler(nullptr, FALSE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);



    return ErrorCode::OK;
}

