/*
 *  CActionSimultaneously.cpp
 *  Wooliz
 *
 *  Created by Anton Zagovalko on 12.07.11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "CActionSimultaneously.h"
#include <stdarg.h>

CActionSimultaneously::CActionSimultaneously(CAction* first, CAction* next, ...)
{
	m_actions.push_back(first);
    if (next)
    {
        m_actions.push_back(next);
        va_list uk_arg;
        va_start( uk_arg, next );
        CAction* p;
        while ( ( p = va_arg( uk_arg, CAction* ) ) )
        {
            m_actions.push_back( p );
        }
        va_end( uk_arg );
    }
	
    CheckForLinks();
}

CActionSimultaneously::CActionSimultaneously(const std::vector< CAction* >& els)
{
    m_actions.resize(els.size());
    std::copy(els.begin(), els.end(), m_actions.begin());
    CheckForLinks();
}

CActionSimultaneously::CActionSimultaneously(const CActionSimultaneously& action):
CAction(action)
{
    std::list< CAction* >::const_iterator it = action.m_actions.begin();
	for (; it != action.m_actions.end(); it++)
	{
        m_actions.push_back((*it)->GetCopy());
    }
    CheckForLinks();
}

CActionSimultaneously::~CActionSimultaneously()
{
	std::list< CAction* >::iterator i = m_actions.begin();
	for (; i != m_actions.end(); i++)
	{
		delete *i;
        (*i) = NULL;
	}	
}

void CActionSimultaneously::CheckForLinks()
{
    std::list< CAction* >::iterator i = m_actions.begin();
	for (; i != m_actions.end(); i++)
	{
		CAction* act = *i;
        CopyLinksFromAction(act);
	}
}

void CActionSimultaneously::Update(float dt)
{
    if(IsDelete())
        return;
	if (m_actions.size() == 0) Delete();
	std::list< CAction* >::iterator j, i = m_actions.begin();
	for (; i != m_actions.end();)
	{
		(*i)->Update(dt);
		if ((*i)->IsDelete()) 
		{
            j = i;
            i++;
			RemoveAction(*j);
		}
		else 
			i++;
	}	
}

void CActionSimultaneously::RemoveAction(CAction* action)
{
	std::list< CAction* >::iterator i = m_actions.begin();
	for (; i != m_actions.end(); i++)
	{
		if (*i == action)
		{
            const std::list<CNode*>& linkNodes = action->GetLinkNodes();
            for (std::list< CNode* >::const_iterator it = linkNodes.begin(); it != linkNodes.end(); it++)
                RemoveLink((*it));
            
			delete *i;
            (*i) = NULL;
			m_actions.erase(i);
			return;
		}
	}
}

CAction* CActionSimultaneously::GetCopy()
{
	CActionSimultaneously* res = new CActionSimultaneously(*this);
	return res;
}

CAction* CActionSimultaneously::GetReversCopy()
{
	CActionSimultaneously* res = new CActionSimultaneously(*this);
    std::list< CAction* >::iterator i, j;
    i = m_actions.begin();
    j = res->m_actions.begin();
	for (; i != m_actions.end(); i++, j++)
	{
        delete *j;
        *j = (*i)->GetReversCopy();
	}
    res->CheckForLinks();
	return res;
}

CAction* CActionSimultaneously::Add(CAction* act)
{
    m_actions.push_back( act );
    CopyLinksFromAction(act);
    return this;
}


float CActionSimultaneously::GetDurationTime()
{
	float max = 0;
	std::list< CAction* >::iterator i = m_actions.begin();
	for (; i != m_actions.end(); i++)
	{
		float time = (*i)->GetDurationTime();
		if (time < 0) return -1;
		max = (max > time) ? max : time;
	}
	
	return max;
}
