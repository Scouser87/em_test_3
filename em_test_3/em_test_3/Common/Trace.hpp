//
//  Trace.hpp
//  em_test_3
//
//  Created by Sergei Makarov on 09.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#ifndef Trace_hpp
#define Trace_hpp

#include <stdio.h>
#include <string>

void G_CustomLog(int _lvl, int _extraFlags, const char* type, const char* s_function, int s_line, const char * sFmt,...);
#define LOG(...) {G_CustomLog(0, 0, "ALL",__func__,__LINE__,__VA_ARGS__);}
#define LOGeX(_lvl, _extraFlags, ...) {G_CustomLog(_lvl, _extraFlags, "ALL",__func__,__LINE__,__VA_ARGS__);}
#define ALOG(...) {G_CustomLog(0, 0, "FRAMEWORK",__func__,__LINE__,__VA_ARGS__);}
#define ALOGeX(_lvl, _extraFlags, ...) {G_CustomLog(_lvl, _extraFlags, "FRAMEWORK",__func__,__LINE__,__VA_ARGS__);}
#define GLOG(...) {G_CustomLog(0, 0, "GAME",__func__,__LINE__,__VA_ARGS__);}
#define GLOGeX(_lvl, _extraFlags, ...) {G_CustomLog(_lvl, _extraFlags, "GAME",__func__,__LINE__,__VA_ARGS__);}
#define NLOG(...) {G_CustomLog(0, 0, "NETWORK",__func__,__LINE__,__VA_ARGS__);}
#define NLOGeX(_lvl, _extraFlags, ...) {G_CustomLog(_lvl, _extraFlags, "NETWORK",__func__,__LINE__,__VA_ARGS__);}
#define SLOG(...) {G_CustomLog(0, 0, "SERVER",__func__,__LINE__,__VA_ARGS__);}
#define SLOGeX(_lvl, _extraFlags, ...) {G_CustomLog(_lvl, _extraFlags, "SERVER",__func__,__LINE__,__VA_ARGS__);}
#define OS_LOG(...) {G_CustomLog(0, 0, "SYSTEM",__func__,__LINE__,__VA_ARGS__);}
#define OS_LOGeX(_lvl, _extraFlags, ...) {G_CustomLog(_lvl, _extraFlags, "SYSTEM",__func__,__LINE__,__VA_ARGS__);}
#define TLOG(...) {G_CustomLog(0, 0, "TRACE",__func__,__LINE__,__VA_ARGS__);}
#define TLOGeX(_lvl, _extraFlags, ...) {G_CustomLog(_lvl, _extraFlags, "TRACE",__func__,__LINE__,__VA_ARGS__);}

#define C_LOG(type,...) {G_CustomLog(0, 0, type,__func__,__LINE__,__VA_ARGS__);}

#define TRACE(x)  { tracefunctions::stringwithnameofvarible=#x; tracefunctions::print(x);}

namespace tracefunctions
{
    extern std::string stringwithnameofvarible;
    
    void print(int x);
    void print(unsigned int x);
    void print(long x);
    void print(unsigned long x);
    void print(long long x);
    void print(unsigned long long x);
    void print(bool x);
    void print(double x);
    void print(const char* x);
    void print(char* x);
    void print(std::string x);    
};

#endif /* Trace_hpp */
