/*
 *  CDirector.h
 *  Ananas
 *
 *  Created by Anton Zagovalko on 25.01.12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

//Dynamic calculating graphics level
//#define GRAPHICS_LEVEL
//#define AVERAGE_WEIGHT 800

#include <set>
#include "algebra3.h"

class CActionMngr;
class CNode;

class CDirector
{
    static CDirector* s_director;
    
    CDirector();
    ~CDirector();
    
public:
    static CDirector&           GetDirector();
    static CActionMngr&         GetActionManager();
    
    void            Init(float x, float y);
    void			Update();
    void			TouchBegin(intptr_t ID, vec2 pos);
    void			TouchMove(intptr_t ID, vec2 pos);
    void			TouchEnd(intptr_t ID, vec2 pos);
    void			TouchCancel(intptr_t ID, vec2 pos);
    void            ResetTouches();
    void			SetScreenSize(vec2 size);
    
    CNode*          GetRooot() { return m_root; }
    
    void DeleteNode(CNode* node);
    
private:
    void    CheckDeleteNodes();
    double  CalculateDT();
    
private:
    CActionMngr*        m_actionMngr;
    
    CNode*              m_root;
    std::set<CNode*>    m_nodesToDelete;
    
    double              m_prevTime;
    vec2                m_canvasSize;
    
};