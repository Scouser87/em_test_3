//
//  CElipticalMovement.cpp
//  Pirrrates
//
//  Created by Andrey Sulnichenko on 04.05.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "CElipticalMovement.h"


CElipticalMovement::CElipticalMovement(const CElipticalMovement& act)
:CAction(act),
m_object(act.m_object),
m_time(act.m_time),
m_timeMax(act.m_timeMax),
m_center(act.m_center),
m_radius(act.m_radius),
m_start(act.m_start),
m_finish(act.m_finish)
{
}

CElipticalMovement::CElipticalMovement(CSprite* node, vec2 center, vec2 radius, float start, float finish, float time, float speed)
{
    m_object = node;
    m_center = center;
    m_radius = radius;
    m_start = start;
    m_finish = finish;
    m_timeMax = (speed) ? fabsf(finish - start)/speed : time;
    m_time = 0;

    if (m_timeMax < 0) Delete();
    
    AddLink(dynamic_cast< CNode* >(node));
}

void CElipticalMovement::Update(float dt)																		
{
    m_time += dt;
    float pathProc = m_time / m_timeMax;
    
    if (pathProc >= 1.0f)
    {
        float angle = m_finish;
        float x = m_center.x + sin(angle) * m_radius.x;
        float y = m_center.y + cos(angle) * m_radius.y;
        m_object->SetPosition(vec2(x, y));
        Delete();
        return;
    }
    
    float angle = m_start + (m_finish - m_start) * pathProc;
    float x = m_center.x + sin(angle) * m_radius.x;
    float y = m_center.y + cos(angle) * m_radius.y;
    m_object->SetPosition(vec2(x, y));
}

CAction* CElipticalMovement::GetCopy()
{
    return new CElipticalMovement(*this);
}

CAction* CElipticalMovement::GetReversCopy()		        
{
    CElipticalMovement* res = (CElipticalMovement*)GetCopy();
    float start = res->m_start;
    res->m_start = res->m_finish;
    res->m_finish = start;
    return res;
}	

float CElipticalMovement::GetDurationTime()	
{
    return m_timeMax;
}
