//
//  Trace.cpp
//  em_test_3
//
//  Created by Sergei Makarov on 09.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#include "Trace.hpp"
#include <stdlib.h>
#include <stdarg.h>

#define LOG_BUFFER_SIZE 65536
char*   tflog_buffer = NULL;
size_t  tflog_buffer_size = 0;

void G_CustomLog(int _lvl, int _extraFlags, const char* type, const char* s_function, int s_line, const char * sFmt,...)
{
    if (type == NULL || s_function == NULL || sFmt == NULL)
        return;
    
    if (!tflog_buffer)
    {
        tflog_buffer_size = LOG_BUFFER_SIZE;
        tflog_buffer = (char*) malloc(tflog_buffer_size * sizeof(char));
    }
    
    int res = 0;
    do {
        
        if (res > tflog_buffer_size)
        {
            if(char* temp_buffer_ptr = (char*) realloc (tflog_buffer, tflog_buffer_size * 2 * sizeof(char)))
            {
                tflog_buffer = temp_buffer_ptr;
                tflog_buffer_size *= 2;
            }
            else
                break;  // can't allocate more memory, will write log partly
        }
        
        va_list ap;
        va_start( ap, sFmt );
        res = vsnprintf( tflog_buffer, tflog_buffer_size, sFmt, ap );
        va_end( ap );
        
    } while (res > tflog_buffer_size);
    
    printf("%s\n", tflog_buffer);
}

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