/*
 *  CActionWaiting.cpp
 *  Wooliz
 *
 *  Created by Anton Zagovalko on 12.07.11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "CActionWaiting.h"
#include "../CDirector.h"

CActionWaiting::CActionWaiting(float time)
: m_timeMax(time),
  m_curTime(0)
{
}

void CActionWaiting::Update(float dt)
{
	m_curTime += dt;
	if (m_curTime >= m_timeMax) Delete();
}

CAction* CActionWaiting::GetCopy()
{
	return new CActionWaiting(*this);	
}

float CActionWaiting::GetDurationTime()
{
	return (m_timeMax < 0) ? 0 :m_timeMax;
}

float CActionWaiting::GetTime()
{
	return m_curTime;
}
void CActionWaiting::AddTime(float time)
{
    m_timeMax += time;
}
void CActionWaiting::SetTime(float time)
{
    m_curTime = 0;
    m_timeMax = time;
}

CActionWaitingFor::CActionWaitingFor(CAction* act)
:m_action(		act),
 m_func(		NULL)
{
}

void CActionWaitingFor::Update(float dt)
{
    if(IsDelete())
        return;
	if (m_action && !CDirector::GetDirector().GetActionManager().IsActionExist(m_action)) Delete();
	else if (m_func && (m_node->*m_func)()) Delete();
}

CAction* CActionWaitingFor::GetCopy()
{
	return new CActionWaitingFor(*this);
}

float CActionWaitingFor::GetDurationTime()
{
	return -1;
}
