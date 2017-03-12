/*
 *  CActionRepeate.cpp
 *  Wooliz
 *
 *  Created by Anton Zagovalko on 12.07.11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "CActionRepeate.h"

CActionRepeate::CActionRepeate(CAction* action, int count)
: m_action(action),
  m_countMax(count),
  m_counter(0)
{	
	m_actionCopy = m_action->GetCopy();
    CopyLinksFromAction(m_action);
}

CActionRepeate::CActionRepeate(const CActionRepeate& action)
:CAction(action)
{
	m_action = action.m_action->GetCopy();
	m_actionCopy = action.m_actionCopy->GetCopy();
    CopyLinksFromAction(m_action);
}

CActionRepeate::~CActionRepeate()
{
	delete m_action;
	delete m_actionCopy;
}

void CActionRepeate::Update(float dt)
{
    if(IsDelete())
        return;
	m_actionCopy->Update(dt);
	if (m_actionCopy->IsDelete())
	{		
		if (m_countMax > 0)
		{
			m_counter++;
			if (m_countMax <= m_counter)
			{
				Delete();
				return;
			}
		}
		
		delete	m_actionCopy;
		m_actionCopy = m_action->GetCopy();
	}		
}

CAction* CActionRepeate::GetCopy()
{
	return new CActionRepeate(*this);
}

float CActionRepeate::GetDurationTime()
{
	if (m_countMax <= 0) return -1;
	
	return m_action->GetDurationTime() * m_countMax;
}
