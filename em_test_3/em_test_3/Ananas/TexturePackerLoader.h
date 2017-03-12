//
//  TexturePackerLoder.h
//  HouseofFun_2_3
//
//  Created by Andrey Sulnichenko on 07.04.14.
//
//

// This class implement need fuctional for work with TexturePacker( http://www.codeandweb.com/texturepacker )

#pragma once 

#include "CSprite.h"

#include <iostream>
#include <string>
#include <sstream>

template <typename T>
inline std::string ToStdStr(const T& a)
{
    std::ostringstream stream;
    stream << a;
    return stream.str();
}

class CAnimation;

typedef struct sFrameInfo
{
    int metaDataIndx;
    vec2 framePos;
    vec2 frameSize;
    bool rotated;
    bool trimmed;
    vec2 spriteSourceSizePos;
    vec2 spriteSourceSize;
    vec2 sourceSize;
} sFrameInfo;

struct sMetaData
{
    std::string app;
    std::string version;
    std::string image;
    std::string format;
    vec2        size;
    float       manualScale;
    
    sMetaData():
                size(0,0),
                manualScale(1.0f){}
};

class CTexturePackerLoader
{
public:
    CTexturePackerLoader(const std::string& path, bool activate = false /* we don't really need to activate textures here */);
    CSprite*  GetSprite(const std::string& name, std::string alternativePath="");
    CSprite*  GetSprite(const std::string& name, const sFrameInfo*& frameInfo); //return CSprite* and sFrameInfo* frameInfo
    CImage*  GetImage(const std::string& name);
    CImage*  GetImage(const std::string& name, const sFrameInfo*& frameInfo); //return CImage* and sFrameInfo* frameInfo
    const sFrameInfo* GetFrameInfo(const std::string& name);
    void ApplySpriteParameters(const std::string& name, CSprite* sprite);
    
    static void ApplySpriteParameters(CSprite* sprite, const sFrameInfo& info, const sMetaData& metaData);

    CAnimation* CreateAnimation(std::string mask = "", int from = 0, int to = 0);
    
    std::vector<std::string> GetLoadList() { return m_loadList; }
    
    const std::map<std::string, CImage*>&       GetTexturesMap()  { return m_textures; }
    const std::map< std::string, sFrameInfo >&  GetFramesMap()    { return m_frames; }
    const std::vector<sMetaData>&               GetMetaDataList() { return m_metaData; }

private:
    void ParseJson(const std::string& path, bool activate = true);
    
    std::map<std::string, CImage*>       m_textures;
    std::vector<std::string>             m_loadList;
    std::map< std::string, sFrameInfo >  m_frames;
    std::vector<sMetaData>               m_metaData;

public:
    std::string m_path;
};

class CTexturePackerManager
{
public:
    CTexturePackerManager();
    ~CTexturePackerManager();
    static CTexturePackerManager& Get();
    static CSprite* GetSpriteOfPacker(const std::string& path);
    static void SetImageSpriteOfPacker(CSprite* sprite, const std::string& path);
    
    CTexturePackerLoader& GetTexturePacker(const std::string&, bool activate = false /* we don't really need to activate textures here */);

    void Clear();

private:
    std::map<std::string, CTexturePackerLoader*> m_loadersList;
};
