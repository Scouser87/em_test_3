//
//  Utils.cpp
//  HoF-Cocos2dx
//
//  Created by Sergei Makarov on 08.04.16.
//
//

#include "Utils.hpp"
#include "pugixml.hpp"
#include <stdio.h>
#include <stdarg.h>
#include <set>
#include "CDirector.h"
#include "TexturePackerLoader.h"
#include "CAnimation.h"
#include "FileLoader.hpp"

std::string G_StringFormat(const std::string fmt, ...)
{
    std::string s;
    int n, size=fmt.size()*2;
    bool b=false;
    va_list marker;
    while (!b)
    {
        s.resize(size);
        va_start(marker, fmt);
        n = vsnprintf((char*)s.c_str(), size, fmt.c_str(), marker);
        va_end(marker);
        if ((n>0) && ((b=(n<size))==true)) s.resize(n); else size*=2;
    }
    return s;
}

CSprite* CreateAnimationFromPlist(const std::string& path, float fps)
{
    std::set<std::string> frames;
    
    CTexturePackerLoader& textPacker = CDirector::GetDirector().GetPackerManager()->GetTexturePacker(path);
    
    CAnimation* animation = textPacker.CreateAnimation();
    animation->SetAnimationSpeed(fps);
    
    return animation;
}

void GetAnimFilesFromPlist(const std::string& path, std::unordered_set<std::string>& array)
{
    
    std::string pngFileName = path + ".json";
    if (IsFileExist(pngFileName))
    {
        array.insert(path + ".png");
    }
    else
    {
        int index = 1;
        pngFileName = path + G_StringFormat("%d.plist", index);
        while (IsFileExist(pngFileName))
        {
            array.insert(path + G_StringFormat("%d.png", index));
            
            ++index;
            pngFileName = path + G_StringFormat("%d.plist", index);
        }
    }
}

CSprite* CreateAnimationFromXml(const std::string& path, float fps)
{
    CAnimation* anim = new CAnimation();
    anim->AddFrameXML(path + "/imageOffset.xml");
    anim->SetAnimationSpeed(fps);
    
    return anim;
}

void GetAnimFilesFromXml(const std::string& path, std::unordered_set<std::string>& array)
{
    pugi::xml_document doc;
    std::string fileContent = gReadFile((path + "/imageOffset.xml").c_str(), "rb");
    
    pugi::xml_parse_result result = doc.load(fileContent.c_str());
    
    if (result.status == pugi::status_ok)
    {
        pugi::xml_node imageSize = doc.child("imageSize");
        pugi::xml_node image = imageSize.first_child();
        while (image)
        {
            const char* name = image.attribute("name").as_string();
            
            if (name)
                array.insert(path + "/" + name);
            
            image = image.next_sibling();
        }
    }
}
