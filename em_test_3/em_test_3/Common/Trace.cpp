//
//  Trace.cpp
//  em_test_3
//
//  Created by Sergei Makarov on 09.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#include "Trace.hpp"

namespace tracefunctions
{
    std::string stringwithnameofvarible;
    
    void print(int x) { printf("%s = %i\n", stringwithnameofvarible.c_str(), x); }
    void print(unsigned int x) { printf("%s = %i\n", stringwithnameofvarible.c_str(), x); }
    void print(long x) { printf("%s = %li\n", stringwithnameofvarible.c_str(), x); }
    void print(unsigned long x) { printf("%s = %lu\n", stringwithnameofvarible.c_str(), x); }
    void print(long long x) { printf("%s = %lli\n", stringwithnameofvarible.c_str(), x); }
    void print(unsigned long long x) { printf("%s = %llu\n", stringwithnameofvarible.c_str(), x); }
    void print(bool x) { printf("%s = %s\n", stringwithnameofvarible.c_str(), (x) ? "true" : "false"); }
    void print(double x) { printf("%s = %f\n", stringwithnameofvarible.c_str(), x); }
    void print(const char* x) { printf("%s = %s\n", stringwithnameofvarible.c_str(), x); }
    void print(char* x) { printf("%s = %s\n", stringwithnameofvarible.c_str(), x); }
    void print(std::string x) { printf("%s = %s\n", stringwithnameofvarible.c_str(), x.c_str()); }
};