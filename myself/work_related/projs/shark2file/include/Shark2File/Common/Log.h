//
// Created by skymelody on 2020/1/16.
//

#ifndef SHARK2FILE_LOG_H
#define SHARK2FILE_LOG_H
#include <cstdio>

#ifdef __linux__
#define CLRF "\n"
#else
#define CLRF "\r\n"
#endif

#define LOG ::printf


#endif //SHARK2FILE_LOG_H
