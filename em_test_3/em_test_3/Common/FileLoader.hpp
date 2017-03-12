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

std::string gReadFile(std::string fileName, const char* params);
bool IsFileExist(std::string fileName);
std::string GetFolder(const std::string& _path, bool shouldLeaveSeparator);

unsigned int png_texture_load(const char * file_name, int & width, int & height);

#endif /* FileLoader_hpp */
