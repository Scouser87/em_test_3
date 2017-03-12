//
//  GameScene.cpp
//  em_test_3
//
//  Created by Sergei Makarov on 12.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#include "GameScene.hpp"

CGameScene::CGameScene()
{
    sp = new CSprite("resources/2.png");
    sp->SetPosition(GetScreenSize()*0.5f);
    AddChild(sp);
}

bool CGameScene::TouchBegin(intptr_t ID, vec2& pos, bool isPrecessed)
{
    sp->SetPosition(pos);
    return isPrecessed;
}

bool CGameScene::TouchMove(intptr_t ID, vec2& pos, bool isPrecessed)
{
    sp->SetPosition(pos);
    return isPrecessed;
}

bool CGameScene::TouchEnd(intptr_t ID, vec2& pos, bool isPrecessed)
{
    sp->SetPosition(pos);
    return isPrecessed;
}

void CGameScene::Update(float dt)
{
    sp->SetAngle(sp->GetAngle() + dt);
}