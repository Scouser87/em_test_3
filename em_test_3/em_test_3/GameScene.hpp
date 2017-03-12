//
//  GameScene.hpp
//  em_test_3
//
//  Created by Sergei Makarov on 12.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#ifndef GameScene_hpp
#define GameScene_hpp

#include "CSprite.h"

class CGameScene : public CSprite
{
public:
    CGameScene();
    
    bool TouchBegin(intptr_t ID, vec2& pos, bool isPrecessed);
    bool TouchMove(intptr_t ID, vec2& pos, bool isPrecessed);    
    bool TouchEnd(intptr_t ID, vec2& pos, bool isPrecessed);
    
    void Update(float dt);
    
private:
    CSprite* sp;
};

#endif /* GameScene_hpp */
