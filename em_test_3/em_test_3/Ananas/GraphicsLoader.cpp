//
//  GraphicsLoader.cpp
//  em_test_3
//
//  Created by Sergei Makarov on 12.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#include "GraphicsLoader.hpp"
#include "FileLoader.hpp"
#include "CButton.h"

GraphicsLoader::GraphicsLoader(const std::string& path)
{
    m_isCentered = false;
    LoadFromPath(path);
}

void GraphicsLoader::LoadFromPath(const std::string& path)
{
    size_t find = path.find(".xsp");
    if (find != std::string::npos)
        m_loaderPath = path.substr(0, find) + "/";
    pugi::xml_document doc;
    std::string fileContent = gReadFile(path.c_str(), "rb");
    
    pugi::xml_parse_result result = doc.load(fileContent.c_str());
    
    if (result.status == pugi::status_ok)
    {
        pugi::xml_node objects = doc.child("objects");
        SetName(objects.attribute("name").as_string());
        
        vec2 size(objects.attribute("width").as_float(), objects.attribute("height").as_float());
        SetSize(size);
        
        m_isCentered = objects.attribute("isCentered").as_bool();
        
        ParseObject(this, objects);
    }
}

CSprite* GraphicsLoader::FindNodeByName(const std::string& name)
{
    CSprite* result = NULL;
    auto it = m_nodes.find(name);
    if (it != m_nodes.end())
        result = (*it).second;
    return result;
}

std::string GraphicsLoader::GetImgFilePath(const std::string& path)
{
    std::string fileName = m_loaderPath + path;
    if (IsFileExist(fileName + ".png"))
        return fileName + ".png";
    else if(IsFileExist(fileName + ".jpg"))
        return fileName + ".jpg";
    return fileName;
}

void GraphicsLoader::ParseObject(CSprite* parent, pugi::xml_node object)
{
    pugi::xml_node child = object.last_child();
    while (child)
    {
        std::string name = child.name();
        if (name == "Group")
        {
            CSprite* newGroup = NULL;
            
            std::string groupName = child.attribute("name").as_string();
            vec2 pos(child.attribute("x").as_float(), child.attribute("y").as_float());
            
            if (groupName.find("btn_") != std::string::npos)
            {
                std::string btnPath[3];
                for (int i = 0; i < 3; ++i)
                    btnPath[i] = "";
                
                int index = 0;
                pugi::xml_node btnChild = child.first_child();
                while (btnChild)
                {
                    btnPath[index] = btnChild.attribute("name").as_string();
                    btnChild = btnChild.next_sibling();
                    ++index;
                }
                
                CButton* btn = new CButton;
                if (!btnPath[0].empty())
                    btn->SetNormal(new CSprite(GetImgFilePath(btnPath[0])));
                if (!btnPath[1].empty())
                    btn->SetPressed(new CSprite(GetImgFilePath(btnPath[1])));
                if (!btnPath[2].empty())
                    btn->SetDisabled(new CSprite(GetImgFilePath(btnPath[2])));
                
                btn->SetPosition(pos - parent->GetPosition());
                parent->AddChild(btn);
                
                newGroup = btn;
            }
            else
            {
                CSprite* layer = new CSprite;
                
                layer->SetName(groupName);
                
                vec2 size;
                sscanf(child.attribute("size").as_string(), "%f,%f", &size.width, &size.height);
                layer->SetSize(size);
                
                ParseObject(layer, child);
                
                parent->AddChild(layer);
                
                newGroup = layer;
            }
            
            m_nodes[groupName] = newGroup;
        }
        else if(name == "Sprite")
        {
            CSprite* newSprite = new CSprite(GetImgFilePath(child.attribute("name").as_string()));
            
            if (newSprite)
            {
                newSprite->SetName(child.attribute("name").as_string());
                
                vec2 pos(child.attribute("x").as_float(), child.attribute("y").as_float());
                newSprite->SetPosition(pos - parent->GetPosition());
                
                parent->AddChild(newSprite);
                
                m_nodes[newSprite->GetName()] = newSprite;
            }
            
        }
        child = child.previous_sibling();
    }
}