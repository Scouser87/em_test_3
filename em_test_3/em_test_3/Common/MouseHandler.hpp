//
//  MouseHandler.hpp
//  em_test_3
//
//  Created by Sergei Makarov on 10.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#ifndef MouseHandler_hpp
#define MouseHandler_hpp

#include <stdio.h>
#include "algebra3.h"

class MouseHandler
{
public:
    enum eMouseState
    {
        eMS_up,
        eMS_down
    };
    
    MouseHandler();
    
    const vec2& GetCanvasPos() { return m_canvasPos; }
    const eMouseState& GetState() { return m_state; }
    
    void SetCanvasPos(float x, float y);
    void SetState(eMouseState state) { m_state = state; }
    
private:
    vec2 m_canvasPos;
    eMouseState m_state;
};

#endif /* MouseHandler_hpp */
