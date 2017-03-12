//
//  TexturePackerLoder.cpp
//  HouseofFun_2_3
//
//  Created by Andrey Sulnichenko on 07.04.14.
//
//

#include "TexturePackerLoader.h"
#include "json.h"
#include "CAnimation.h"
#include "FileLoader.hpp"
#include "Trace.hpp"
#include "Utils.hpp"

#define ToStr(a) ToStdStr(a).c_str()

CTexturePackerLoader::CTexturePackerLoader(const std::string& path, bool activate):
m_path(path)
{
    if(IsFileExist(path + ".json"))
        ParseJson(path, activate);
    else
    {
        int i = 1;
        while (IsFileExist(path + ToStr(i) + ".json")) {
            ParseJson(path + ToStr(i), activate);
            i++;
        }
    }
}

void CTexturePackerLoader::ParseJson(const std::string& path, bool activate)
{
    Json::Reader reader;
    Json::Value jason;
    
    std::string fileContent = gReadFile(path + ".json", "rb");
    
    if (!reader.parse(fileContent.c_str(), jason))
        return;

    sMetaData metaData;
    const Json::Value& meta = jason["meta"];
    if (!meta.isNull())
    {
        metaData.app     = meta["app"].asString();
        metaData.version = meta["version"].asString();
        metaData.image   = meta["image"].asString();
        metaData.format  = meta["format"].asString();

        const Json::Value& size = meta["size"];
        metaData.size.x  = size["w"].asInt();
        metaData.size.y  = size["h"].asInt();

        if (!jason.GetValueByPath("meta,manualScale").isNull()){
            metaData.manualScale = jason.GetValueByPath("meta,manualScale").asDouble();
        }
    }
    m_metaData.push_back(metaData);

    const Json::Value& frames = jason["frames"];
    if (!frames.isNull())
    {
        for (Json::Value::const_iterator it = frames.begin(); it != frames.end(); ++it)
        {
            const Json::Value& frameIterator = *it;

            sFrameInfo frame;

            const Json::Value& frameInfo = frameIterator["frame"];
            frame.framePos.x  = frameInfo["x"].asInt();
            frame.framePos.y  = frameInfo["y"].asInt();
            frame.frameSize.x = frameInfo["w"].asInt();
            frame.frameSize.y = frameInfo["h"].asInt();

            frame.rotated = frameIterator["rotated"].asBool();
            frame.trimmed = frameIterator["trimmed"].asBool();

            const Json::Value& spriteSourceSize = frameIterator["spriteSourceSize"];
            frame.spriteSourceSizePos.x = spriteSourceSize["x"].asInt();
            frame.spriteSourceSizePos.y = spriteSourceSize["y"].asInt();
            frame.spriteSourceSize.x    = spriteSourceSize["w"].asInt();
            frame.spriteSourceSize.y    = spriteSourceSize["h"].asInt();

            const Json::Value& sourceSize = frameIterator["sourceSize"];
            frame.sourceSize.x = sourceSize["w"].asInt();
            frame.sourceSize.y = sourceSize["h"].asInt();

            frame.metaDataIndx = m_metaData.size() - 1;

            const std::string& filename = frameIterator["filename"].asString();
            m_frames[filename] = frame;
        }
    }
    
    std::string theFilename = GetFolder(path, true) + metaData.image;
    if (activate)
        m_textures[metaData.image] = new CImage(theFilename);
    else
        m_textures[metaData.image] = new CImage(theFilename);

    m_loadList.push_back(theFilename);
}

const sFrameInfo* CTexturePackerLoader::GetFrameInfo(const std::string& name)
{
    sFrameInfo* data(NULL);
    if (m_frames.find(name) != m_frames.end())
        data = &m_frames[name];
    else if (m_frames.find(name + ".webp") != m_frames.end())
        data = &m_frames[name + ".webp"];
    else if (m_frames.find(name + ".png") != m_frames.end())
        data = &m_frames[name + ".png"];
    else if (m_frames.find(name + ".jpg") != m_frames.end())
        data = &m_frames[name + ".jpg"];
    
    return data;
}

void CTexturePackerLoader::ApplySpriteParameters(const std::string& name, CSprite* sprite)
{
    const sFrameInfo* frame(GetFrameInfo(name));
    if (frame)
        ApplySpriteParameters(sprite, *frame, m_metaData[frame->metaDataIndx]);
}

CSprite* CTexturePackerLoader::GetSprite(const std::string& name, std::string alternativePath)
{
    CSprite* result(NULL);
    const sFrameInfo* frame(GetFrameInfo(name));

    if (frame)
    {
        result = new CSprite(m_textures[m_metaData[frame->metaDataIndx].image]);
        ApplySpriteParameters(result, *frame, m_metaData[frame->metaDataIndx]);
    }
    else if (alternativePath != "")
        result =  new CSprite(alternativePath);
    else
        ALOG("Wrong frame name: \"%s\"", name.c_str());
    
    return result;
}

CSprite*  CTexturePackerLoader::GetSprite(const std::string& name, const sFrameInfo*& frameInfo)
{
    CSprite* result(NULL);
    frameInfo = GetFrameInfo(name);
    if (frameInfo)
    {
        result = new CSprite(m_textures[m_metaData[frameInfo->metaDataIndx].image]);
        ApplySpriteParameters(result, *frameInfo, m_metaData[frameInfo->metaDataIndx]);
        
        
    }
    else
        ALOG("Wrong frame name: \"%s\"", name.c_str());
    
    return result;
}

