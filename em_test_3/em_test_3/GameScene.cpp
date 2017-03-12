//
//  GameScene.cpp
//  em_test_3
//
//  Created by Sergei Makarov on 12.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#include "GameScene.hpp"
#include "CActionModifiers.h"
#include "GraphicsLoader.hpp"

CGameScene::CGameScene()
{
//    sp = new CSprite("resources/2.png");
//    sp->SetPosition(GetScreenSize()*0.5f);
    //    AddChild(sp);
    
    GraphicsLoader* skin = new GraphicsLoader("resources/res/game_75/skin.xsp");
    AddChild(skin);
    skin->SetPosition(GetScreenSize()*0.5f);
    
    GraphicsLoader* gloder = new GraphicsLoader("resources/res/TopPanel/TopPanel.xsp");
    gloder->SetPosition(vec2(GetScreenSize().x*0.5f, GetScreenSize().y - gloder->GetSize().y*0.5f));
    gloder->SetScale(vec2(GetScreenSize().x/gloder->GetSize().x, 1.0f));
    AddChild(gloder);
    
    
    CSprite* rect = new CSprite(sColor(1, 0.5, 0.3));
    rect->SetPosition(vec2(100, 100));
    rect->SetSize(vec2(100, 50));
    AddChild(rect);
    
    AddAction(ActionMoveTo(rect, vec2(700, 600), 3.0f));
    AddAction(ActionChangeColor(rect, sColor(1, 0.5, 0.3), sColor(0.3, 0.5, 1), 3.0f));
    AddAction(ActionRotate(rect, 0, M_PI, 3.0f));
}

bool CGameScene::TouchBegin(intptr_t ID, vec2& pos, bool isPrecessed)
{
    return isPrecessed;
}

bool CGameScene::TouchMove(intptr_t ID, vec2& pos, bool isPrecessed)
{
    return isPrecessed;
}

bool CGameScene::TouchEnd(intptr_t ID, vec2& pos, bool isPrecessed)
{
    return isPrecessed;
}

void CGameScene::Update(float dt)
{
    ;
}