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
