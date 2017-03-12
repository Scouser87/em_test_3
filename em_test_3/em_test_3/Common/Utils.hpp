//
//  Utils.hpp
//  HoF-Cocos2dx
//
//  Created by Sergei Makarov on 08.04.16.
//
//

#ifndef Utils_hpp
#define Utils_hpp

#include <stdio.h>
#include <unordered_set>
#include "CSprite.h"

std::string G_StringFormat(const std::string fmt, ...);

CSprite* CreateAnimationFromPlist(const std::string& path, float fps = 24.0f);
void GetAnimFilesFromPlist(const std::string& path, std::unordered_set<std::string>& array);

CSprite* CreateAnimationFromXml(const std::string& path, float fps = 24.0f);
void GetAnimFilesFromXml(const std::string& path, std::unordered_set<std::string>& array);

#endif /* Utils_hpp */
