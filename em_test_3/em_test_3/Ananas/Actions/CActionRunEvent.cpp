/*
 *  CActionRunEvent.cpp
 *  Casino
 *
 *  Created by Anton Zagovalko on 30.01.12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "CActionRunEvent.h"

CActionRunEvent::CActionRunEvent()
:m_data(		NULL),
m_obj(      NULL)
{
    ;
}

CActionRunEvent::CActionRunEvent(const CActionRunEvent& act):CAction(act),
m_event(act.m_event),
m_data(act.m_data),
m_obj(act.m_obj)
{
    
}

CActionRunEvent::~CActionRunEvent()
{
    ;
}

CActionRunEvent::CActionRunEvent(const CEvent& ev)
:m_data(		NULL)
{
    m_event = ev;
}

CActionRunEvent* CActionRunEvent::SetData(void* data)
{
	m_data = data;
	return this;
}

void CActionRunEvent::Update(float dt)
{
	if (IsDelete()) return;
	m_event.RunEvent(m_data);
	Delete();
}

CAction* CActionRunEvent::GetCopy()
{
	return new CActionRunEvent(*this);
}

float CActionRunEvent::GetDurationTime()
{
	return 0;
}