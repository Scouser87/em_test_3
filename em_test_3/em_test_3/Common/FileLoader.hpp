//
//  FileLoader.hpp
//  em_test_3
//
//  Created by Sergei Makarov on 09.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#ifndef FileLoader_hpp
#define FileLoader_hpp

#include <stdio.h>
#include <string>

std::string gReadFile(const char* fileName, const char* params);

unsigned int png_texture_load(const char * file_name, int & width, int & height);

#endif /* FileLoader_hpp */
