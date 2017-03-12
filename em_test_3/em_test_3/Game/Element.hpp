//
//  Element.hpp
//  em_test_3
//
//  Created by Sergei Makarov on 12.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#ifndef Element_hpp
#define Element_hpp

#include <unordered_set>
#include "CSprite.h"
#include "pugixml.hpp"

class Element : public CSprite
{
public:
    enum eElementState
    {
        eLS_normal,
        eLS_active
    };
    
    static CSprite* createElementLayer(const int& elementId, pugi::xml_node layerInfo);
    static void GetAnimFilesFromLayer(const int& elementId, pugi::xml_node layerInfo, std::unordered_set<std::string>& array);
    static void GetAnimFiles(const int& elementId, pugi::xml_node info, std::unordered_set<std::string>& array);
    
    Element(pugi::xml_node info);
    
    void ParseInfo(pugi::xml_node info);
    void SetState(eElementState state);
    
private:
    int m_id;
    
    eElementState m_state;
    
    CSprite* m_normal;
    CSprite* m_active;
    CSprite* m_highlight;
    
    pugi::xml_node m_info;
};

#endif /* Element_hpp */
