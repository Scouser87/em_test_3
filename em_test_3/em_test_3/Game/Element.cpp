//
//  Element.cpp
//  em_test_3
//
//  Created by Sergei Makarov on 12.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#include "Element.hpp"
#include "Utils.hpp"

CSprite* Element::createElementLayer(const int& elementId, pugi::xml_node layerInfo)
{
    CSprite* result = NULL;
    if (layerInfo)
    {
        std::string type = layerInfo.attribute("type").as_string();
        if (type.compare("image") == 0)
        {
            std::string path = G_StringFormat("res/game_75/GameElements/%d/%s.png", elementId, layerInfo.attribute("name").as_string());
            result = new CSprite(path);
        }
        else if(type.compare("texturePacker") == 0)
        {
            std::string path = layerInfo.attribute("name").as_string();
            if (path.find("ForAll") == std::string::npos)
                path = G_StringFormat("res/game_75/GameElements/%d/%s", elementId, layerInfo.attribute("name").as_string());
            else
                path = G_StringFormat("res/game_75/GameElements/%s", layerInfo.attribute("name").as_string());
            result = CreateAnimationFromPlist(path);
        }
        else if(type.compare("animation") == 0)
        {
            std::string name = (layerInfo.attribute("name") ? layerInfo.attribute("name").as_string() : layerInfo.name());
            std::string path("");
            if (name.find("highlight") != std::string::npos)
                path = "res/game_75/GameElements/ForAll/Highlight";
            else if (name.find("ForAll") == std::string::npos)
                path = G_StringFormat("res/game_75/GameElements/%d/%s", elementId, name.c_str());
            else
                path = G_StringFormat("res/game_75/GameElements/%s", name.c_str());
            result = CreateAnimationFromXml(path);
        }
        if (result)
        {
            pugi::xml_attribute shift = layerInfo.attribute("shift");
            if (shift)
            {
                vec2 shiftVal;
                sscanf(shift.as_string(), "%f,%f", &shiftVal.x, &shiftVal.y);
                result->SetPosition(shiftVal);
            }
            pugi::xml_attribute scale = layerInfo.attribute("scale");
            if (scale)
            {
                vec2 scaleVal;
                sscanf(scale.as_string(), "%f,%f", &scaleVal.x, &scaleVal.y);
                result->SetScale(scaleVal);
            }
        }
    }
    
    return result;
}

void Element::GetAnimFilesFromLayer(const int& elementId, pugi::xml_node layerInfo, std::unordered_set<std::string>& array)
{
    if (layerInfo)
    {
        std::string type = layerInfo.attribute("type").as_string();
        if (type.compare("image") == 0)
        {
            ;
        }
        else if(type.compare("texturePacker") == 0)
        {
            std::string path = layerInfo.attribute("name").as_string();
            if (path.find("ForAll") == std::string::npos)
                path = G_StringFormat("res/game_75/GameElements/%d/%s", elementId, layerInfo.attribute("name").as_string());
            else
                path = G_StringFormat("res/game_75/GameElements/%s", layerInfo.attribute("name").as_string());
            GetAnimFilesFromPlist(path, array);
        }
        else if(type.compare("animation") == 0)
        {
            std::string name = (layerInfo.attribute("name") ? layerInfo.attribute("name").as_string() : layerInfo.name());
            std::string path("");
            if (name.find("highlight") != std::string::npos)
                path = "res/game_75/GameElements/ForAll/Highlight";
            else if (name.find("ForAll") == std::string::npos)
                path = G_StringFormat("res/game_75/GameElements/%d/%s", elementId, name.c_str());
            else
                path = G_StringFormat("res/game_75/GameElements/%s", name.c_str());
            GetAnimFilesFromXml(path, array);
        }
    }
}

void Element::GetAnimFiles(const int& elementId, pugi::xml_node info, std::unordered_set<std::string>& array)
{
    Element::GetAnimFilesFromLayer(elementId, info.child("active"), array);
    Element::GetAnimFilesFromLayer(elementId, info.child("highlight"), array);
}

Element::Element(pugi::xml_node info)
{
    m_normal = NULL;
    m_active = NULL;
    m_highlight = NULL;
    
    SetAnchorPoint(vec2(0.5f, 0.5f));
    ParseInfo(info);
    SetState(eLS_normal);
}

void Element::ParseInfo(pugi::xml_node info)
{
    m_id = info.attribute("ID").as_int();
    
    m_info = info;
    
    m_normal = Element::createElementLayer(m_id, info.child("normal"));
    AddChild(m_normal);
}

void Element::SetState(eElementState state)
{
    if (state == eLS_normal)
    {
        m_normal->SetVisible(true);
        if (m_active)
        {
            m_active->SetVisible(false);
            m_active->Delete();
            m_active = NULL;
        }
        if (m_highlight)
        {
            m_highlight->SetVisible(false);
            m_highlight->Delete();
            m_highlight = NULL;
        }
    }
    else if (state == eLS_active)
    {
        m_normal->SetVisible(false);
        if (!m_active)
        {
            m_active = Element::createElementLayer(m_id, m_info.child("active"));
            if(m_active)
                AddChild(m_active);
            else
            {
                m_active = Element::createElementLayer(m_id, m_info.child("normal"));
                AddChild(m_active);
            }
        }
        if(m_active)
            m_active->SetVisible(true);
        if (!m_highlight)
        {
            m_highlight = Element::createElementLayer(m_id, m_info.child("highlight"));
            if(m_highlight)
                AddChild(m_highlight);
        }
        if (m_highlight)
            m_highlight->SetVisible(true);
    }
    m_state = state;
}