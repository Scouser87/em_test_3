//
//  GraphicsLoader.hpp
//  em_test_3
//
//  Created by Sergei Makarov on 12.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#ifndef GraphicsLoader_hpp
#define GraphicsLoader_hpp

#include "CSprite.h"
#include "pugixml.hpp"

class GraphicsLoader : public CSprite
{
public:
    GraphicsLoader(const std::string& path);
    
    void        LoadFromPath(const std::string& path);
    CSprite*       FindNodeByName(const std::string& name);
    std::string GetImgFilePath(const std::string& path);
        
private:
    void ParseObject(CSprite* parent, pugi::xml_node object);
    
    std::string m_loaderPath;
    std::map<std::string, CSprite*> m_nodes;
    
    bool m_isCentered;
};

#endif /* GraphicsLoader_hpp */
