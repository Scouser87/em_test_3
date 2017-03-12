/*
 *  CActonAcceleration.cpp
 *  Ananas
 *
 *  Created by Anton Zagovalko on 30.01.12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "CActionAcceleration.h"


CActionAcceleration::CActionAcceleration(CAction* act, float coefStart, float coefEnd, float time)
:m_action(      act),
 m_curCoef(     coefStart)
{    
    m_changer = new ChangerType(this, &CActionAcceleration::SetCoef, coefStart, coefEnd, (time != 0) ? time : act->GetDurationTime());
}

CActionAcceleration::~CActionAcceleration()
{
    delete m_action;
    delete m_changer;
}

void CActionAcceleration::Update(float dt)
{
    if(IsDelete())
        return;
    if (!m_changer->IsDelete())
        m_changer->Update(dt);
    
    m_action->Update(dt * m_curCoef);
    if (m_action->IsDelete())
        Delete();
}

CAction* CActionAcceleration::GetCopy()
{
    CActionAcceleration* newAct = new CActionAcceleration(*this);
    newAct->m_changer = (ChangerType*)m_changer->GetCopy();
    return newAct;
}

CAction* CActionAcceleration::GetReversCopy()
{
    CActionAcceleration* newAct = new CActionAcceleration(*this);
    newAct->m_changer = (ChangerType*)m_changer->GetReversCopy();
    return newAct;
}

float CActionAcceleration::GetDurationTime()
{
    return m_action->GetDurationTime();
}

CActionAcceleration* CActionAcceleration::SetCoef(const float& coef)
{
    m_curCoef = coef;
    return this;
}


/////////////////////////////////////////////////////////////////////////////////

CActionAccelerationTimeSave::CActionAccelerationTimeSave(CAction* act, float coef)
:m_action(	act),
m_coef(		coef),
m_time(		0),
m_prevTime(	0),
m_timeMax(	act->GetDurationTime())
{
	if (m_coef == 0) m_coef = 1;
	else if (fabsf(m_coef) < 1.0) m_coef = -(1 / m_coef);	
}

CActionAccelerationTimeSave::~CActionAccelerationTimeSave()
{
	delete m_action;
}

void CActionAccelerationTimeSave::Update(float dt)
{
	m_prevTime = m_time;
	m_time += dt;
    if (m_time >= m_timeMax)
    {
        Delete();
        m_time = m_timeMax;
    }
	m_action->Update(TimeFunc(m_time) - TimeFunc(m_prevTime));	
}

float CActionAccelerationTimeSave::TimeFunc(float t)
{
	if (m_coef > 0)
	{
		return pow(t, m_coef) / pow(m_timeMax, m_coef-1);
	}
	else
	{
		return m_timeMax - pow((m_timeMax - t), -m_coef) / pow(m_timeMax, -m_coef-1);
	}
}

CAction* CActionAccelerationTimeSave::GetCopy()
{
	CActionAccelerationTimeSave* res = new CActionAccelerationTimeSave(*this);
	res->m_action = res->m_action->GetCopy();
	return res;
}

CAction* CActionAccelerationTimeSave::GetReversCopy()
{
	CActionAccelerationTimeSave* res = (CActionAccelerationTimeSave*)GetCopy();
	res->m_coef = -res->m_coef;
	return res;
}

float CActionAccelerationTimeSave::GetDurationTime()
{
	return m_timeMax;
}