CImage* CTexturePackerLoader::GetImage(const std::string& name)
{
    const sFrameInfo* frame(GetFrameInfo(name));
    if (frame)
        return m_textures[m_metaData[frame->metaDataIndx].image];

    return NULL;
}

CImage*  CTexturePackerLoader::GetImage(const std::string& name, const sFrameInfo*& frameInfo)
{
    frameInfo = GetFrameInfo(name);;
    if (frameInfo)
        return m_textures[m_metaData[frameInfo->metaDataIndx].image];
    
    return NULL;
}

void CTexturePackerLoader::ApplySpriteParameters(CSprite* result, const sFrameInfo& info, const sMetaData& metaData)
{
    float shiftX = 0, shiftY = 0;
    
    if(info.trimmed)
    {
        shiftX = info.spriteSourceSizePos.x + (info.spriteSourceSize.x-info.sourceSize.x)*0.5;
        shiftY = -(info.spriteSourceSizePos.y + (info.spriteSourceSize.y-info.sourceSize.y)*0.5);
    }
    if(info.rotated)
    {
        result->SetSize(info.frameSize);
        vec2 from, to;
        from = info.framePos;    from.x /= metaData.size.x;    from.y /= metaData.size.y;
        to = info.framePos + vec2(info.frameSize.y, info.frameSize.x);    to.x /= metaData.size.x;    to.y /= metaData.size.y;
        
        result->SetTextureCoord(from, to);
        result->TextureCoordRotate90();
    }
    else
    {
        result->SetSize(info.frameSize);
        vec2 from, to;
        from = info.framePos;    from.x /= metaData.size.x;    from.y /= metaData.size.y;
        to = info.framePos + info.frameSize;    to.x /= metaData.size.x;    to.y /= metaData.size.y;
        
        result->SetTextureCoord(from, to);
    }
    
    result->SetAnchorPoint(vec2(-shiftX / result->GetSize().x + 0.5, -shiftY / result->GetSize().y + 0.5));
    if (metaData.manualScale != 1.0f){
        result->SetScale(metaData.manualScale);
    }
}

CAnimation* CTexturePackerLoader::CreateAnimation(std::string mask, int from, int to)
{
    CAnimation* anim = new CAnimation;
    if (mask == "")
    {
        std::map< std::string, sFrameInfo >::iterator i;
        for (i = m_frames.begin(); i != m_frames.end(); i++)
        {
            anim->AddFrame(m_textures[m_metaData[i->second.metaDataIndx].image], i->second, m_metaData[i->second.metaDataIndx]);
        }
    }
    else
    {
        for (int i = from; i <= to; i++)
        {
            std::string frameName = G_StringFormat(mask.c_str(), i);
            if (m_frames.find(frameName) == m_frames.end())
            {
                ALOG("Wrong frame name: \"%s\"", frameName.c_str());
                anim->AddFrame(NULL, vec2(), vec2(), vec2());
            }
            else
            {
                sFrameInfo& data = m_frames[frameName];
                anim->AddFrame(m_textures[m_metaData[data.metaDataIndx].image], data, m_metaData[data.metaDataIndx]);
            }
        }
    }
    return anim;
}

CTexturePackerManager::CTexturePackerManager()
{
    
}

CTexturePackerManager::~CTexturePackerManager()
{
    std::map<std::string,CTexturePackerLoader*>::iterator it = m_loadersList.begin();
    for(; it != m_loadersList.end(); it++)
        delete it->second;
    m_loadersList.clear();
}

CTexturePackerManager& CTexturePackerManager::Get()
{
    static CTexturePackerManager mngr;
    return mngr;
}

CSprite* CTexturePackerManager::GetSpriteOfPacker(const std::string& path)
{
    size_t found = path.rfind("/");
    std::string temp = path;
    std::string _path = temp.substr(0,found);
    temp = path;
    std::string _name = temp.substr(found+1);
    return Get().GetTexturePacker(_path).GetSprite(_name);
}

void CTexturePackerManager::SetImageSpriteOfPacker(CSprite* sprite, const std::string& path)
{
     if (CSprite* sp = CTexturePackerManager::GetSpriteOfPacker(path))
     {
          sprite->SetImage(sp->GetImage());
          sprite->AddChild(sp);
          sp->Delete();
     }
    size_t found = path.rfind("/");
    std::string temp = path;
    std::string _path = temp.substr(0,found);
    temp = path;
    std::string _name = temp.substr(found+1);
    Get().GetTexturePacker(_path).ApplySpriteParameters(_name, sprite);
}

CTexturePackerLoader& CTexturePackerManager::GetTexturePacker(const std::string& path, bool activate)
{
    std::map<std::string,CTexturePackerLoader*>::iterator it;
    it=m_loadersList.find(path);
	if(it != m_loadersList.end())
        return *it->second;
    
    CTexturePackerLoader *packer = new CTexturePackerLoader(path, activate);
    m_loadersList[path] = packer;
    return *m_loadersList[path];
}

void CTexturePackerManager::Clear()
{
    std::map<std::string,CTexturePackerLoader*>::iterator it = m_loadersList.begin();
    for(; it != m_loadersList.end(); it++)
        delete it->second;
    m_loadersList.clear();
}
