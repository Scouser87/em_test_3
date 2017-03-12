//
//  CActionElipticalMovement.h
//  Pirrrates
//
//  Created by Andrey Sulnichenko on 04.05.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "CAction.h"
#include "../CSprite.h"

// класс, обеспечивающий движение по эллипсу
class CElipticalMovement: public CAction
{
public:
	
public:
	CElipticalMovement(const CElipticalMovement& act);
    CElipticalMovement(CSprite* node, vec2 center, vec2 radius, float start, float finish, float time, float speed = 0);
   
    void Update(float dt);
    CAction*						GetCopy();
    CAction*						GetReversCopy();
    float							GetDurationTime();
protected:
    CSprite*      m_object;
	float	m_time;
	float	m_timeMax;
    
    vec2 m_center; //центр эллипса
    vec2 m_radius; // радиус эллипса
    float m_start; // начальный угол
    float m_finish; // конечный угол
};
